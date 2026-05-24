#include <stdio.h>
#include <string.h>
#include <time.h>
#include "sgf.h"

extern Disque disque;
int trouver_inode_libre(void);
int trouver_bloc_libre(void);


int ajouter_entree(int inode_rep, char *nom, int inode_cible) {
    Inode *rep = &disque.inodes[inode_rep];

    if (rep->blocs[0] == -1) {
        int b = trouver_bloc_libre();
        if (b == -1) {
            printf("Erreur : plus de blocs disponibles.\n");
            return -1;
        }
        rep->blocs[0] = b;
        disque.sb.bitmap_blocs[b] = 1;
        disque.sb.nb_blocs_libres--;

        EntreeRep *entrees = (EntreeRep *) disque.blocs[b].donnees;
        for (int i = 0; i < (int) MAX_ENTREES; i++) {
            entrees[i].inode = -1;
        }
    }

    int b = rep->blocs[0];
    EntreeRep *entrees = (EntreeRep *) disque.blocs[b].donnees;

    for (int i = 0; i < (int) MAX_ENTREES; i++) {
        if (entrees[i].inode == -1) {
            strncpy(entrees[i].nom, nom, MAX_NOM - 1);
            entrees[i].nom[MAX_NOM - 1] = '\0';
            entrees[i].inode = inode_cible;
            return 0;
        }
    }

    printf("Erreur : repertoire plein.\n");
    return -1;
}


int mycreat(char *nom, int droits) {
    if (strlen(nom) >= MAX_NOM) {
        printf("Erreur : nom trop long.\n");
        return -1;
    }

    int i = trouver_inode_libre();
    if (i == -1) {
        printf("Erreur : plus d'inodes disponibles.\n");
        return -1;
    }

    disque.inodes[i].type       = TYPE_FICHIER;
    disque.inodes[i].taille     = 0;
    disque.inodes[i].droits     = droits;
    disque.inodes[i].date_modif = (int) time(NULL);
    disque.inodes[i].nb_liens   = 1;
    for (int j = 0; j < MAX_BLOCS_FIC; j++) {
        disque.inodes[i].blocs[j] = -1;
    }

    disque.sb.bitmap_inodes[i] = 1;
    disque.sb.nb_inodes_libres--;

    if (ajouter_entree(disque.sb.inode_racine, nom, i) == -1) {
        disque.inodes[i].type = TYPE_LIBRE;
        disque.sb.bitmap_inodes[i] = 0;
        disque.sb.nb_inodes_libres++;
        return -1;
    }

    printf("Fichier '%s' cree (inode %d).\n", nom, i);
    return i;
}