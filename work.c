#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "work.h"

// --- Manipulation de WorkFile ---

WorkFile* createWorkFile(char* name) {
    WorkFile* wf = (WorkFile*)malloc(sizeof(WorkFile));
    wf->name = name ? strdup(name) : NULL;
    wf->hash = NULL;
    wf->mode = 0;
    return wf;
}

char* wfts(WorkFile* wf) {
    if (!wf || !wf->name) return strdup("");
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s\t%s\t%d", 
             wf->name, 
             wf->hash ? wf->hash : "", 
             wf->mode);
    return strdup(buffer);
}

WorkFile* stwf(char* ch) {
    if (!ch) return NULL;
    WorkFile* wf = createWorkFile(NULL);
    char* token = strtok(ch, "\t");
    if (token) wf->name = strdup(token);
    token = strtok(NULL, "\t");
    if (token && strlen(token) > 0) wf->hash = strdup(token);
    token = strtok(NULL, "\t");
    if (token) sscanf(token, "%d", &wf->mode);
    return wf;
}

// --- Manipulation de WorkTree ---

WorkTree* initWorkTree() {
    WorkTree* wt = (WorkTree*)malloc(sizeof(WorkTree));
    wt->tab = (WorkFile*)calloc(MAX_FILES, sizeof(WorkFile));
    wt->size = MAX_FILES;
    wt->n = 0;
    return wt;
}

int inWorkTree(WorkTree* wt, char* name) {
    if (!wt || !name) return -1;
    for (int i = 0; i < wt->n; i++) {
        if (wt->tab[i].name && strcmp(wt->tab[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode) {
    if (!wt || !name || inWorkTree(wt, name) != -1) return -1;
    if (wt->n >= wt->size) {
        wt->size *= 2;
        wt->tab = (WorkFile*)realloc(wt->tab, wt->size * sizeof(WorkFile));
    }
    WorkFile* wf = &wt->tab[wt->n++];
    wf->name = strdup(name);
    wf->hash = hash ? strdup(hash) : NULL;
    wf->mode = mode;
    return 0;
}

char* wtts(WorkTree* wt) {
    if (!wt || wt->n == 0) return strdup("");
    size_t totalLen = 0;
    char** lines = (char**)malloc(wt->n * sizeof(char*));
    for (int i = 0; i < wt->n; i++) {
        lines[i] = wfts(&wt->tab[i]);
        totalLen += strlen(lines[i]) + 1; // +1 pour '\n'
    }
    char* result = (char*)malloc(totalLen);
    result[0] = '\0';
    for (int i = 0; i < wt->n; i++) {
        strcat(result, lines[i]);
        if (i < wt->n - 1) strcat(result, "\n");
        free(lines[i]);
    }
    free(lines);
    return result;
}

WorkTree* stwt(char* ch) {
    WorkTree* wt = initWorkTree();
    if (!ch || !*ch) return wt;
    char* copy = strdup(ch);
    char* line = strtok(copy, "\n");
    while (line) {
        WorkFile* wf = stwf(line);
        appendWorkTree(wt, wf->name, wf->hash, wf->mode);
        free(wf->name);
        if (wf->hash) free(wf->hash);
        free(wf);
        line = strtok(NULL, "\n");
    }
    free(copy);
    return wt;
}

int wttf(WorkTree* wt, char* file) {
    FILE* f = fopen(file, "w");
    if (!f) return -1;
    char* str = wtts(wt);
    fprintf(f, "%s", str);
    free(str);
    fclose(f);
    return 0;
}

WorkTree* ftwt(char* file) {
    FILE* f = fopen(file, "r");
    if (!f) return initWorkTree();
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    char* buffer = (char*)malloc(size + 1);
    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);
    WorkTree* wt = stwt(buffer);
    free(buffer);
    return wt;
}

// --- Enregistrement et restauration ---

char* blobWorkTree(WorkTree* wt) {
    char tmpTemplate[] = "/tmp/worktreeXXXXXX";
    char tmpPath[100];
    strcpy(tmpPath, tmpTemplate);
    int fd = mkstemp(tmpPath);
    if (fd == -1) return NULL;
    close(fd);

    if (wttf(wt, tmpPath) != 0) {
        unlink(tmpPath);
        return NULL;
    }

    char* hash = sha256file(tmpPath);
    if (!hash) {
        unlink(tmpPath);
        return NULL;
    }

    char* path = hashToPath(hash);
    char dir[3];
    snprintf(dir, 3, "%.2s", hash);
    struct stat st = {0};
    if (stat(dir, &st) == -1) mkdir(dir, 0700);

    char blobPath[70];
    snprintf(blobPath, 70, "%s.t", path);
    cp(blobPath, tmpPath);

    unlink(tmpPath);
    free(path);
    return hash;
}

char* saveWorkTree(WorkTree* wt, char* path) {
    struct stat st;
    for (int i = 0; i < wt->n; i++) {
        WorkFile* wf = &wt->tab[i];
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, wf->name);

        if (stat(fullPath, &st) == -1) continue;

        if (S_ISREG(st.st_mode)) { // Fichier régulier
            blobFile(fullPath);
            free(wf->hash);
            wf->hash = sha256file(fullPath);
            wf->mode = getChmod(fullPath);
        } else if (S_ISDIR(st.st_mode)) { // Répertoire
            WorkTree* subWt = initWorkTree();
            List* dirList = listdir(fullPath);
            Cell* current = *dirList;
            while (current) {
                char subPath[1024];
                size_t fullPathLen = strlen(fullPath);
                size_t dataLen = strlen(current->data);
                size_t totalLen = fullPathLen + 1 + dataLen + 1; // +1 pour '/', +1 pour '\0'
                if (totalLen > sizeof(subPath)) {
                    fprintf(stderr, "Erreur : chemin trop long pour %s/%s\n", fullPath, current->data);
                    current = current->next;
                    continue;
                }
                snprintf(subPath, totalLen, "%s/%s", fullPath, current->data); // Utilise totalLen, pas sizeof(subPath)
                appendWorkTree(subWt, current->data, NULL, 0);
                current = current->next;
            }
            freeList(dirList);
            free(wf->hash);
            wf->hash = saveWorkTree(subWt, fullPath);
            wf->mode = getChmod(fullPath);

            // Libération mémoire
            for (int j = 0; j < subWt->n; j++) {
                free(subWt->tab[j].name);
                if (subWt->tab[j].hash) free(subWt->tab[j].hash);
            }
            free(subWt->tab);
            free(subWt);
        }
    }
    return blobWorkTree(wt);
}

void restoreWorkTree(WorkTree* wt, char* path) {
    for (int i = 0; i < wt->n; i++) {
        WorkFile* wf = &wt->tab[i];
        char* hashPath = hashToPath(wf->hash);
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, wf->name);

        char blobPath[70];
        snprintf(blobPath, 70, "%s.t", hashPath);
        if (file_exists(blobPath)) { // Répertoire
            WorkTree* subWt = ftwt(blobPath);
            mkdir(fullPath, 0700);
            setMode(wf->mode, fullPath);
            restoreWorkTree(subWt, fullPath);

            // Libération mémoire
            for (int j = 0; j < subWt->n; j++) {
                free(subWt->tab[j].name);
                if (subWt->tab[j].hash) free(subWt->tab[j].hash);
            }
            free(subWt->tab);
            free(subWt);
        } else { // Fichier
            cp(fullPath, hashPath);
            setMode(wf->mode, fullPath);
        }
        free(hashPath);
    }
}

// --- Fonctions utilitaires (déjà dans l'énoncé) ---

int getChmod(const char* path) {
    struct stat ret;
    if (stat(path, &ret) == -1) return -1;
    return (ret.st_mode & S_IRUSR) | (ret.st_mode & S_IWUSR) | (ret.st_mode & S_IXUSR) |
           (ret.st_mode & S_IRGRP) | (ret.st_mode & S_IWGRP) | (ret.st_mode & S_IXGRP) |
           (ret.st_mode & S_IROTH) | (ret.st_mode & S_IWOTH) | (ret.st_mode & S_IXOTH);
}

void setMode(int mode, char* path) {
    char buff[100];
    sprintf(buff, "chmod %o %s", mode, path);
    system(buff);
}
