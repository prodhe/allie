/*
 *  main.h
 */

#ifndef game_main_h
#define game_main_h

/* Includes */
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"

/* Macros */
#define dprint(expr, time) printf("%u: " #expr "\n", time)

/* Defines */
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define GRAVITY 0.30f
#define MAX_GRAVITY 15

#define GAME_STATE_START 0
#define GAME_STATE_RUN 1
#define GAME_STATE_PAUSE 2
#define GAME_STATE_DEAD 3
#define GAME_STATE_OVER 4
#define GAME_STATE_PLAYERJUSTHIT 5

#define DAMAGE_ALL 15    /* 1111 */
#define DAMAGE_TOP 1     /* 0001 */
#define DAMAGE_RIGHT 2   /* 0010 */
#define DAMAGE_BOTTOM 4  /* 0100 */
#define DAMAGE_LEFT 8    /* 1000 */

#define FRAGILE_ALL 15    /* 1111 */
#define FRAGILE_TOP 1     /* 0001 */
#define FRAGILE_RIGHT 2   /* 0010 */
#define FRAGILE_BOTTOM 4  /* 0100 */
#define FRAGILE_LEFT 8    /* 1000 */

#define NUM_GROUND 1000
#define NUM_GROUNDCLUTTER 200
#define NUM_PLATFORM 500
#define NUM_OBSTACLE 200
#define NUM_CLOUD 200
#define NUM_ENEMY 100

/* All in-game objects */
typedef struct
{
    float x, y, dx, dy;
    int w, h;
    int animFrame, health, damage, speed;
    unsigned int scoreValue, timeHit;
    short solid, aerial, gravityBound, onPlatform,
          facingLeft, damageSides, fragileSides;
    SDL_Texture **textures;
} Object;

/* Loaded textures */
typedef struct
{
    /* Images */
    SDL_Texture *player[15];
    SDL_Texture *ground[6];
    SDL_Texture *groundClutter[3];
    SDL_Texture *platform[2];
    SDL_Texture *obstacle[4];
    SDL_Texture *cloud[3];
    SDL_Texture *enemySlime[3];
    SDL_Texture *enemyFly[3];
} Textures;

/* Main game machine */
typedef struct
{
    /* Objects */
    Object player;
    Object ground[NUM_GROUND];
    Object groundClutter[NUM_GROUNDCLUTTER];
    Object platform[NUM_PLATFORM];
    Object obstacle[NUM_OBSTACLE];
    Object cloud[NUM_CLOUD];
    Object enemy[NUM_ENEMY];

    /* Textures */
    Textures textures;

    /* Fonts */
    TTF_Font *font;

    /* Sounds */
    Mix_Chunk *soundMusic;
    Mix_Chunk *soundHit, *soundDie;

    /* Time frames, scrolling, score*/
    int scrollX;
    unsigned int time, score;

    /* Game state and pause */
    short state, pause;

    /* SDL window and renderer */
    SDL_Window *window;
    SDL_Renderer *renderer;
} GameState;

/* Main function */
int main(int argc, char *argv[]);

/* Check whether an object is (near) within screen size.
 * This is used throughout the program to minimize
 * drawing, collision detection and movement of
 * objects that is not on the screen anyway */
int onScreen(Object *obj, GameState *game);
int nearScreen(Object *obj, GameState *game);

#endif
