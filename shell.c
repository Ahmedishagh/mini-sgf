#include <stdio.h>
#include <string.h>
#include "sgf.h"

int charger_disque(void);
int sauvegarder_disque(void);
int mycreat(char *nom, int droits);
int mywrite(int inode_num, char *buffer, int taille);
int myread(int inode_num, char *buffer, int taille_max);
int myopen(char *nom, int droits);
int myclose(int inode_num);
int chercher_entree(char *nom);

extern Disque disque;

int main(void) {
    printf("=== Test du SGF ===\n");
    charger_disque();

    int ino = myopen("rapport.txt", 644);
    printf("Inode obtenu pour rapport.txt : %d\n", ino);

    char *texte = "Mini systeme de gestion de fichiers - IATIC3.";
    mywrite(ino, texte, strlen(texte));

    myclose(ino);
    printf("Fichier ferme.\n");

    int ino2 = myopen("rapport.txt", 644);
    printf("Inode obtenu en rouvrant       : %d  (doit etre identique)\n", ino2);

    char lecture[1024];
    memset(lecture, 0, sizeof(lecture));
    myread(ino2, lecture, 1024);
    printf("Contenu relu : %s\n", lecture);

    int absent = chercher_entree("inexistant.txt");
    printf("Recherche d'un fichier absent  : %d  (doit etre -1)\n", absent);

    sauvegarder_disque();
    printf("=== Fin du test ===\n");
    return 0;
}