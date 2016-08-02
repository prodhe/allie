/* 
 * render.h
 */

#ifndef game_render_h
#define game_render_h

#include "main.h"

void doRender(GameState *game);
void renderText(char *str, int x, int y, SDL_Color color, short center,
                TTF_Font *font, SDL_Renderer *renderer);
void renderLoadedTexture(SDL_Texture *texture, int x, int y, short center,
                         short flip, SDL_Renderer *renderer);

#endif
