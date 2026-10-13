// Pont entre le modèle et le navigateur : compilé avec model.c en WebAssembly (make web).
// La page web joue le rôle de la vue et du contrôleur ; les règles restent celles du modèle.
//
// Il n'y a pas de bibliothèque C dans un navigateur. Le modèle n'en utilise que malloc, free
// et assert : les deux en-têtes de libc/ et l'allocateur ci-dessous suffisent à les fournir.
#include <stddef.h>
#include "model/model.h"

#define EXPORT(nom) __attribute__((export_name(#nom))) nom

#define LIGNES 40
#define COLONNES 60

static Model model = { LIGNES, COLONNES };
static Moto motos[2];
static int **plateau;

// Le plateau est alloué une seule fois et jamais libéré : un allocateur qui avance dans une réserve suffit
static unsigned char reserve[64 * 1024];
static size_t utilise;

void *malloc(size_t taille) {
    taille = (taille + 7) & ~(size_t)7; // aligné sur 8 octets
    if (utilise + taille > sizeof reserve) {
        __builtin_trap();
    }
    void *bloc = reserve + utilise;
    utilise += taille;
    return bloc;
}

void free(void *bloc) {
    (void)bloc;
}

void EXPORT(web_nouvelle_partie)(void) {
    if (plateau) {
        reinitialiser(plateau, &motos[0], &motos[1], &model);
    } else {
        plateau = creationDuJeu(&motos[0], &motos[1], &model);
    }
}

// joueur : 0 ou 1 ; direction et valeur de retour de web_avancer : les enums de model.h
void EXPORT(web_tourner)(int joueur, int direction) { tourner(&motos[joueur], direction); }
int EXPORT(web_avancer)(void) { return avancer(plateau, &motos[0], &motos[1], &model); }

int EXPORT(web_lignes)(void) { return model.lignes; }
int EXPORT(web_colonnes)(void) { return model.colonnes; }
int EXPORT(web_case)(int ligne, int colonne) { return plateau[ligne][colonne]; }
int EXPORT(web_x)(int joueur) { return motos[joueur].x; }
int EXPORT(web_y)(int joueur) { return motos[joueur].y; }
int EXPORT(web_direction)(int joueur) { return motos[joueur].directions; }
