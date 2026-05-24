#include <stdio.h>
#include <string.h>
#include "sgf.h"

int charger_disque(void);
int sauvegarder_disque(void);
int mycreat(char *nom, int droits);
int mymkdir(char *nom);
int chercher_entree(char *nom);

extern Disque disque;
extern int inode_courant;

int main(void) {
    printf("=== Test du SGF ===\n");
    charger_disque();

    int dossier = mymkdir("photos");

    mycreat("racine.txt", 644);

    printf("\n--- On entre dans photos ---\n");
    inode_courant = dossier;

    mycreat("vacances.jpg", 644);

    printf("Recherche 'vacances.jpg' dans photos : inode %d\n",
           chercher_entree("vacances.jpg"));

    printf("Recherche 'racine.txt' dans photos  : %d (doit etre -1)\n",
           chercher_entree("racine.txt"));

    printf("\n--- On remonte au parent (cd ..) ---\n");
    inode_courant = chercher_entree("..");
    printf("Inode courant apres remontee : %d (doit etre 0, la racine)\n",
           inode_courant);

    printf("Recherche 'racine.txt' dans racine  : inode %d\n",
           chercher_entree("racine.txt"));

    sauvegarder_disque();
    printf("=== Fin du test ===\n");
    return 0;
}