// Tests du modèle seul : make test
#include <assert.h>
#include <stdio.h>
#include "model/model.h"

static Model model = { 10, 20 };
static Moto m1, m2;

// Avance tant que la partie continue, au plus `pas` fois
static Gagnant jouer(int **plateau, int pas) {
    Gagnant resultat = CONTINUER;
    while (pas-- > 0 && resultat == CONTINUER) {
        resultat = avancer(plateau, &m1, &m2, &model);
    }
    return resultat;
}

int main(void) {
    int **plateau = creationDuJeu(&m1, &m2, &model);

    // départ : moto 1 au quart gauche vers le haut, moto 2 au quart droit vers le bas
    assert(m1.x == 4 && m1.y == 5 && m1.directions == HAUT);
    assert(m2.x == 16 && m2.y == 5 && m2.directions == BAS);
    assert(plateau[5][4] == 1 && plateau[5][16] == 2);

    // un pas : chacune avance et laisse sa trace
    assert(avancer(plateau, &m1, &m2, &model) == CONTINUER);
    assert(m1.y == 4 && m2.y == 6 && plateau[4][4] == 1 && plateau[6][16] == 2);

    // mur : la moto 2 touche le bas (ligne 9) avant que la moto 1 touche le haut
    assert(jouer(plateau, 100) == JOUEUR1);

    // les deux touchent un mur au même pas : égalité
    reinitialiser(plateau, &m1, &m2, &model);
    plateau[m2.y][m2.x] = 0;
    m2.y = 4; // même distance au mur que la moto 1
    plateau[m2.y][m2.x] = m2.valeur;
    assert(jouer(plateau, 100) == EGALITE);

    // virage : un quart de tour est accepté, un demi-tour refusé
    reinitialiser(plateau, &m1, &m2, &model);
    tourner(&m1, BAS);
    assert(m1.prochaine == HAUT);
    tourner(&m1, DROITE);
    assert(m1.prochaine == DROITE);
    // deux touches avant le pas suivant ne font pas un demi-tour (HAUT -> DROITE -> BAS)
    tourner(&m1, BAS);
    assert(m1.prochaine == DROITE);
    assert(avancer(plateau, &m1, &m2, &model) == CONTINUER);
    assert(m1.x == 5 && m1.y == 5 && m1.directions == DROITE);

    // trace : la moto 1 fait demi-tour en deux virages et percute sa propre trace
    reinitialiser(plateau, &m1, &m2, &model);
    tourner(&m2, GAUCHE);                                  // la moto 2 s'écarte du mur
    assert(avancer(plateau, &m1, &m2, &model) == CONTINUER); // m1 en (4,4)
    tourner(&m1, DROITE);
    assert(avancer(plateau, &m1, &m2, &model) == CONTINUER); // (5,4)
    tourner(&m1, BAS);
    assert(avancer(plateau, &m1, &m2, &model) == CONTINUER); // (5,5)
    tourner(&m1, GAUCHE);
    assert(avancer(plateau, &m1, &m2, &model) == JOUEUR2);   // (4,5) = sa case de départ

    // face à face : les deux motos arrivent sur la même case
    reinitialiser(plateau, &m1, &m2, &model);
    m1.x = 8; m1.directions = m1.prochaine = DROITE;
    m2.x = 10; m2.directions = m2.prochaine = GAUCHE;
    assert(avancer(plateau, &m1, &m2, &model) == EGALITE);

    // rejouer : le plateau est vidé
    reinitialiser(plateau, &m1, &m2, &model);
    int occupees = 0;
    for (int i = 0; i < model.lignes; i++) {
        for (int j = 0; j < model.colonnes; j++) {
            occupees += plateau[i][j] != 0;
        }
    }
    assert(occupees == 2);

    free_jeu(plateau, model);
    printf("test_model : OK\n");
    return 0;
}
