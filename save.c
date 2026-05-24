#include <stdio.h>
#include "sgf.h"

extern Disque disque;

void formater_disque(void);

int sauvegarder_disque(void) {
    FILE *f = fopen(FICHIER_DISQUE, "wb");
    if (f == NULL) {
        printf("Erreur : impossible d'ouvrir %s en ecriture.\n", FICHIER_DISQUE);
        return -1;
    }

    fwrite(&disque, sizeof(Disque), 1, f);

    fclose(f);
    return 0;
}

int charger_disque(void) {
    FILE *f = fopen(FICHIER_DISQUE, "rb");
    if (f == NULL) {
        printf("Aucun disque trouve : creation d'un nouveau disque.\n");
        formater_disque();
        return 0;
    }

    fread(&disque, sizeof(Disque), 1, f);

    fclose(f);
    printf("Disque recharge depuis %s.\n", FICHIER_DISQUE);
    return 0;
}