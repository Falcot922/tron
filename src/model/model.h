#ifndef MODEL_H
#define MODEL_H

// Le modèle : les règles du jeu et rien d'autre. Il ne connaît ni SDL ni ncurses,
// ce qui permet de le brancher sur n'importe quelle interface et de le tester seul.

typedef enum Directions {HAUT, BAS, GAUCHE, DROITE} Directions;
typedef enum Gagnant {JOUEUR1, JOUEUR2, CONTINUER, EGALITE} Gagnant;

typedef struct Moto {
    int x, y;
    Directions directions; // direction du dernier déplacement
    Directions prochaine;  // direction demandée par le joueur, appliquée au prochain déplacement
    int valeur;            // ce que la moto écrit sur le plateau (1 ou 2)
} Moto;

typedef struct Model {
    int lignes, colonnes;
} Model;

// Le plateau est un tableau lignes x colonnes : 0 = case libre, sinon la valeur de la moto passée par là
int **creationDuJeu(Moto *moto1, Moto *moto2, const Model *model);
void reinitialiser(int **plateau, Moto *moto1, Moto *moto2, const Model *model);
void free_jeu(int **plateau, Model model);

void tourner(Moto *moto, Directions direction);
Gagnant avancer(int **plateau, Moto *moto1, Moto *moto2, const Model *model);
const char *gagnant_message(Gagnant resultat);

#endif
