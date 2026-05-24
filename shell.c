#include <stdio.h>
#include <string.h>
#include "sgf.h"

int charger_disque(void);
int sauvegarder_disque(void);
int mycreat(char *nom, int droits);
int mywrite(int inode_num, char *buffer, int taille);
int myread(int inode_num, char *buffer, int taille_max);
int mylink(char *nom1, char *nom2);
int myunlink(char *nom);
int chercher_entree(char *nom);

extern Disque disque;
extern int inode_courant;

int main(void) {
    printf("=== Test des liens ===\n");
    charger_disque();

    int ino = mycreat("original.txt", 644);
    char *texte = "Contenu partage par plusieurs noms.";
    mywrite(ino, texte, strlen(texte));

    mylink("original.txt", "copie.txt");

    printf("Inode de original.txt : %d\n", chercher_entree("original.txt"));
    printf("Inode de copie.txt    : %d (doit etre identique)\n",
           chercher_entree("copie.txt"));

    char lecture[1024];
    memset(lecture, 0, sizeof(lecture));
    myread(chercher_entree("copie.txt"), lecture, 1024);
    printf("Lecture via copie.txt : %s\n", lecture);

    printf("\n--- On supprime original.txt ---\n");
    myunlink("original.txt");

    memset(lecture, 0, sizeof(lecture));
    myread(chercher_entree("copie.txt"), lecture, 1024);
    printf("Lecture via copie.txt apres suppression : %s\n", lecture);

    sauvegarder_disque();
    printf("=== Fin du test ===\n");
    return 0;
}