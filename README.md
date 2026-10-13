# Tron

Jeu de Tron réalisé par **[Grégory DUPORGE](https://gregoryduporge.me/)** et **Julien Biout** dans le cadre d'une SAé en C.

Deux motos laissent une trace derrière elles ; la première qui percute un mur ou une trace a perdu.

Le projet suit l'architecture MVC : un seul modèle, et trois interfaces qui s'y branchent.

| Interface | Technologie | Lancement |
|---|---|---|
| Graphique | **SDL2** | `./tron -sdl` |
| Texte | **ncurses** | `./tron -ncurse` |
| Navigateur | **WebAssembly** | `make web`, voir plus bas |

## Installation

Sous Ubuntu ou Debian :

```bash
sudo apt update
sudo apt install build-essential pkg-config libsdl2-dev libsdl2-image-dev libncurses-dev
```

## Compilation

```bash
make          # compile ./tron
make test     # teste les règles du jeu (ne demande ni SDL ni ncurses)
make clean
```

## Jouer

Le jeu se lance depuis la racine du projet (les images y sont cherchées).

```bash
./tron -sdl                    # interface graphique (par défaut)
./tron -sdl -taille 700 700    # taille de la fenêtre : <hauteur> <largeur>
./tron -ncurse                 # interface texte, dans le terminal
```

| | Joueur 1 (bleu) | Joueur 2 (rouge) |
|---|---|---|
| Touches | `Z` `Q` `S` `D` | flèches |

Seuls les quarts de tour sont possibles. Si les deux motos se percutent ou sont bloquées au même
instant, il y a égalité.

## Version navigateur

Le modèle ne dépend d'aucune bibliothèque : il se compile tel quel en WebAssembly.

```bash
sudo apt install clang lld
make web                        # écrit build/web/tron.js
make web WEB_DIR=chemin/du/site # ou directement dans un site
```

`tron.js` contient le modèle compilé. La page qui le charge n'a plus qu'à transmettre les virages
et à dessiner le plateau : c'est ce que fait la version jouable du
[portfolio](https://github.com/Falcot922/portfolio) (`projets/tron/`).

## Organisation

```
src/model/        les règles : plateau, motos, déplacements, fin de partie
src/controller/   lecture des touches et boucle de jeu, pour SDL2 et pour ncurses
src/view/         affichage SDL2 et affichage ncurses
src/web/          pont entre le modèle et le navigateur
src/utils/img/    images de l'interface SDL2
tests/            tests du modèle
```
