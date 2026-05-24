#include <stdio.h>
#include "sgf.h"

int charger_disque(void);
int sauvegarder_disque(void);

extern Disque disque;

int main(void) {
    printf("=== Test du SGF ===\n");

    charger_disque();

    printf("Inodes libres : %d\n", disque.sb.nb_inodes_libres);
    printf("Blocs libres  : %d\n", disque.sb.nb_blocs_libres);
    printf("Inode racine  : %d\n", disque.sb.inode_racine);

    sauvegarder_disque();

    printf("=== Fin du test ===\n");
    return 0;
}