/*
 * resources.h
 */

#ifndef game_resources_h
#define game_resources_h

#include "main.h"

void loadResources(game_t *game);
void loadTexture(SDL_Renderer *renderer, SDL_Texture **texture, char *src);
void freeResources(game_t *game);

#endif
