#include <stdio.h>
#include <SDL2/SDL.h>
#include <ncurses.h>
#include "control.h"
#include "model/model.h"
#include "view/view_sdl.h"
#include "view/view_ncurses.h"

// ─── SDL ───

#define SDL_LIGNES 100
#define SDL_COLONNES 150
#define SDL_PAS_MS 40

// Joueur 1 : ZQSD, joueur 2 : flèches
static void clavier_sdl(SDL_Keycode key, Moto *moto1, Moto *moto2) {
    switch (key) {
        case SDLK_z: tourner(moto1, HAUT); break;
        case SDLK_q: tourner(moto1, GAUCHE); break;
        case SDLK_s: tourner(moto1, BAS); break;
        case SDLK_d: tourner(moto1, DROITE); break;
        case SDLK_UP: tourner(moto2, HAUT); break;
        case SDLK_LEFT: tourner(moto2, GAUCHE); break;
        case SDLK_DOWN: tourner(moto2, BAS); break;
        case SDLK_RIGHT: tourner(moto2, DROITE); break;
        default: break;
    }
}

// Menu, puis parties à la suite : un clic sur start lance la première, un clic sur restart les suivantes
void jouer_sdl(int largeur, int hauteur) {
    if (sdl_ouvrir(largeur, hauteur) != 0) {
        return;
    }

    Model model = { SDL_LIGNES, SDL_COLONNES };
    Moto moto1, moto2;
    int **plateau = creationDuJeu(&moto1, &moto2, &model);

    enum { MENU, EN_JEU, FIN } etat = MENU;
    sdl_afficher_menu();

    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN && etat == EN_JEU) {
                clavier_sdl(event.key.keysym.sym, &moto1, &moto2);
            } else if (event.type == SDL_MOUSEBUTTONDOWN && etat != EN_JEU) {
                SDL_Point clic = { event.button.x, event.button.y };
                SDL_Rect bouton = etat == MENU ? sdl_bouton_start() : sdl_bouton_restart();
                if (SDL_PointInRect(&clic, &bouton)) {
                    reinitialiser(plateau, &moto1, &moto2, &model);
                    etat = EN_JEU;
                }
            }
        }

        if (etat == EN_JEU) {
            Gagnant resultat = avancer(plateau, &moto1, &moto2, &model);
            sdl_afficher_jeu(&model, plateau, resultat);
            if (resultat != CONTINUER) {
                printf("%s\n", gagnant_message(resultat));
                etat = FIN;
            }
        }
        SDL_Delay(SDL_PAS_MS);
    }

    free_jeu(plateau, model);
    sdl_fermer();
}

// ─── ncurses ───

#define NCURSES_COTE 25 // plateau de 25 x 25 au plus, réduit si le terminal est plus petit
#define NCURSES_PAS_MS 200

static void clavier_ncurses(int touche, Moto *moto1, Moto *moto2) {
    switch (touche) {
        case 'z': tourner(moto1, HAUT); break;
        case 'q': tourner(moto1, GAUCHE); break;
        case 's': tourner(moto1, BAS); break;
        case 'd': tourner(moto1, DROITE); break;
        case KEY_UP: tourner(moto2, HAUT); break;
        case KEY_LEFT: tourner(moto2, GAUCHE); break;
        case KEY_DOWN: tourner(moto2, BAS); break;
        case KEY_RIGHT: tourner(moto2, DROITE); break;
        default: break;
    }
}

// Retourne l'option validée avec Entrée
static int menu_ncurses(void) {
    int choix = MENU_LANCER;
    for (;;) {
        ncurses_afficher_menu(choix);
        switch (getch()) {
            case KEY_UP: choix = (choix + MENU_NB_CHOIX - 1) % MENU_NB_CHOIX; break;
            case KEY_DOWN: choix = (choix + 1) % MENU_NB_CHOIX; break;
            case '\n':
            case KEY_ENTER: return choix;
            default: break;
        }
    }
}

// Une partie : retourne son issue
static Gagnant partie_ncurses(int **plateau, Moto *moto1, Moto *moto2, const Model *model) {
    Gagnant resultat = CONTINUER;
    erase();
    nodelay(stdscr, TRUE); // getch ne bloque plus : le jeu avance même sans touche
    while (resultat == CONTINUER) {
        ncurses_afficher_plateau(plateau, model);
        napms(NCURSES_PAS_MS);
        int touche;
        while ((touche = getch()) != ERR) {
            clavier_ncurses(touche, moto1, moto2);
        }
        resultat = avancer(plateau, moto1, moto2, model);
    }
    nodelay(stdscr, FALSE);
    return resultat;
}

void jouer_ncurses(void) {
    ncurses_ouvrir();

    Model model = { LINES < NCURSES_COTE ? LINES : NCURSES_COTE, COLS / 2 < NCURSES_COTE ? COLS / 2 : NCURSES_COTE };
    if (model.lignes < 5 || model.colonnes < 5) {
        ncurses_fermer();
        fprintf(stderr, "Terminal trop petit pour jouer.\n");
        return;
    }

    if (menu_ncurses() == MENU_LANCER) {
        Moto moto1, moto2;
        int **plateau = creationDuJeu(&moto1, &moto2, &model);

        int touche;
        do {
            reinitialiser(plateau, &moto1, &moto2, &model);
            ncurses_afficher_fin(partie_ncurses(plateau, &moto1, &moto2, &model));
            do {
                touche = getch();
            } while (touche != 'o' && touche != 'n');
        } while (touche == 'o');

        free_jeu(plateau, model);
    }

    ncurses_afficher_au_revoir();
    ncurses_fermer();
}
