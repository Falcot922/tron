#include <assert.h>
#include <stdlib.h>
#include "model.h"

// Message de fin de partie
const char *gagnant_message(Gagnant resultat) {
    switch (resultat) {
        case JOUEUR1: return "Le joueur 1 a gagné !";
        case JOUEUR2: return "Le joueur 2 a gagné !";
        case EGALITE: return "Égalité !";
        default: return "";
    }
}

// Vide le plateau et replace les motos au départ. Sert à la création et pour rejouer :
// le plateau n'est alloué qu'une fois par programme.
void reinitialiser(int **plateau, Moto *moto1, Moto *moto2, const Model *model) {
    for (int i = 0; i < model->lignes; i++) {
        for (int j = 0; j < model->colonnes; j++) {
            plateau[i][j] = 0;
        }
    }

    // Positions et directions de départ (choisies arbitrairement, faciles à changer)
    moto1->x = model->colonnes / 5;
    moto1->y = model->lignes / 2;
    moto1->valeur = 1;
    moto1->directions = moto1->prochaine = HAUT;

    moto2->x = (model->colonnes / 5) * 4;
    moto2->y = model->lignes / 2;
    moto2->valeur = 2;
    moto2->directions = moto2->prochaine = BAS;

    plateau[moto1->y][moto1->x] = moto1->valeur;
    plateau[moto2->y][moto2->x] = moto2->valeur;
}

// Création du plateau et placement des motos
int **creationDuJeu(Moto *moto1, Moto *moto2, const Model *model) {
    // en dessous de 5 colonnes, les deux motos partiraient de la même case
    assert(model->lignes >= 1 && model->colonnes >= 5);

    int **plateau = malloc(model->lignes * sizeof(int *));
    assert(plateau);
    for (int i = 0; i < model->lignes; i++) {
        plateau[i] = malloc(model->colonnes * sizeof(int));
        assert(plateau[i]);
    }

    reinitialiser(plateau, moto1, moto2, model);
    return plateau;
}

void free_jeu(int **plateau, Model model) {
    for (int i = 0; i < model.lignes; i++) {
        free(plateau[i]);
    }
    free(plateau);
}

static int verticale(Directions direction) {
    return direction == HAUT || direction == BAS;
}

// Demande de virage : seul un quart de tour est permis. On compare à la direction du dernier
// déplacement, pas à la dernière demande : deux touches entre deux pas ne font pas un demi-tour.
void tourner(Moto *moto, Directions direction) {
    if (verticale(direction) != verticale(moto->directions)) {
        moto->prochaine = direction;
    }
}

// Avance la moto d'une case si elle est libre. Retourne 0 si elle est bloquée (mur ou trace).
static int avancer_moto(int **plateau, Moto *moto, const Model *model) {
    int x = moto->x, y = moto->y;
    moto->directions = moto->prochaine;

    switch (moto->directions) {
        case HAUT: y -= 1; break;
        case BAS: y += 1; break;
        case GAUCHE: x -= 1; break;
        case DROITE: x += 1; break;
    }
    if (x < 0 || x >= model->colonnes || y < 0 || y >= model->lignes || plateau[y][x] != 0) {
        return 0;
    }
    moto->x = x;
    moto->y = y;
    return 1;
}

// Un pas de jeu : avance les deux motos et retourne CONTINUER, ou l'issue de la partie.
// À n'appeler qu'une fois par pas : après une fin de partie, il faut reinitialiser().
Gagnant avancer(int **plateau, Moto *moto1, Moto *moto2, const Model *model) {
    int avance1 = avancer_moto(plateau, moto1, model);
    int avance2 = avancer_moto(plateau, moto2, model);

    // les deux motos bloquées, ou arrivées sur la même case
    if ((!avance1 && !avance2) || (moto1->x == moto2->x && moto1->y == moto2->y)) {
        return EGALITE;
    }
    if (!avance1) {
        return JOUEUR2;
    }
    if (!avance2) {
        return JOUEUR1;
    }

    plateau[moto1->y][moto1->x] = moto1->valeur;
    plateau[moto2->y][moto2->x] = moto2->valeur;
    return CONTINUER;
}
