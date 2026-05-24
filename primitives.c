#include <stdio.h>
#include <string.h>
#include <time.h>
#include "sgf.h"

extern Disque disque;
extern int inode_courant;
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

    if (ajouter_entree(inode_courant, nom, i) == -1) {
        disque.inodes[i].type = TYPE_LIBRE;
        disque.sb.bitmap_inodes[i] = 0;
        disque.sb.nb_inodes_libres++;
        return -1;
    }

    printf("Fichier '%s' cree (inode %d).\n", nom, i);
    return i;
}

int mywrite(int inode_num, char *buffer, int taille) {
    Inode *ino = &disque.inodes[inode_num];

    if (ino->type != TYPE_FICHIER) {
        printf("Erreur : l'inode %d n'est pas un fichier.\n", inode_num);
        return -1;
    }

    int octets_ecrits = 0;
    int reste = taille;
    int indice_bloc = 0;

    while (reste > 0 && indice_bloc < MAX_BLOCS_FIC) {
        if (ino->blocs[indice_bloc] == -1) {
            int b = trouver_bloc_libre();
            if (b == -1) {
                printf("Erreur : disque plein.\n");
                break;
            }
            ino->blocs[indice_bloc] = b;
            disque.sb.bitmap_blocs[b] = 1;
            disque.sb.nb_blocs_libres--;
        }

        int a_ecrire = reste;
        if (a_ecrire > TAILLE_BLOC) {
            a_ecrire = TAILLE_BLOC;
        }

        int b = ino->blocs[indice_bloc];
        memcpy(disque.blocs[b].donnees, buffer + octets_ecrits, a_ecrire);

        octets_ecrits += a_ecrire;
        reste         -= a_ecrire;
        indice_bloc++;
    }

    ino->taille     = octets_ecrits;
    ino->date_modif = (int) time(NULL);

    return octets_ecrits;
}

int myread(int inode_num, char *buffer, int taille_max) {
    Inode *ino = &disque.inodes[inode_num];

    if (ino->type != TYPE_FICHIER) {
        printf("Erreur : l'inode %d n'est pas un fichier.\n", inode_num);
        return -1;
    }

    int octets_lus = 0;
    int reste = ino->taille;
    if (reste > taille_max) {
        reste = taille_max;
    }

    int indice_bloc = 0;
    while (reste > 0 && indice_bloc < MAX_BLOCS_FIC) {
        if (ino->blocs[indice_bloc] == -1) {
            break;
            }
        int a_lire = reste;
        if (a_lire > TAILLE_BLOC) {
            a_lire = TAILLE_BLOC;
        }

        int b = ino->blocs[indice_bloc];
        memcpy(buffer + octets_lus, disque.blocs[b].donnees, a_lire);

        octets_lus += a_lire;
        reste      -= a_lire;
        indice_bloc++;
    }

    return octets_lus;
}

int chercher_entree(char *nom) {
    int inode_rep = inode_courant;
    Inode *rep = &disque.inodes[inode_rep];

    if (rep->blocs[0] == -1) {
        return -1;
    }

    int b = rep->blocs[0];
    EntreeRep *entrees = (EntreeRep *) disque.blocs[b].donnees;

    for (int i = 0; i < (int) MAX_ENTREES; i++) {
        if (entrees[i].inode != -1) {               
            if (strcmp(entrees[i].nom, nom) == 0) {
                return entrees[i].inode;  
            }
        }
    }

    return -1; 
}


int myopen(char *nom, int droits) {
    int ino = chercher_entree(nom);

    if (ino != -1) {
        return ino;
    }

    return mycreat(nom, droits);
}


int myclose(int inode_num) {
    if (inode_num < 0 || inode_num >= NB_INODES) {
        printf("Erreur : inode %d invalide.\n", inode_num);
        return -1;
    }
    if (disque.inodes[inode_num].type == TYPE_LIBRE) {
        printf("Erreur : l'inode %d n'est pas utilise.\n", inode_num);
        return -1;
    }
    return 0;  
}

int mymkdir(char *nom) {
    if (strlen(nom) >= MAX_NOM) {
        printf("Erreur : nom trop long.\n");
        return -1;
    }

    if (chercher_entree(nom) != -1) {
        printf("Erreur : '%s' existe deja.\n", nom);
        return -1;
    }

    int i = trouver_inode_libre();
    if (i == -1) {
        printf("Erreur : plus d'inodes disponibles.\n");
        return -1;
    }

    disque.inodes[i].type       = TYPE_REPERTOIRE;
    disque.inodes[i].taille     = 0;
    disque.inodes[i].droits     = 755;
    disque.inodes[i].date_modif = (int) time(NULL);
    disque.inodes[i].nb_liens   = 1;
    for (int j = 0; j < MAX_BLOCS_FIC; j++) {
        disque.inodes[i].blocs[j] = -1;
    }

    disque.sb.bitmap_inodes[i] = 1;
    disque.sb.nb_inodes_libres--;

    if (ajouter_entree(inode_courant, nom, i) == -1) {
        disque.inodes[i].type = TYPE_LIBRE;
        disque.sb.bitmap_inodes[i] = 0;
        disque.sb.nb_inodes_libres++;
        return -1;
    }

    ajouter_entree(i, ".", i);
    ajouter_entree(i, "..", inode_courant);

    printf("Repertoire '%s' cree (inode %d).\n", nom, i);
    return i;
}

int supprimer_entree(int inode_rep, char *nom) {
    Inode *rep = &disque.inodes[inode_rep];
    if (rep->blocs[0] == -1) {
        return -1;
    }

    int b = rep->blocs[0];
    EntreeRep *entrees = (EntreeRep *) disque.blocs[b].donnees;

    for (int i = 0; i < (int) MAX_ENTREES; i++) {
        if (entrees[i].inode != -1 && strcmp(entrees[i].nom, nom) == 0) {
            int inode_trouve = entrees[i].inode;
            entrees[i].inode = -1;
            entrees[i].nom[0] = '\0';
            return inode_trouve;
        }
    }
    return -1;
}

void liberer_inode(int inode_num) {
    Inode *ino = &disque.inodes[inode_num];

    for (int j = 0; j < MAX_BLOCS_FIC; j++) {
        if (ino->blocs[j] != -1) {
            int b = ino->blocs[j];
            disque.sb.bitmap_blocs[b] = 0;
            disque.sb.nb_blocs_libres++;
            ino->blocs[j] = -1;
        }
    }

    ino->type = TYPE_LIBRE;
    disque.sb.bitmap_inodes[inode_num] = 0;
    disque.sb.nb_inodes_libres++;
}

int myunlink(char *nom) {
    int ino = supprimer_entree(inode_courant, nom);
    if (ino == -1) {
        printf("Erreur : '%s' introuvable.\n", nom);
        return -1;
    }

    disque.inodes[ino].nb_liens--;

    if (disque.inodes[ino].nb_liens <= 0) {
        liberer_inode(ino);
        printf("Fichier '%s' supprime (inode %d libere).\n", nom, ino);
    } else {
        printf("Lien '%s' supprime (il reste %d lien(s)).\n",
               nom, disque.inodes[ino].nb_liens);
    }

    return 0;
}