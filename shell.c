#include <stdio.h>
#include <string.h>
#include "sgf.h"

int charger_disque(void);
int sauvegarder_disque(void);
int mycreat(char *nom, int droits);
int mywrite(int inode_num, char *buffer, int taille);
int myread(int inode_num, char *buffer, int taille_max);

extern Disque disque;

int main(void) {
    printf("=== Test du SGF ===\n");
    charger_disque();

    /* 1. On crée un fichier */
    int ino = mycreat("hello.txt", 644);

    /* 2. On écrit un texte dedans */
    char *texte = "Bonjour ! Ceci est mon premier fichier dans le SGF.";
    int n = mywrite(ino, texte, strlen(texte));
    printf("Octets ecrits : %d\n", n);

    /* 3. On relit le fichier dans un buffer vide */
    char lecture[1024];
    memset(lecture, 0, sizeof(lecture));     // on vide le buffer d'abord
    int lus = myread(ino, lecture, 1024);
    printf("Octets lus    : %d\n", lus);
    printf("Contenu relu  : %s\n", lecture);

    sauvegarder_disque();
    printf("=== Fin du test ===\n");
    return 0;
}