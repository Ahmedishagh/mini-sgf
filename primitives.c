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

int mylink(char *nom1, char *nom2) {
    int ino = chercher_entree(nom1);
    if (ino == -1) {
        printf("Erreur : '%s' introuvable.\n", nom1);
        return -1;
    }

    if (chercher_entree(nom2) != -1) {
        printf("Erreur : '%s' existe deja.\n", nom2);
        return -1;
    }

    if (ajouter_entree(inode_courant, nom2, ino) == -1) {
        return -1;
    }

    disque.inodes[ino].nb_liens++;

    printf("Lien '%s' cree vers '%s' (inode %d, %d liens).\n",
           nom2, nom1, ino, disque.inodes[ino].nb_liens);
    return 0;
}

/* ========================================================
   compter_entrees : compte le nombre d'entrées réelles
   d'un répertoire (y compris . et ..).
   ======================================================== */
int compter_entrees(int inode_rep) {
    Inode *rep = &disque.inodes[inode_rep];
    if (rep->blocs[0] == -1) {
        return 0;
    }

    int b = rep->blocs[0];
    EntreeRep *entrees = (EntreeRep *) disque.blocs[b].donnees;

    int compte = 0;
    for (int i = 0; i < (int) MAX_ENTREES; i++) {
        if (entrees[i].inode != -1) {
            compte++;
        }
    }
    return compte;
}

int myrmdir(char *nom) {
    int ino = chercher_entree(nom);
    if (ino == -1) {
        printf("Erreur : '%s' introuvable.\n", nom);
        return -1;
    }

    if (disque.inodes[ino].type != TYPE_REPERTOIRE) {
        printf("Erreur : '%s' n'est pas un repertoire.\n", nom);
        return -1;
    }

    if (compter_entrees(ino) > 2) {
        printf("Erreur : le repertoire '%s' n'est pas vide.\n", nom);
        return -1;
    }

    supprimer_entree(inode_courant, nom);

    liberer_inode(ino);

    printf("Repertoire '%s' supprime (inode %d libere).\n", nom, ino);
    return 0;
}

void cmd_ls(void) {
    Inode *rep = &disque.inodes[inode_courant];
    if (rep->blocs[0] == -1) {
        return;
    }

    int b = rep->blocs[0];
    EntreeRep *entrees = (EntreeRep *) disque.blocs[b].donnees;

    for (int i = 0; i < (int) MAX_ENTREES; i++) {
        if (entrees[i].inode != -1) {
            int type = disque.inodes[entrees[i].inode].type;
            if (type == TYPE_REPERTOIRE) {
                printf("%s/  ", entrees[i].nom);
            } else {
                printf("%s  ", entrees[i].nom);
            }
        }
    }
    printf("\n");
}


void cmd_df(void) {
    int blocs_libres  = disque.sb.nb_blocs_libres;
    int inodes_libres = disque.sb.nb_inodes_libres;

    printf("--- Informations du disque ---\n");
    printf("Inodes : %d libres / %d total\n", inodes_libres, NB_INODES);
    printf("Blocs  : %d libres / %d total\n", blocs_libres, NB_BLOCS);
    printf("Espace libre : %d octets\n", blocs_libres * TAILLE_BLOC);
}

void cmd_cd(char *nom) {
    if (nom == NULL) {
        inode_courant = disque.sb.inode_racine;
        return;
    }

    int ino = chercher_entree(nom);
    if (ino == -1) {
        printf("cd : '%s' introuvable.\n", nom);
        return;
    }

    if (disque.inodes[ino].type != TYPE_REPERTOIRE) {
        printf("cd : '%s' n'est pas un repertoire.\n", nom);
        return;
    }

    inode_courant = ino;
}

void cmd_cat(char *nom) {
    if (nom == NULL) {
        printf("cat : nom de fichier manquant.\n");
        return;
    }

    int ino = chercher_entree(nom);
    if (ino == -1) {
        printf("cat : '%s' introuvable.\n", nom);
        return;
    }

    if (disque.inodes[ino].type != TYPE_FICHIER) {
        printf("cat : '%s' n'est pas un fichier.\n", nom);
        return;
    }

    char buffer[TAILLE_BLOC * MAX_BLOCS_FIC];
    memset(buffer, 0, sizeof(buffer));
    int n = myread(ino, buffer, sizeof(buffer) - 1);
    if (n > 0) {
        printf("%s\n", buffer);
    }
}

void cmd_echo(char *texte, char *redirection, char *fichier) {
    if (texte == NULL || redirection == NULL || fichier == NULL) {
        printf("echo : usage : echo texte > fichier\n");
        return;
    }
    if (strcmp(redirection, ">") != 0) {
        printf("echo : symbole de redirection '>' attendu.\n");
        return;
    }

    int ino = myopen(fichier, 644);
    if (ino == -1) {
        printf("echo : impossible d'ouvrir '%s'.\n", fichier);
        return;
    }

    mywrite(ino, texte, strlen(texte));
    myclose(ino);
}

void cmd_cp(char *src, char *dst) {
    if (src == NULL || dst == NULL) {
        printf("cp : usage : cp source destination\n");
        return;
    }

    int ino_src = chercher_entree(src);
    if (ino_src == -1) {
        printf("cp : '%s' introuvable.\n", src);
        return;
    }
    if (disque.inodes[ino_src].type != TYPE_FICHIER) {
        printf("cp : '%s' n'est pas un fichier.\n", src);
        return;
    }

    char buffer[TAILLE_BLOC * MAX_BLOCS_FIC];
    memset(buffer, 0, sizeof(buffer));
    int n = myread(ino_src, buffer, sizeof(buffer));

    int ino_dst = mycreat(dst, disque.inodes[ino_src].droits);
    if (ino_dst == -1) {
        printf("cp : impossible de creer '%s'.\n", dst);
        return;
    }
    mywrite(ino_dst, buffer, n);

    printf("'%s' copie vers '%s'.\n", src, dst);
}

void cmd_mv(char *src, char *dst) {
    if (src == NULL || dst == NULL) {
        printf("mv : usage : mv source destination\n");
        return;
    }

    int ino_src = chercher_entree(src);
    if (ino_src == -1) {
        printf("mv : '%s' introuvable.\n", src);
        return;
    }

    cmd_cp(src, dst);
    myunlink(src);

    printf("'%s' renomme en '%s'.\n", src, dst);
}

void cmd_ls_long(void) {
    Inode *rep = &disque.inodes[inode_courant];
    if (rep->blocs[0] == -1) {
        return;
    }

    int b = rep->blocs[0];
    EntreeRep *entrees = (EntreeRep *) disque.blocs[b].donnees;

    for (int i = 0; i < (int) MAX_ENTREES; i++) {
        if (entrees[i].inode != -1) {
            Inode *ino = &disque.inodes[entrees[i].inode];

            char type_car;
            if (ino->type == TYPE_REPERTOIRE) {
                type_car = 'd';
            } else {
                type_car = '-';
            }

            time_t date = (time_t) ino->date_modif;
            struct tm *tinfo = localtime(&date);
            char date_txt[32];
            strftime(date_txt, sizeof(date_txt), "%d/%m/%Y %H:%M", tinfo);

            printf("%c %d %6d octets  %s  %s\n",
                   type_car,
                   ino->droits,
                   ino->taille,
                   date_txt,
                   entrees[i].nom);
        }
    }
}