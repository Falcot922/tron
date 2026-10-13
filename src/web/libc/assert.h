// Remplace <assert.h> pour la version WebAssembly : voir ../tron_web.c
#ifndef WEB_ASSERT_H
#define WEB_ASSERT_H

#define assert(condition) ((condition) ? (void)0 : __builtin_trap())

#endif
