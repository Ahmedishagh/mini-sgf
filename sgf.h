#ifndef SGF_H
#define SGF_H


#define NB_INODES        64
#define NB_BLOCS         256
#define TAILLE_BLOC      1024
#define MAX_BLOCS_FIC    30
#define MAX_NOM          32


#define TYPE_LIBRE       0
#define TYPE_FICHIER     1
#define TYPE_REPERTOIRE  2


#define FICHIER_DISQUE   "disk.img"


typedef struct {
    int nb_inodes_libres;
    int nb_blocs_libres;
    int inode_racine;
    int bitmap_inodes[NB_INODES];
    int bitmap_blocs[NB_BLOCS];
} Superbloc;


typedef struct {
    int type;
    int taille;
    int droits;
    int date_modif;
    int nb_liens;
    int blocs[MAX_BLOCS_FIC];
} Inode;


typedef struct {
    char donnees[TAILLE_BLOC];
} Bloc;


typedef struct {
    Superbloc sb;
    Inode inodes[NB_INODES];
    Bloc blocs[NB_BLOCS];
} Disque;

typedef struct {
    char nom[MAX_NOM];
    int inode;
} EntreeRep;

#define MAX_ENTREES (TAILLE_BLOC / sizeof(EntreeRep))

#endif