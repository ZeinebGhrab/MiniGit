/*
 * Ce programme teste toutes les fonctionnalités implémentées :
 * - Hachage de fichiers (Chapitre 1)
 * - Gestion de listes chaînées
 * - Système de blobs
 * - Structures WorkFile/WorkTree (Chapitre 2)
 * - Sauvegarde et restauration d'arbres
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "list.h"
#include "work.h"

int main() {
    // ==========================================
    // SECTION : Tests du Chapitre 1 (Fonctions de base)
    // ==========================================
    printf("\n=== Tests du Chapitre 1 ===\n");

    /**
     Test de hashFile : Vérifie que le hachage d'un fichier est correctement calculé et stocké
     */
    printf("\n[TEST] hashFile :\n");
    hashFile("main.c", "hash_output.txt");
    FILE* f = fopen("hash_output.txt", "r");
    if (f) {
        char hash[65];
        fscanf(f, "%64s", hash);
        printf("HASH DE main.c : %s\n", hash);
        fclose(f);
    }

    /**
     Test de sha256file : Vérifie que le hachage en mémoire est cohérent avec hashFile
     */
    printf("\n[TEST] sha256file :\n");
    char* hash2 = sha256file("main.c");
    if (hash2) {
        printf("HASH MEMOIRE : %s\n", hash2);
    }

    /**
     Test des listes chaînées : Vérifie le bon fonctionnement des opérations sur les listes
     */
    printf("\n[TEST] Liste chaînée :\n");
    List* L = initList();
    insertFirst(L, buildCell("fichier1"));
    insertFirst(L, buildCell("fichier2"));
    
    // Test sérialisation
    char* list_str = ltos(L);
    if (list_str) {
        printf("LISTE SERIALISEE : %s\n", list_str);
        ltof(L, "list.txt"); // Sauvegarde dans un fichier
        free(list_str);
    }

    // Test désérialisation
    List* L2 = ftol("list.txt");
    char* list_str2 = ltos(L2);
    if (list_str2) {
        printf("LISTE LUE : %s\n", list_str2);
        free(list_str2);
    }

    /** 
    Test de blobFile : Vérifie la création d'un instantané de fichier
     */
    printf("\n[TEST] blobFile :\n");
    blobFile("main.c");
    if (hash2) {
        printf("INSTANTANE CREE. Verifiez le repertoire %.2s/\n", hash2);
        free(hash2);
    }

    // ==========================================
    // SECTION : Tests du Chapitre 2 (Structures avancées)
    // ==========================================
    printf("\n=== Tests du Chapitre 2 ===\n");

    /** 
    Test de WorkFile : Vérifie la création et sérialisation d'un WorkFile
     */
    printf("\n[TEST] WorkFile :\n");
    WorkFile* wf = createWorkFile("test.txt");
    wf->hash = strdup("abcd1234");
    wf->mode = 0644;
    
    char* wf_str = wfts(wf);
    printf("WORKFILE : %s\n", wf_str);

    // Test désérialisation
    WorkFile* wf2 = stwf(wf_str);
    printf("WORKFILE RECONSTRUIT : %s\n", wfts(wf2));

    /**
    Test de WorkTree : Vérifie la gestion d'un arbre de fichiers
     */
    printf("\n[TEST] WorkTree :\n");
    WorkTree* wt = initWorkTree();
    appendWorkTree(wt, "test.txt", NULL, 0644);
    appendWorkTree(wt, "main.c", NULL, 0644);

    // Création d'un fichier de test si inexistant
    if (!file_exists("test.txt")) {
        FILE* f1 = fopen("test.txt", "w");
        fprintf(f1, "Contenu de test\n");
        fclose(f1);
    }

    char* wt_str = wtts(wt);
    printf("WORKTREE INITIAL : %s\n", wt_str);

    /**
     Test de saveWorkTree : Vérifie la sauvegarde complète d'un arbre
     */
    printf("\n[TEST] saveWorkTree :\n");
    char* wt_hash = saveWorkTree(wt, ".");
    if (wt_hash) {
        printf("HASH WORKTREE : %s\n", wt_hash);
    }

    // Sauvegarde dans un fichier
    wttf(wt, "worktree_snapshot.txt");
    WorkTree* wt_restored = ftwt("worktree_snapshot.txt");
    char* wt_restored_str = wtts(wt_restored);
    printf("WORKTREE CHARGE : %s\n", wt_restored_str);

    /**
     Test de restoreWorkTree :  Vérifie la restauration complète d'un arbre
     */
    printf("\n[TEST] restoreWorkTree :\n");
    system("mkdir -p restored");
    restoreWorkTree(wt_restored, "./restored");
    printf("FICHIERS RESTAURES. Verifiez avec 'ls -l restored'.\n");

    // ==========================================
    // SECTION : Nettoyage
    // ==========================================

    // Libération mémoire Chapitre 1
    freeList(L);
    freeList(L2);

    // Libération mémoire Chapitre 2
    free(wf->name);
    free(wf->hash);
    free(wf);
    free(wf2->name);
    free(wf2->hash);
    free(wf2);
    free(wf_str);

    for (int i = 0; i < wt->n; i++) {
        free(wt->tab[i].name);
        if (wt->tab[i].hash) free(wt->tab[i].hash);
    }
    free(wt->tab);
    free(wt);

    for (int i = 0; i < wt_restored->n; i++) {
        free(wt_restored->tab[i].name);
        if (wt_restored->tab[i].hash) free(wt_restored->tab[i].hash);
    }
    free(wt_restored->tab);
    free(wt_restored);

    free(wt_str);
    free(wt_restored_str);
    free(wt_hash);

    return 0;
}

