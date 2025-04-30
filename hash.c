#include "hash.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

#define COMMAND_BUFFER_SIZE 2048

int hashFile(const char* source, const char* dest) {
    char command[COMMAND_BUFFER_SIZE];
    int needed = snprintf(command, sizeof(command), "sha256sum \"%s\" > \"%s\"", source, dest);
    if (needed >= sizeof(command)) {
        fprintf(stderr, "Chemin trop long pour la commande de hachage\n");
        return -1;
    }
    return system(command);
}

char* sha256file(const char* file) {
    if (!file) return NULL;

    char tmp[] = "/tmp/hashXXXXXX";
    char tmp_path[PATH_MAX];
    strncpy(tmp_path, tmp, sizeof(tmp_path));
    tmp_path[PATH_MAX-1] = '\0';

    int fd = mkstemp(tmp_path);
    if (fd == -1) return NULL;

    char command[COMMAND_BUFFER_SIZE];
    int needed = snprintf(command, sizeof(command), 
                      "sha256sum \"%s\" | cut -d' ' -f1 > \"%s\"", 
                      file, tmp_path);
    if (needed >= sizeof(command)) {
        close(fd);
        unlink(tmp_path);
        fprintf(stderr, "Chemin trop long pour la commande sha256sum\n");
        return NULL;
    }

    if (system(command) != 0) {
        close(fd);
        unlink(tmp_path);
        return NULL;
    }

    FILE* f = fopen(tmp_path, "r");
    if (!f) {
        close(fd);
        unlink(tmp_path);
        return NULL;
    }

    char* hash = malloc(65);
    if (!hash) {
        fclose(f);
        close(fd);
        unlink(tmp_path);
        return NULL;
    }

    if (fscanf(f, "%64s", hash) != 1) {
        free(hash);
        hash = NULL;
    }
    fclose(f);
    close(fd);
    unlink(tmp_path);
    return hash;
}

List* listdir(const char* root_dir) {
    if (!root_dir) return NULL;

    List* L = initList();
    if (!L) return NULL;

    DIR* dp = opendir(root_dir);
    if (!dp) {
        freeList(L);
        return NULL;
    }

    struct dirent* ep;
    while ((ep = readdir(dp))) {
        if (strcmp(ep->d_name, ".") && strcmp(ep->d_name, "..")) {
            Cell* new_cell = buildCell(ep->d_name);
            if (new_cell) {
                insertFirst(L, new_cell);
            }
        }
    }
    closedir(dp);
    return L;
}

int file_exists(const char* file) {
    return file && (access(file, F_OK) == 0);
}

void cp(const char* to, const char* from) {
    if (!file_exists(from)) return;
    
    FILE* src = fopen(from, "rb");
    if (!src) return;
    
    FILE* dst = fopen(to, "wb");
    if (!dst) {
        fclose(src);
        return;
    }

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes, dst);
    }

    fclose(src);
    fclose(dst);
}

char* hashToPath(const char* hash) {
    if (!hash || strlen(hash) < 3) return NULL;
    
    char* path = malloc(HASH_PATH_SIZE);
    if (!path) return NULL;
    
    int needed = snprintf(path, HASH_PATH_SIZE, "%.2s/%.64s", hash, hash + 2);
    if (needed >= HASH_PATH_SIZE) {
        free(path);
        return NULL;
    }
    return path;
}

int blobFile(const char* file) {
    if (!file_exists(file)) return -1;

    char* hash = sha256file(file);
    if (!hash) return -1;

    char* path = hashToPath(hash);
    if (!path) {
        free(hash);
        return -1;
    }

    char dir[3];
    strncpy(dir, path, 2);
    dir[2] = '\0';

    mkdir(dir, 0700);
    cp(path, file);

    free(hash);
    free(path);
    return 0;
}
