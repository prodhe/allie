/*
 *  collision.h
 */

#ifndef game_collision_h
#define game_collision_h

#include "main.h"

float collide2d(float ax, float ay, float aw, float ah,
                float bx, float by, float bw, float bh);

int collision(GameState *game, Object *obj, Object *target, int n,
              short checkCliff, short debug);
int verticalCollision(Object* obj, Object *target, unsigned int gameTime, short debug);
int horizontalCollision(Object* obj, Object *target, unsigned int gameTime, short debug);
int isObjectCrashing(Object *obj, Object *target);

void hurtObject(Object *reciever, Object *dealer, unsigned int gameTime);

#endif

