/* 
 * process.h
 */

#ifndef game_process_h
#define game_process_h

#include "main.h"

int processEvents(game_t *game);
void process(game_t *game);

short managePlayer(Object *obj, short state, unsigned int gameTime);
void animatePlayer(Object *obj, short state, unsigned int gameTime);

void manageEnemy(Object *obj, unsigned int gameTime);
void animateEnemy(Object *obj, unsigned int gameTime);

void applyGravity(Object *obj);

#endif
