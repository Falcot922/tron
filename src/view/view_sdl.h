#ifndef VIEW_SDL_H
#define VIEW_SDL_H

#include <SDL2/SDL.h>
#include "model/model.h"

// Interface graphique : ne fait que dessiner. Les événements sont gérés par le contrôleur.

int sdl_ouvrir(int largeur, int hauteur); // 0 si la fenêtre et les images sont prêtes
void sdl_fermer(void);

void sdl_afficher_menu(void);
// Plateau en cours ; si la partie est finie, ajoute le bouton restart et annonce le résultat
void sdl_afficher_jeu(const Model *model, int **plateau, Gagnant resultat);

// Emplacement des boutons, pour que le contrôleur sache où l'on clique
SDL_Rect sdl_bouton_start(void);
SDL_Rect sdl_bouton_restart(void);

#endif
