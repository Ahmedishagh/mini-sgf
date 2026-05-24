#include <stdio.h>
#include <string.h>
#include <time.h>
#include "sgf.h"

Disque disque;
int inode_courant;

int trouver_inode_libre(void) {
    for (int i = 0; i < NB_INODES; i++) {
        if (disque.sb.bitmap_inodes[i] == 0) {
            return i;
        }
    }
    return -1;
}

int trouver_bloc_libre(void) {
    for (int b = 0; b < NB_BLOCS; b++) {
        if (disque.sb.bitmap_blocs[b] == 0) {
            return b;
        }
    }
    return -1;
}

void formater_disque(void) {
    memset(&disque, 0, sizeof(Disque));

    for (int i = 0; i < NB_INODES; i++) {
        disque.inodes[i].type = TYPE_LIBRE;
        for (int j = 0; j < MAX_BLOCS_FIC; j++) {
            disque.inodes[i].blocs[j] = -1;
        }
    }

    disque.sb.nb_inodes_libres = NB_INODES;
    disque.sb.nb_blocs_libres  = NB_BLOCS;

    disque.inodes[0].type       = TYPE_REPERTOIRE;
    disque.inodes[0].taille     = 0;
    disque.inodes[0].droits     = 755;
    disque.inodes[0].date_modif = (int) time(NULL);
    disque.inodes[0].nb_liens   = 1;

    disque.sb.bitmap_inodes[0] = 1;
    disque.sb.nb_inodes_libres = disque.sb.nb_inodes_libres - 1;

    disque.sb.inode_racine = 0;

    inode_courant = 0;

    printf("Disque formate : %d inodes et %d blocs disponibles.\n",
           disque.sb.nb_inodes_libres, disque.sb.nb_blocs_libres);
}