/* SDL playground */
   
#include <stdio.h>
#include <time.h>
#include "main.h"
#include "resources.h"
#include "init.h"
#include "process.h"
#include "render.h"

/* Main */
int main(int argc, char *argv[])
{
    /* State "object" */
    GameState game;

    /* SDL initialization */
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    Mix_Init(MIX_INIT_OGG);
    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);

    /* Basic SDL window and renderer */
    game.window = NULL;
    game.renderer = NULL;
    game.window = SDL_CreateWindow("Allie",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              WIN_WIDTH,
                              WIN_HEIGHT,
                              0
                              );
    game.renderer = SDL_CreateRenderer(game.window, -1,
                                  SDL_RENDERER_ACCELERATED
                                  | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(game.renderer, WIN_WIDTH, WIN_HEIGHT);

    /* Seed random */
    srandom(time(NULL));

    /* Load and initialize game */
    loadResources(&game);
    initGame(&game);

    /* Game event loop */
	int done = 0;
	while (!done)
    {
        /* Check for events (and maybe a quit) */
        done = processEvents(&game);

        /* General processing along with collision detection */
        process(&game);

        /* Render screen */
        doRender(&game);
	}

    /* Clean up memory */
    freeResources(&game);

    /* SDL tools */
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    TTF_Quit();
	SDL_Quit();

    /* Exit main */
	return 0;
}

/* On screen detection of object.
 * Mainly used for rendering to avoid calculate stuff that
 * is not visible anyway */
int onScreen(Object *obj, GameState *game)
{
    if (obj->x < abs(game->scrollX)+WIN_WIDTH &&   /* right edge */
        abs(game->scrollX) - obj->x < 0+obj->w &&  /* left edge */
        obj->y < WIN_HEIGHT && obj->y > 0-obj->h)  /* bottom and top edge */
        return 1;
    else
        return 0;
}

/* Near screen detection of object.  This is used on all moving objects that
 * needs manipulation, even when it's not visible (ie, enemies and objects
 * that are gravity bound). If it's too far off though, let it be. Too far
 * is an additional screen in x and an additional screen on top. If it falls
 * below bottom, it is discarded immediately. */
int nearScreen(Object *obj, GameState *game)
{
    if (obj->x < abs(game->scrollX)+WIN_WIDTH*2 &&           /* right edge */
        abs(game->scrollX) - obj->x - WIN_WIDTH < 0+obj->w &&  /* left edge */
        obj->y < WIN_HEIGHT && obj->y > 0-obj->h-WIN_HEIGHT) /* bottom and top edge */
        return 1;
    else
        return 0;
}


/* EOF */
