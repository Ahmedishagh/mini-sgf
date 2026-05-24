#include <stdio.h>
#include <string.h>
#include "sgf.h"

int charger_disque(void);
int sauvegarder_disque(void);
int mycreat(char *nom, int droits);
int mymkdir(char *nom);
int myrmdir(char *nom);
int chercher_entree(char *nom);

extern Disque disque;
extern int inode_courant;

int main(void) {
    printf("=== Test rmdir ===\n");
    charger_disque();

    mycreat("ancre.txt", 644);

    int inodes_ref = disque.sb.nb_inodes_libres;
    int blocs_ref  = disque.sb.nb_blocs_libres;
    printf("Reference : %d inodes, %d blocs\n", inodes_ref, blocs_ref);

    printf("\n--- Cas 1 : dossier non vide ---\n");
    int dossier = mymkdir("travail");
    inode_courant = dossier;      
    mycreat("fichier_dedans.txt", 644); 
    inode_courant = disque.sb.inode_racine; 
    myrmdir("travail");               

    printf("\n--- Cas 2 : dossier vide ---\n");
    mymkdir("vide");                 
    int inodes_avant = disque.sb.nb_inodes_libres;
    int blocs_avant  = disque.sb.nb_blocs_libres;
    myrmdir("vide");                
    int inodes_apres = disque.sb.nb_inodes_libres;
    int blocs_apres  = disque.sb.nb_blocs_libres;

    if (inodes_apres > inodes_avant && blocs_apres > blocs_avant) {
        printf(">>> SUCCES : le dossier vide a ete supprime et la place rendue.\n");
    } else {
        printf(">>> Probleme : la place n'a pas ete totalement rendue.\n");
    }

    if (chercher_entree("travail") != -1) {
        printf(">>> SUCCES : le dossier non vide 'travail' a bien ete protege.\n");
    } else {
        printf(">>> Probleme : 'travail' a ete supprime alors qu'il ne devait pas.\n");
    }

    sauvegarder_disque();
    printf("=== Fin du test ===\n");
    return 0;
}