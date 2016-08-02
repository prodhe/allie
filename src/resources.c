/*
 * resources.c
 */

#include "resources.h"
#include "main.h"

/* External resource loading, such as textures, fonts and sounds */
void loadResources(GameState *game)
{
    /* Lazy short handle for game textures struct */
    Textures *gt = &game->textures;

    /* Reusable variables */
    SDL_Surface *surface = NULL;
    int i;
    char src[128];

    /* Ground textures */
    gt->ground[0] = NULL;
    loadTexture(game->renderer, &gt->ground[0], "resources/png/ground.png");
    gt->ground[1] = NULL;
    loadTexture(game->renderer, &gt->ground[1], "resources/png/ground_dirt.png");
    gt->ground[2] = NULL;
    loadTexture(game->renderer, &gt->ground[2], "resources/png/ground_sand.png");
    gt->ground[3] = NULL;
    loadTexture(game->renderer, &gt->ground[3], "resources/png/ground_rock.png");
    gt->ground[4] = NULL;
    loadTexture(game->renderer, &gt->ground[4], "resources/png/ground_cave.png");
    gt->ground[5] = NULL;
    loadTexture(game->renderer, &gt->ground[5], "resources/png/water.png");

    /* Platform textures */
    gt->platform[0] = NULL;
    loadTexture(game->renderer, &gt->platform[0], "resources/png/block.png");
    gt->platform[1] = NULL;
    loadTexture(game->renderer, &gt->platform[1], "resources/png/plank.png");

    /* Cloud textures */
    for (i=0; i < 3; i+=1)
    {
        gt->cloud[i] = NULL;
        sprintf(src, "resources/png/cloud_%d.png", i+1);
        loadTexture(game->renderer, &gt->cloud[i], src);
    }

    /* Ground clutter textures for good looks */
    gt->groundClutter[0] = NULL;
    loadTexture(game->renderer, &gt->groundClutter[0], "resources/png/bush.png");
    gt->groundClutter[1] = NULL;
    loadTexture(game->renderer, &gt->groundClutter[1], "resources/png/grass.png");
    gt->groundClutter[2] = NULL;
    loadTexture(game->renderer, &gt->groundClutter[2], "resources/png/rock.png");

    /* Obstacles textures */
    gt->obstacle[0] = NULL;
    loadTexture(game->renderer, &gt->obstacle[0], "resources/png/hill_short.png");
    gt->obstacle[1] = NULL;
    loadTexture(game->renderer, &gt->obstacle[1], "resources/png/hill_long.png");
    gt->obstacle[2] = NULL;
    loadTexture(game->renderer, &gt->obstacle[2], "resources/png/crate.png");
    gt->obstacle[3] = NULL;
    loadTexture(game->renderer, &gt->obstacle[3], "resources/png/spikes.png");


    /* Enemy textures */
    for (i=0; i < 3; i+=1)
    {
        gt->enemySlime[i] = NULL;
        sprintf(src, "resources/png/enemies/slime_%d.png", i+1);
        loadTexture(game->renderer, &gt->enemySlime[i], src);
    }
    for (i=0; i < 3; i+=1)
    {
        gt->enemyFly[i] = NULL;
        sprintf(src, "resources/png/enemies/fly_%d.png", i+1);
        loadTexture(game->renderer, &gt->enemyFly[i], src);
    }

    /* Character walking */
    for (i=0; i < 11; i+=1)
    {
        gt->player[i] = NULL;
        sprintf(src, "resources/png/character/walk/walk00%02d.png", i+1);
        loadTexture(game->renderer, &gt->player[i], src);
    }

    /* Character jump, front and head */
    gt->player[11] = NULL;
    loadTexture(game->renderer, &gt->player[11], "resources/png/character/jump.png");
    gt->player[12] = NULL;
    loadTexture(game->renderer, &gt->player[12], "resources/png/character/front.png");
    gt->player[13] = NULL;
    loadTexture(game->renderer, &gt->player[13], "resources/png/character/head.png");
    gt->player[14] = NULL;
    loadTexture(game->renderer, &gt->player[14], "resources/png/character/head_small.png");

    /* Fonts */
    game->font = NULL;
    game->font = TTF_OpenFont("resources/Gloria.ttf", 48);
    if (game->font == NULL)
    {
        printf("Cannot find font: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }

    surface = NULL;

    /* Sounds */
    /* game->soundMusic = Mix_LoadMUS("resources/erik_skiff-we_are_the_resistors.ogg"); */
    game->soundMusic = NULL;
    game->soundMusic = Mix_LoadWAV("resources/erik_skiff-we_are_the_resistors.wav");
    if (game->soundMusic == NULL)
    {
        printf("music error: %s\n", Mix_GetError());
    }
    else
    {
        Mix_VolumeChunk(game->soundMusic, MIX_MAX_VOLUME/3);
    }
    game->soundHit = Mix_LoadWAV("resources/omph.wav");
    game->soundDie = Mix_LoadWAV("resources/wilhelm.wav");
    if (game->soundDie != NULL)
    {
        Mix_VolumeChunk(game->soundDie, MIX_MAX_VOLUME/2);
    }
}

/* Load texture */
void loadTexture(SDL_Renderer *renderer, SDL_Texture **texture, char *src)
{
    /* Load surface from source string */
    SDL_Surface *surface = NULL;
    surface = IMG_Load(src);
    if (surface == NULL)
    {
        printf("Cannot find %s\n", src);
        SDL_Quit();
        exit(1);
    }

    /* Save surface to texture */
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL)
    {
        printf("Cannot create texture: %s\n", src);
        SDL_Quit();
        exit(1);
    }

    /* Memory clean */
    SDL_FreeSurface(surface);
    surface = NULL;
}

/* Memory cleanup when program exits */
void freeResources(GameState *game)
{
    /* Free textures */
    Textures *gt = &game->textures;
    int i;

    /* Player */
    for (i=0; i < 15; i+=1)
    {
        if (gt->player[i] != NULL)
            SDL_DestroyTexture(gt->player[i]);
    }

    /* Ground */
    for (i=0; i < 6; i+=1)
    {
        if (gt->ground[i] != NULL)
            SDL_DestroyTexture(gt->ground[i]);
    }

    /* Ground clutter */
    for (i=0; i < 3; i+=1)
    {
        if (gt->groundClutter[i] != NULL)
            SDL_DestroyTexture(gt->groundClutter[i]);
    }

    /* Platform */
    for (i=0; i < 2; i+=1)
    {
        if (gt->platform[i] != NULL)
            SDL_DestroyTexture(gt->platform[i]);
    }

    /* Obstacle */
    for (i=0; i < 4; i+=1)
    {
        if (gt->obstacle[i] != NULL)
            SDL_DestroyTexture(gt->obstacle[i]);
    }

    /* Cloud */
    for (i=0; i < 3; i+=1)
    {
        if (gt->cloud[i] != NULL)
            SDL_DestroyTexture(gt->cloud[i]);
    }

    /* Enemy */
    for (i=0; i < 3; i+=1)
    {
        if (gt->enemySlime[i] != NULL)
            SDL_DestroyTexture(gt->enemySlime[i]);
    }

    /* Enemy */
    for (i=0; i < 3; i+=1)
    {
        if (gt->enemyFly[i] != NULL)
            SDL_DestroyTexture(gt->enemyFly[i]);
    }

    /* Free fonts */
    TTF_CloseFont(game->font);

    /* Stop all audio from playing before Mix_FreeChunk */
    Mix_HaltMusic();
    Mix_HaltChannel(-1);

    /* Free sounds */
    if (game->soundMusic != NULL)
    {
        /* Mix_FreeMusic(game->soundMusic); */
        Mix_FreeChunk(game->soundMusic);
    }
    if (game->soundHit != NULL)
        Mix_FreeChunk(game->soundHit);
    if (game->soundDie != NULL)
        Mix_FreeChunk(game->soundDie);

    /* Free window, renderer and close down TTF and SDL */
    if (game->window != NULL)
        SDL_DestroyWindow(game->window);
    if (game->renderer != NULL)
        SDL_DestroyRenderer(game->renderer);
}
