#include <stdio.h>
#include <SDL2/SDL_image.h>
#include "view_sdl.h"

#define DOSSIER_IMAGES "src/utils/img/"
#define BOUTON_LARGEUR 200
#define BOUTON_HAUTEUR 100
#define BANDEAU_BAS 100 // place réservée sous le plateau pour le bouton restart

// Les variables globales évitent de passer la fenêtre et les textures à chaque fonction.
// Une structure serait plus propre, mais pour une seule fenêtre c'était overkill.
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *backgroundTexture;
static SDL_Texture *Logo;
static SDL_Texture *StartBouton;
static SDL_Texture *RestartBouton;

static SDL_Texture *charger(const char *fichier) {
    SDL_Texture *texture = IMG_LoadTexture(renderer, fichier);
    if (!texture) {
        fprintf(stderr, "Erreur IMG_LoadTexture (%s) : %s\n", fichier, IMG_GetError());
    }
    return texture;
}

// Libère tout ce qui a été créé : sert en fin de programme comme en cas d'échec à l'ouverture
void sdl_fermer(void) {
    if (RestartBouton) SDL_DestroyTexture(RestartBouton);
    if (StartBouton) SDL_DestroyTexture(StartBouton);
    if (Logo) SDL_DestroyTexture(Logo);
    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    RestartBouton = StartBouton = Logo = backgroundTexture = NULL;
    renderer = NULL;
    window = NULL;
    IMG_Quit();
    SDL_Quit();
}

int sdl_ouvrir(int largeur, int hauteur) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur SDL_Init : %s\n", SDL_GetError());
        return -1;
    }
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        fprintf(stderr, "Erreur IMG_Init : %s\n", IMG_GetError());
        SDL_Quit();
        return -1;
    }

    window = SDL_CreateWindow("Tron", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, largeur, hauteur, SDL_WINDOW_SHOWN);
    if (window) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }
    if (!window || !renderer) {
        fprintf(stderr, "Erreur à la création de la fenêtre : %s\n", SDL_GetError());
        sdl_fermer();
        return -1;
    }

    backgroundTexture = charger(DOSSIER_IMAGES "background.png");
    Logo = charger(DOSSIER_IMAGES "logo.png");
    StartBouton = charger(DOSSIER_IMAGES "start_bouton.png");
    RestartBouton = charger(DOSSIER_IMAGES "restart_bouton.png");
    if (!backgroundTexture || !Logo || !StartBouton || !RestartBouton) {
        fprintf(stderr, "Les images sont cherchées dans " DOSSIER_IMAGES " : lancez le jeu depuis la racine du projet.\n");
        sdl_fermer();
        return -1;
    }
    return 0;
}

SDL_Rect sdl_bouton_start(void) {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    SDL_Rect bouton = { (width - BOUTON_LARGEUR) / 2, (height - BOUTON_HAUTEUR) / 2, BOUTON_LARGEUR, BOUTON_HAUTEUR };
    return bouton;
}

SDL_Rect sdl_bouton_restart(void) {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    SDL_Rect bouton = { (width - BOUTON_LARGEUR) / 2, height - BOUTON_HAUTEUR - 25, BOUTON_LARGEUR, BOUTON_HAUTEUR };
    return bouton;
}

// Fond, logo (à moitié de sa taille) et bouton start
void sdl_afficher_menu(void) {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    int logoWidth, logoHeight;
    SDL_QueryTexture(Logo, NULL, NULL, &logoWidth, &logoHeight);
    logoWidth /= 2;
    logoHeight /= 2;
    SDL_Rect logoRect = { (width - logoWidth) / 2, 20, logoWidth, logoHeight };
    SDL_Rect bouton = sdl_bouton_start();

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    SDL_RenderCopy(renderer, Logo, NULL, &logoRect);
    SDL_RenderCopy(renderer, StartBouton, NULL, &bouton);
    SDL_RenderPresent(renderer);
}

void sdl_afficher_jeu(const Model *model, int **plateau, Gagnant resultat) {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);

    // une case de marge autour du plateau ; au moins 1 pixel par case
    int cellWidth = SDL_max(1, width / (model->colonnes + 2));
    int cellHeight = SDL_max(1, (height - BANDEAU_BAS) / (model->lignes + 2));
    int offsetX = (width - model->colonnes * cellWidth) / 2;
    int offsetY = (height - model->lignes * cellHeight - BANDEAU_BAS) / 2;

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

    SDL_Rect fond = { offsetX, offsetY, model->colonnes * cellWidth, model->lignes * cellHeight };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &fond);

    // joueur 1 en bleu, joueur 2 en rouge
    for (int i = 0; i < model->lignes; i++) {
        for (int j = 0; j < model->colonnes; j++) {
            if (plateau[i][j] == 0) {
                continue;
            }
            SDL_Rect cellRect = { offsetX + j * cellWidth, offsetY + i * cellHeight, cellWidth, cellHeight };
            if (plateau[i][j] == 1) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            }
            SDL_RenderFillRect(renderer, &cellRect);
        }
    }

    if (resultat != CONTINUER) {
        SDL_Rect bouton = sdl_bouton_restart();
        SDL_RenderCopy(renderer, RestartBouton, NULL, &bouton);
        // pas de SDL_ttf dans le projet : le résultat s'affiche dans le titre de la fenêtre
        char titre[64];
        snprintf(titre, sizeof titre, "Tron - %s", gagnant_message(resultat));
        SDL_SetWindowTitle(window, titre);
    } else {
        SDL_SetWindowTitle(window, "Tron - bleu : ZQSD, rouge : flèches");
    }
    SDL_RenderPresent(renderer);
}
