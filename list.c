#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

List* initList() {
    List* L = malloc(sizeof(List));
    if (L) *L = NULL;
    return L;
}

Cell* buildCell(char* ch) {
    if (!ch) return NULL;
    Cell* c = malloc(sizeof(Cell));
    if (!c) return NULL;
    c->data = strdup(ch); //dupliquer une chaine pour eviter de modifier l'origine
    if (!c->data) {
        free(c);
        return NULL;
    }
    c->next = NULL;
    return c;
}

void insertFirst(List* L, Cell* C) {
    if (!L || !C) return;
    C->next = *L;
    *L = C;
}

char* ctos(Cell* c) {
    return c ? strdup(c->data) : NULL;
}

char* ltos(List* L) {
    if (!L || !*L) return strdup("");
    
    size_t total_size = 1; // pour null 
    Cell* current = *L;
    while (current) {
        total_size += strlen(current->data) + 1; // +1 pour separateur
        current = current->next;
    }

    char* result = malloc(total_size);
    if (!result) return NULL;
    result[0] = '\0';

    current = *L;
    while (current) {
        strcat(result, current->data);
        if (current->next) strcat(result, "|"); //concaténation
        current = current->next;
    }
    return result;
}

Cell* listGet(List* L, int i) {
    if (!L) return NULL;
    Cell* current = *L;
    for (int pos = 0; current && pos < i; pos++)
        current = current->next;
    return current;
}

Cell* searchList(List* L, char* str) {
    if (!L || !str) return NULL;
    Cell* current = *L;
    while (current) {
        if (strcmp(current->data, str) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

List* stol(char* s) {
    if (!s) return NULL;
    List* L = initList();
    if (!L) return NULL;
    
    char* copy = strdup(s);
    if (!copy) {
        freeList(L);
        return NULL;
    }
    
    char* token = strtok(copy, "|");
    while (token) {
        Cell* new_cell = buildCell(token);
        if (!new_cell) {
            free(copy);
            freeList(L);
            return NULL;
        }
        insertFirst(L, new_cell);
        token = strtok(NULL, "|");
    }
    free(copy);
    return L;
}

void ltof(List* L, char* path) {
    if (!L || !path) return;
    FILE* f = fopen(path, "w");
    if (!f) return;
    
    char* s = ltos(L);
    if (s) {
        fprintf(f, "%s", s);
        free(s);
    }
    fclose(f);
}

List* ftol(char* path) {
    if (!path) return NULL;
    FILE* f = fopen(path, "r");
    if (!f) return initList();
    
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = malloc(fsize + 1);
    if (!buffer) {
        fclose(f);
        return initList();
    }
    
    fread(buffer, 1, fsize, f);
    buffer[fsize] = '\0';
    fclose(f);
    
    List* L = stol(buffer);
    free(buffer);
    return L ? L : initList();
}

void freeList(List* L) {
    if (!L) return;
    
    Cell* current = *L;
    while (current) {
        Cell* tmp = current;
        current = current->next;
        free(tmp->data);
        free(tmp);
    }
    free(L);
}
