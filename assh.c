#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "util.h"
#include "parser.h"

#define TAILLE_REP 200


/*
void gest_ps_tache_fond ()
{
    pid_t pid_fils_zombie = wait (NULL) ;
    printf ("Fin du processus %d\n", pid_fils_zombie) ;
}
*/


int main (void)
{

    // Déclarations et initialisations préliminaires
    char line[lineSize] ;       // ligne de commande saisie
    pid_t res_f, res_w, res ;   // pid des processus
    WordList * wl = WL_new() ;  // pointeur sur une structure liste de mots pour l'analyse de la ligne de commandes

/*
    // Pour le traitement du signal SIGCHLD
    struct sigaction nouv_sa ;
    sigemptyset (&nouv_sa.sa_mask) ;
    nouv_sa.sa_handler = gest_ps_tache_fond ;
    nouv_sa.sa_flags = 0 ;
    sigaction (SIGCHLD, &nouv_sa, 0) ;
*/


    //------------------------------------------------------------------------//
    //---------------------------- Coeur du shell ----------------------------//
    // Boucle avec affichage de prompt, saisie d'une commande, analyse de la  //
    // ligne saisie et lancement de la commande si elle est correcte ---------//
    //------------------------------------------------------------------------//
    int boucle = 1 ;
    while (boucle)
    {
        //----------------------- Saisie d'une commande -----------------------

        printf ("assh$ ") ;              // affichage du prompt
        fgets (line, lineSize, stdin) ;  // lecture d'une ligne


        //------------------- Analyse de la commande saisie -------------------

        suppr_ret_char (line) ;  // suppression du retour chariot
        parseCmd (line, wl) ;    // analyse de la commande


        //------------------ Lancement de la commande saisie ------------------

        //---- Ligne vide ----
        if (wl->nbWord == 0) // aucun mot dans la ligne saisie -> on ne fait rien, on passe au tour de boucle suivant
        {}

        //---- Commandes internes ----
        else if (!strcmp(wl->word[0], "exit")) // commande 'exit' : on sort de la boucle, on quitte le shell
        {
            boucle = 0 ;
        }

        else if (!strcmp(wl->word[0], "cd"))  // Commande 'cd' :
        {
            if (wl->nbWord >= 3)  // nombre d'arguments de 'cd' trop grand (>= 2)
                fprintf (stderr, "assh : cd : trop d'arguments\n") ;
            else  // nombre d'arguments de 'cd' correct (= 0 ou 1)
            {
                // Récupération du répertoire
                char repertoire [TAILLE_REP] = "" ;
                if (wl->nbWord == 1 || (wl->nbWord == 2 && !strcmp(wl->word[1],"~")))  // aucun argument (commande réduite à 'cd') ou argument égal à '~'
                    strcat (repertoire, getenv("HOME")) ;  // le répertoire de destination est le répertoire personnel
                else // un argument fourni non égal à '~'
                    strcat (repertoire, wl->word[1]) ;  // le répertoire de destination est fixé à l'argument fourni

                // Changement de répertoire et gestion des erreurs
                if (chdir(repertoire) == -1)  // on tente de changer de répertoire, il y a une erreur
                {
                    switch (errno)  // On cherche dans errno les causes de l'échec du changement de répertoire et on affiche un message correspondant
                    {
                        case EACCES :
                            fprintf (stderr, "assh : cd : accès refusé\n") ;
                            break ;
                        case ENOENT :
                            fprintf (stderr, "assh : cd : répertoire inexistant\n") ;
                            break ;
                        case ENOTDIR :
                            fprintf (stderr, "assh : cd : le nom spécifié n'est pas un répertoire\n") ;
                            break ;
                        default :
                            fprintf (stderr, "assh : cd : erreur inconnue\n") ;
                    }
                }
                else  // le changement de répertoire a réussi
                {
                    // Récupération du chemin complet du répertoire courant ('repertoire' peut être un chemin relatif ou un raccourci (".", ".."))
                    char repertoire_complet [TAILLE_REP] ;
                    getcwd (repertoire_complet, TAILLE_REP) ;

                    // Mise à jour de la variable d'environnement PWD
                    setenv ("PWD", repertoire_complet, 1) ;

                    // Affichage du répertoire courant
                    printf ("%s\n", repertoire_complet) ;
                }
            }
        }

        //---- Lancement de processus ----
        else
        {
            // Duplication du processus
            res_f = fork () ;
            if (res_f == -1)  // La duplication a échoué
            {
                fprintf (stderr, "assh : %s : lancement de la commande impossible\n", wl->word[0]) ;
            }
            else if (res_f == 0) // processus fils
            {
                res = execvp (wl->word[0], wl->word) ;  // on tente d'exécuter une nouvelle commande dans ce processus
                if (res == -1)
                {
                    fprintf (stderr, "assh : %s : commande introuvable\n", wl->word[0]) ;
                    exit (EXIT_FAILURE) ;  // la commande n'a pu être exécutée, on termine le processus fils qui n'a plus de raison d'être
                }
            }
            else  // processus père
            {
                /*
                if (dernier_carac(line) == '&')  // tentative de gestion des processus en arrière plan
                {
                }
                else
                {*/
                    res_w = waitpid (res_f, NULL, 0) ;
                    if (res_w == -1) { perror("Echec wait "); exit(errno); }
                //}
            }
        }
    }

    printf ("Vous quittez assh... à bientôt !\n") ;

    return EXIT_SUCCESS ;

}

