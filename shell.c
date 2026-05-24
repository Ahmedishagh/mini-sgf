#include <stdio.h>
#include "sgf.h"

int charger_disque(void);
int sauvegarder_disque(void);
int mycreat(char *nom, int droits);

extern Disque disque;

int main(void) {
    printf("=== Test du SGF ===\n");

    charger_disque();

    mycreat("fichier1.txt", 644);
    mycreat("fichier2.txt", 644);
    mycreat("notes.txt", 644);

    printf("\nInodes libres : %d\n", disque.sb.nb_inodes_libres);
    printf("Blocs libres  : %d\n", disque.sb.nb_blocs_libres);

    sauvegarder_disque();

    printf("=== Fin du test ===\n");
    return 0;
}