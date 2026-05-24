#include <stdio.h>
#include <string.h>
#include "sgf.h"

int charger_disque(void);
int sauvegarder_disque(void);
int mycreat(char *nom, int droits);
int mywrite(int inode_num, char *buffer, int taille);
int myunlink(char *nom);

extern Disque disque;
extern int inode_courant;

int main(void) {
    printf("=== Test suppression (non-fuite) ===\n");
    charger_disque();

    mycreat("permanent.txt", 644);

    int inodes_avant = disque.sb.nb_inodes_libres;
    int blocs_avant  = disque.sb.nb_blocs_libres;
    printf("AVANT : %d inodes libres, %d blocs libres\n",
           inodes_avant, blocs_avant);

    int ino = mycreat("temp.txt", 644);
    char *texte = "Donnees temporaires a supprimer ensuite.";
    mywrite(ino, texte, strlen(texte));
    printf("PENDANT : %d inodes libres, %d blocs libres\n",
           disque.sb.nb_inodes_libres, disque.sb.nb_blocs_libres);

    myunlink("temp.txt");

    int inodes_apres = disque.sb.nb_inodes_libres;
    int blocs_apres  = disque.sb.nb_blocs_libres;
    printf("APRES : %d inodes libres, %d blocs libres\n",
           inodes_apres, blocs_apres);

    if (inodes_avant == inodes_apres && blocs_avant == blocs_apres) {
        printf(">>> SUCCES : aucune fuite, la place a ete rendue !\n");
    } else {
        printf(">>> ECHEC : il y a une fuite de ressources !\n");
    }

    sauvegarder_disque();
    printf("=== Fin du test ===\n");
    return 0;
}