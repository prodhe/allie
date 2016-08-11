/*
 * process.c
 */

#include <math.h>
#include "process.h"
#include "main.h"
#include "init.h"
#include "collision.h"

/* Handle SDL events, like key presses */
int processEvents(game_t *game)
{
    SDL_Event event;
    int done = 0;

    /* Check for events */
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_WINDOWEVENT_CLOSE:
            {
                if (game->window)
                {
                    SDL_DestroyWindow(game->window);
                    game->window = NULL;
                }
            }
            break;
            case SDL_KEYDOWN:
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        done = 1;
                        break;
                    case SDLK_UP:
                        if (game->player.onPlatform)
                        {
                            game->player.dy = -7;
                            game->player.onPlatform = 0;
                        }
                        break;
                    case SDLK_RETURN:
                        if (game->state == GAME_STATE_OVER || game->state == GAME_STATE_RUN)
                        {
                            game->state = GAME_STATE_RUN;
                            initGame(game);
                        }
                        break;
                    case SDLK_d:
                        if (game->state == GAME_STATE_RUN)
                        {
                            game->player.health = 0;
                        }
                        break;
                    case SDLK_h:
                        if (game->state == GAME_STATE_RUN)
                        {
                            game->scrollX += WIN_WIDTH/4;
                            if (game->scrollX > 0)
                                game->scrollX = 0;
                        }
                        break;
                    case SDLK_l:
                        if (game->state == GAME_STATE_RUN)
                        {
                            game->scrollX -= WIN_WIDTH/4;
                        }
                        break;
                    case SDLK_p:
                        if (game->state == GAME_STATE_RUN)
                        {
                            game->pause = (game->pause)?0:1;
                        }
                        break;
                }
            }
            break;
            case SDL_QUIT:
                done = 1;
                break;
            default:
            break;
        }
    }

    if (game->state == GAME_STATE_RUN)
    {
        const Uint8 *state = SDL_GetKeyboardState(NULL);

        /* Higher jumping if you're holding the key down */
        if (state[SDL_SCANCODE_UP])
        {
            /* only allow higher jumping for the start of the jump */
            if (game->player.dy < -6.2)
                game->player.dy -= GRAVITY-GRAVITY/10;
        }

        /* Horizontal moving */
        if (state[SDL_SCANCODE_LEFT])
        {
            game->player.facingLeft = 1;
            game->player.dx -= 0.5;
            if (game->player.dx < -game->player.speed)
            {
                game->player.dx = -game->player.speed;
            }
        }
        else if (state[SDL_SCANCODE_RIGHT])
        {
            game->player.facingLeft = 0;
            game->player.dx += 0.5;
            if (game->player.dx > game->player.speed)
            {
                game->player.dx = game->player.speed;
            }
        }
        else
        {
            /* Create dx friction for smoother animation */
            game->player.dx *= 0.8f;
            if (fabsf(game->player.dx) < 0.1f)
            {
                game->player.dx = 0;
            }
        }
    }

    return done;
}

/* General processing, like moving, animations, health and game states */
void process(game_t *game)
{
    /* Start screen */
    if (game->state == GAME_STATE_START)
    {
        /* Time out until game starts */
        if (game->time > 180)
            game->state = GAME_STATE_RUN;

        /* Flip facing direction of character picture */
        /* if (game->time % 10 == 0) */
        /*     p->facingLeft = (p->facingLeft)?0:1; */
    }

    /* Game is running */
    if ((game->state == GAME_STATE_RUN || game->state == GAME_STATE_DEAD) && game->pause == 0)
    {
        /* GAME SCORE: Add 1/50 of the scrolling to the game score, but only
         * if it adds to the final score. You won't lose points for going
         * backwards */
        unsigned int newScore = ceil(abs(game->scrollX)/50);
        if (newScore > game->score)
            game->score = newScore;

        /* GAME SCROLL: Move with the player as long as game is not paused */
        if (!game->pause)
        {
            game->scrollX = -game->player.x+(WIN_WIDTH/3);
            if (game->scrollX > 0) game->scrollX = 0;
        }

        /* PLAYER: Check collisions */
        collision(game, &game->player, game->ground, NUM_GROUND, 0, 0);
        collision(game, &game->player, game->platform, NUM_PLATFORM, 0, 0);
        collision(game, &game->player, game->obstacle, NUM_OBSTACLE, 0, 0);
        collision(game, &game->player, game->enemy, NUM_ENEMY, 0, 1);

        /* PLAYER: Manage health and movement
         * GAME STATE: Change depending on player actions */
        short oldState = game->state;
        game->state = managePlayer(&game->player, game->state, game->time);
        if (game->state - oldState != 0)
            switch(game->state)
            {
                case GAME_STATE_DEAD:
                    Mix_PlayChannel(-1, game->soundDie, 0);
                    break;
                case GAME_STATE_PLAYERJUSTHIT:
                    Mix_PlayChannel(-1, game->soundHit, 0);
                    game->state = GAME_STATE_RUN;
                    break;
                default:
                    break;
            }

        /* PLAYER: Apply gravity */
        applyGravity(&game->player);

        /* PLAYER: Do animations */
        animatePlayer(&game->player, game->state, game->time);

        /* For loops */
        int i=0;

        /* ENEMIES: Collision, movement and actions with some stupid AI
         * and then add gravity for the next round */
        for (int i=0; i < NUM_ENEMY; i+=1)
            if (nearScreen(&game->enemy[i], game))
            {
                collision(game, &game->enemy[i], game->ground, NUM_GROUND, 1, 0);
                collision(game, &game->enemy[i], game->platform, NUM_PLATFORM, 1, 0);
                collision(game, &game->enemy[i], game->obstacle, NUM_OBSTACLE, 0, 0);

                manageEnemy(&game->enemy[i], game->time);
                applyGravity(&game->enemy[i]);
                animateEnemy(&game->enemy[i], game->time);
            }

        /* OBJECTS: Collision, movement and gravity */
        for (i=0; i < NUM_GROUNDCLUTTER; i+=1)
            if (nearScreen(&game->groundClutter[i], game))
            {
                collision(game, &game->groundClutter[i], game->ground, NUM_GROUND, 1, 0);

                if (game->groundClutter[i].y < WIN_HEIGHT)
                    game->groundClutter[i].y += game->groundClutter[i].dy;

                applyGravity(&game->groundClutter[i]);
            }
        for (i=0; i < NUM_OBSTACLE; i+=1)
            if (nearScreen(&game->obstacle[i], game))
            {
                collision(game, &game->obstacle[i], game->ground, NUM_GROUND, 1, 0);
                collision(game, &game->obstacle[i], game->platform, NUM_PLATFORM, 1, 0);
                collision(game, &game->obstacle[i], game->obstacle, NUM_OBSTACLE, 1, 0);

                if (game->obstacle[i].y < WIN_HEIGHT)
                    game->obstacle[i].y += game->obstacle[i].dy;

                applyGravity(&game->obstacle[i]);
            }

    } /* GAME_STATE_RUN */

    /* Add time regardless of state */
    game->time += 1;
}

/* Manage player: health, movement
 * Returns GAME_STATE_xxxx */
short managePlayer(Object* obj, short state, unsigned int gameTime)
{
    /* PLAYER HEALTH: Kill the player if he falls off screen
     * and go to game over screen if he's already dead */
    if (obj->y > WIN_HEIGHT)
    {
        obj->health = 0;
        if (state == GAME_STATE_DEAD)
            state = GAME_STATE_OVER;
    }

    /* PLAYER HEALTH: Game state dead if below or 0 */
    if (obj->health <= 0 && state == GAME_STATE_RUN)
    {
        obj->health = 0;
        obj->solid = 0;
        obj->dy = -13;
        obj->dx = 0;
        state = GAME_STATE_DEAD;
    }

    /* Return a one-frame only state where the player got hit.
     * Used for sounds and animations */
    if (obj->health > 0 && obj->timeHit == gameTime)
        state = GAME_STATE_PLAYERJUSTHIT;

    /* PLAYER MOVE: If he's mid-air and at the end of the
     * high jump, gradually slow his dx down to a lesser speed */
    /* if (!obj->onPlatform && obj->dy > -2) */
    /* { */
    /*     if (obj->dx > obj->speed/1.1) */
    /*         obj->dx *= 0.95; */
    /* } */

    /* Move player according to current directions,
     * but not off screen to the left. */
    obj->x += obj->dx;
    obj->y += obj->dy;

    if (obj->x < 0)
        obj->x = 0;

    /* Return the eventually new state */
    return state;
}

/* Manage enemy health and stupid AI movement */
void manageEnemy(Object *obj, unsigned int gameTime)
{
    if (obj->health > 0)
    {
        /* If enemy hit a wall or a cliff (collision detection will set dx=0),
         * change direction */
        if (obj->dx == 0)
        {
            /* Enemy textures are reversed, so facingLeft
             * is reversed as well */
            if (obj->facingLeft)
                obj->dx = (-1)*obj->speed;
            else
                obj->dx = 1*obj->speed;
        }
        /* Stupid forward and backward AI if we're already moving */
        else if (gameTime % 50 == 0 && obj->dx != 0)
        {
            /* Randomize whether this exact enemy decides to
             * turn or not */
            if (random()%5 == 0)
            {
                /* change x direction (facingLeft is reversed for enemies) */
                obj->dx *= (-1);
                if (obj->dx < 0)
                    obj->facingLeft = 0;
                else
                    obj->facingLeft = 1;
            }
        }

        /* DEBUG FLYING */
        /* for (int i=0; i < NUM_ENEMY; i+=1) */
        /* { */
        /*     if (game->enemy[i].aerial) */
        /*     { */
        /*         Object *a = &game->enemy[i]; */
        /*         printf("x: %f, dx: %f, y: %f, dy: %f\n", a->x, a->dx, a->y, a->dy); */
        /*         break; */
        /*     } */
        /* } */

        /* stupid up and down AI */
        if (obj->aerial)
        {
            if (obj->dy > 4)
            {
                /* Vertical flying movement for the aerial creatures */
                obj->dy -= (obj->dy*2)-GRAVITY;
            }
        }

        /* Do the actual movement of enemy horizontally */
        if (obj->onPlatform || obj->aerial)
        {
            obj->x += obj->dx;

            /* Set facing direction (which is reversed) */
            obj->facingLeft = (obj->dx > 0) ? 1: 0;
        }

    }
    /* Enemy is equal or below 0 health */
    else
    {
        /* Force health to 0 and make enemy non-collisionable */
        obj->health = 0;
        obj->solid = 0;

    }

    /* always do vertical movement to allow gravity */
    if (obj->y < WIN_HEIGHT)
        obj->y += obj->dy;
}

/* Apply gravity onto object */
void applyGravity(Object *obj)
{
    if (obj->gravityBound > 0 && obj->dy < MAX_GRAVITY)
    {
        obj->dy += GRAVITY;
        if (obj->dy > MAX_GRAVITY)
            obj->dy = MAX_GRAVITY;
    }
}

/* ANIMATION: Player */
void animatePlayer(Object *obj, short state, unsigned int gameTime)
{
    if (state == GAME_STATE_RUN && gameTime%2 == 0)
    {
        /* If mid-air */
        if (!obj->onPlatform)
            obj->animFrame = 11; // jump
        /* On platform */
        else
        {
            /* Standing still */
            if (obj->dx == 0)
                obj->animFrame = 0; // default
            /* Moving */
            else
            {
                /* rotate walking animation:
                * 11 pictures in range [0]--[10] */
                if (obj->animFrame >= 10)
                    obj->animFrame = 0;
                else
                    obj->animFrame += 1;
            }
        }
    }
    else if (state == GAME_STATE_DEAD)
        obj->animFrame = 13; /* head */
}

/* ANIMATION: Enemy */
void animateEnemy(Object *obj, unsigned int gameTime)
{
    if (obj->health <= 0)
        obj->animFrame = 2; //dead
    else
    {
        /* Ground enemies */
        if (!obj->aerial && obj->onPlatform && gameTime % 30 == 0)
        {
            /* alternate betweeen frame 0 and 1 */
            obj->animFrame = (obj->animFrame == 0) ? 1 : 0;
        }
        /* Flying enemies */
        else if (obj->aerial && gameTime % 5 == 0)
        {
            /* alternate betweeen frame 0 and 1 */
            obj->animFrame = (obj->animFrame == 0) ? 1 : 0;
        }
    }
}
