#include <locale.h>
#include <string.h>
#include <ncurses.h>
#include "view_ncurses.h"

enum { PAIRE_SELECTION = 1, PAIRE_JOUEUR1, PAIRE_JOUEUR2 };

void ncurses_ouvrir(void) {
    setlocale(LC_ALL, ""); // pour les accents
    initscr();
    start_color();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    init_pair(PAIRE_SELECTION, COLOR_WHITE, COLOR_BLUE);
    init_pair(PAIRE_JOUEUR1, COLOR_BLUE, COLOR_BLACK);
    init_pair(PAIRE_JOUEUR2, COLOR_RED, COLOR_BLACK);
}

void ncurses_fermer(void) {
    endwin();
}

// Écrit le texte centré sur la ligne y
static void centrer(int y, const char *texte) {
    mvprintw(y, (getmaxx(stdscr) - (int)strlen(texte)) / 2, "%s", texte);
}

// Logo, consignes et options ; l'option `choix` est surlignée
void ncurses_afficher_menu(int choix) {
    const char *options[MENU_NB_CHOIX] = {
        "1. Lancer le jeu",
        "2. Quitter"
    };
    const char *tron[] = {
        "_____ ____  ____  _   _",
        "  |  (  _ \\( __ )| \\ | |",
        "  |   )   /| __ ||  \\| |",
        "  |  (__\\_)(____)|__|\\_|"
    };
    int tron_height = sizeof(tron) / sizeof(tron[0]);

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int tron_x = (max_x - (int)strlen(tron[0])) / 2;
    int tron_y = (max_y - tron_height) / 6 + 1;

    erase();
    mvhline(tron_y - 1, 0, '_', max_x);
    for (int i = 0; i < tron_height; i++) {
        mvprintw(tron_y + i, tron_x, "%s", tron[i]);
    }
    mvhline(tron_y + tron_height, 0, '_', max_x);

    centrer(tron_y + tron_height + 2, "Bienvenue dans le jeu Tron !");
    centrer(tron_y + tron_height + 3, "Flèches pour naviguer, Entrée pour sélectionner.");
    centrer(tron_y + tron_height + 4, "En jeu : joueur 1 avec ZQSD, joueur 2 avec les flèches.");

    for (int i = 0; i < MENU_NB_CHOIX; i++) {
        if (i == choix) attron(COLOR_PAIR(PAIRE_SELECTION));
        centrer(max_y / 2 + i, options[i]);
        if (i == choix) attroff(COLOR_PAIR(PAIRE_SELECTION));
    }
    refresh();
}

// Une case du plateau occupe deux colonnes du terminal, pour rester à peu près carrée
void ncurses_afficher_plateau(int **plateau, const Model *model) {
    for (int i = 0; i < model->lignes; i++) {
        for (int j = 0; j < model->colonnes; j++) {
            if (plateau[i][j] == 1) {
                mvaddch(i, j * 2, '1' | COLOR_PAIR(PAIRE_JOUEUR1) | A_BOLD);
            } else if (plateau[i][j] == 2) {
                mvaddch(i, j * 2, '2' | COLOR_PAIR(PAIRE_JOUEUR2) | A_BOLD);
            } else {
                mvaddch(i, j * 2, '.');
            }
        }
    }
    refresh();
}

void ncurses_afficher_fin(Gagnant resultat) {
    int y = getmaxy(stdscr) / 4;
    erase();
    centrer(y + 1, gagnant_message(resultat));
    centrer(y + 2, "Voulez-vous rejouer ? (o/n)");
    refresh();
}

void ncurses_afficher_au_revoir(void) {
    erase();
    centrer(getmaxy(stdscr) / 4, "Merci d'avoir joué !");
    refresh();
    napms(2000);
}
