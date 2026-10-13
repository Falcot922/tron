#ifndef VIEW_NCURSES_H
#define VIEW_NCURSES_H

#include "model/model.h"

// Interface texte : ne fait que dessiner. Les touches sont lues par le contrôleur.

enum { MENU_LANCER, MENU_QUITTER, MENU_NB_CHOIX };

void ncurses_ouvrir(void);
void ncurses_fermer(void);

void ncurses_afficher_menu(int choix);
void ncurses_afficher_plateau(int **plateau, const Model *model);
void ncurses_afficher_fin(Gagnant resultat); // résultat, puis « Voulez-vous rejouer ? »
void ncurses_afficher_au_revoir(void);

#endif
