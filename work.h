#ifndef WORK_H
#define WORK_H

#include "hash.h"  // Pour les fonctions de hachage et gestion de fichiers
#include "list.h"  // Pour la structure List

// Structure représentant un fichier ou répertoire à versionner
typedef struct {
    char* name;  // Nom du fichier ou répertoire
    char* hash;  // Hash SHA-256 du contenu (NULL si non calculé)
    int mode;    // Permissions en octal (ex. 0644)
} WorkFile;

// Structure représentant un ensemble de WorkFile
typedef struct {
    WorkFile* tab;  // Tableau dynamique de WorkFile
    int size;       // Taille allouée du tableau
    int n;          // Nombre d'éléments actuels
} WorkTree;

// Constantes
#define MAX_FILES 100  // Taille initiale par défaut du WorkTree

// Manipulation de WorkFile
WorkFile* createWorkFile(char* name);
char* wfts(WorkFile* wf);
WorkFile* stwf(char* ch);

// Manipulation de WorkTree
WorkTree* initWorkTree();
int inWorkTree(WorkTree* wt, char* name);
int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode);
char* wtts(WorkTree* wt);
WorkTree* stwt(char* ch);
int wttf(WorkTree* wt, char* file);
WorkTree* ftwt(char* file);

// Enregistrement et restauration
char* blobWorkTree(WorkTree* wt);
char* saveWorkTree(WorkTree* wt, char* path);
void restoreWorkTree(WorkTree* wt, char* path);

// Fonctions utilitaires (déjà fournies dans l'énoncé)
int getChmod(const char* path);
void setMode(int mode, char* path);

#endif // WORK_H
