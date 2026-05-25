#include <stdio.h>
#include <string.h>
#include "sgf.h"

int charger_disque(void);
int sauvegarder_disque(void);

extern Disque disque;
extern int inode_courant;
void cmd_ls(void);
void cmd_df(void);
void cmd_cd(char *nom);
void cmd_cat(char *nom);
int mymkdir(char *nom);
int myrmdir(char *nom);
int myunlink(char *nom);
int mylink(char *nom1, char *nom2);
void cmd_echo(char *texte, char *redirection, char *fichier);
void cmd_cp(char *src, char *dst);
void cmd_mv(char *src, char *dst);
void cmd_ls_long(void);

#define TAILLE_LIGNE 256

int main(void) {
    char ligne[TAILLE_LIGNE];

    printf("=== Mini-Shell SGF (tapez 'exit' pour quitter) ===\n");

    charger_disque();

    while (1) {
        printf("[mysh] $ ");
        fflush(stdout);            

        if (fgets(ligne, TAILLE_LIGNE, stdin) == NULL) {
            printf("\n");
            break;
        }

        ligne[strcspn(ligne, "\n")] = '\0';

        if (strlen(ligne) == 0) {
            continue;
        }

        if (strcmp(ligne, "exit") == 0) {
            sauvegarder_disque();
            printf("Au revoir !\n");
            break;
        }

        char *args[20];
        int nb_args = 0;

        char *mot = strtok(ligne, " ");
        while (mot != NULL && nb_args < 20) {
            args[nb_args] = mot;
            nb_args++;
            mot = strtok(NULL, " ");
        }

        for (int i = nb_args; i < 20; i++) {
            args[i] = NULL;
        }

        if (strcmp(args[0], "ls") == 0) {
            if (args[1] != NULL && strcmp(args[1], "-l") == 0) {
                cmd_ls_long();
            } else {
                cmd_ls();
            }
        }
        else if (strcmp(args[0], "df") == 0) {
            cmd_df();
        }
        else if (strcmp(args[0], "cd") == 0) {
            cmd_cd(args[1]);
        }
        else if (strcmp(args[0], "cat") == 0) {
            cmd_cat(args[1]);
        }
        else if (strcmp(args[0], "mkdir") == 0) {
            mymkdir(args[1]);
        }
        else if (strcmp(args[0], "rmdir") == 0) {
            myrmdir(args[1]);
        }
        else if (strcmp(args[0], "rm") == 0) {
            myunlink(args[1]);
        }
        else if (strcmp(args[0], "ln") == 0) {
            mylink(args[1], args[2]);
        }
        else if (strcmp(args[0], "echo") == 0) {
            cmd_echo(args[1], args[2], args[3]);
        }
        else if (strcmp(args[0], "cp") == 0) {
            cmd_cp(args[1], args[2]);
        }
        else if (strcmp(args[0], "mv") == 0) {
            cmd_mv(args[1], args[2]);
        }
        else {
            printf("Commande inconnue : '%s'\n", args[0]);
        }
    
    }

    return 0;
}