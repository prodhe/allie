/*
 * resources.h
 */

#ifndef game_resources_h
#define game_resources_h

#include "main.h"

void loadResources(GameState *game);
void loadTexture(SDL_Renderer *renderer, SDL_Texture **texture, char *src);
void freeResources(GameState *game);

#endif
