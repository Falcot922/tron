// Remplace <stdlib.h> pour la version WebAssembly : voir ../tron_web.c
#ifndef WEB_STDLIB_H
#define WEB_STDLIB_H

#include <stddef.h>

void *malloc(size_t taille);
void free(void *bloc);

#endif
