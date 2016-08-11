/*
 * render.c
 */

#include "render.h"
#include "main.h"

/* Do the actual game rendering to screen */
void doRender(game_t *game)
{
    /* Placeholder for target SDL_Rect struct for every texture to be copied
        * onto the renderer */
    SDL_Rect rect = { 0, 0, 0, 0 };

    /* Placeholder for width and height of queried texture */
    int w = 0;
    int h = 0;

    /* Placeholder for printing non-static text, along with some predefined
     * colors */
    char text[128];
    SDL_Color colorWhite = { 255, 255, 255, 255 };

    Textures *gt = &game->textures;

    if (game->state == GAME_STATE_START)
    {
        /* Background and clear */
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
        SDL_RenderClear(game->renderer);

        /* Draw header */
        renderText("Allie", WIN_WIDTH/2, WIN_HEIGHT/2-100, colorWhite, 1,
                   game->font, game->renderer);

        /* Draw player */
        renderLoadedTexture(gt->player[12],
                            WIN_WIDTH/2,
                            WIN_HEIGHT/2,
                            1,
                            0,
                            game->renderer);

    }
    else if (game->state == GAME_STATE_RUN || game->state == GAME_STATE_DEAD)
    {
        /* Background and clear */
        SDL_SetRenderDrawColor(game->renderer, 128, 128, 255, 255);
        SDL_RenderClear(game->renderer);

        /* For the loops */
        int i;

        /* Draw clouds */
        for (i=0; i < NUM_CLOUD; i+=1)
        {
            /* Draw only object if within current screen view */
            if (onScreen(&game->cloud[i], game))
            {
                renderLoadedTexture(gt->cloud[game->cloud[i].animFrame],
                                    game->cloud[i].x + game->scrollX,
                                    game->cloud[i].y,
                                    0,
                                    game->cloud[i].facingLeft,
                                    game->renderer);
            }
        }

        /* Draw ground clutter */
        for (i=0; i < NUM_GROUNDCLUTTER; i+=1)
        {
            /* Draw only object if within current screen view */
            if (onScreen(&game->groundClutter[i], game))
            {
                renderLoadedTexture(gt->groundClutter[game->groundClutter[i].animFrame],
                                    game->groundClutter[i].x + game->scrollX,
                                    game->groundClutter[i].y+5,
                                    0,
                                    game->groundClutter[i].facingLeft,
                                    game->renderer);
            }
        }

        /* Draw obstacles */
        for (i=0; i < NUM_OBSTACLE; i+=1)
        {
            /* Draw only object if within current screen view */
            if (onScreen(&game->obstacle[i], game))
            {
                renderLoadedTexture(gt->obstacle[game->obstacle[i].animFrame],
                                    game->obstacle[i].x + game->scrollX,
                                    game->obstacle[i].y+5,
                                    0,
                                    game->obstacle[i].facingLeft,
                                    game->renderer);
            }
        }

        /* Draw platform */
        for (i=0; i < NUM_PLATFORM; i+=1)
        {
            /* Draw only object if within current screen view */
            if (onScreen(&game->platform[i], game))
            {
                renderLoadedTexture(gt->platform[game->platform[i].animFrame],
                                    game->platform[i].x + game->scrollX,
                                    game->platform[i].y,
                                    0,
                                    game->platform[i].facingLeft,
                                    game->renderer);
            }
        }

        /* Draw ground */
        for (i=0; i < NUM_GROUND; i+=1)
        {
            /* Draw only object if within current screen view */
            if (onScreen(&game->ground[i], game))
            {
                renderLoadedTexture(gt->ground[game->ground[i].animFrame],
                                    game->ground[i].x + game->scrollX,
                                    game->ground[i].y,
                                    0,
                                    game->ground[i].facingLeft,
                                    game->renderer);
            }
        }


        /* Draw enemies */
        for (i=0; i < NUM_ENEMY; i+=1)
        {
            /* Draw only object if within current screen view */
            if (onScreen(&game->enemy[i], game))
            {
                SDL_Texture *texture = NULL;

                if (game->enemy[i].aerial)
                    texture = gt->enemyFly[game->enemy[i].animFrame];
                else
                    texture = gt->enemySlime[game->enemy[i].animFrame];

                renderLoadedTexture(texture,
                                    game->enemy[i].x + game->scrollX,
                                    game->enemy[i].y+3,
                                    0,
                                    (game->enemy[i].dx >= 0),
                                    game->renderer);
            }
        }

        /* Draw player
        * facingLeft will flip the image if true */
        renderLoadedTexture(gt->player[game->player.animFrame],
                            game->player.x + game->scrollX,
                            game->player.y+2,
                            0,
                            game->player.facingLeft,
                            game->renderer);

        /* Draw health meter */
        SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
        rect.x = 20+40;
        rect.y = 20+6;
        rect.w = 102;
        rect.h = 20;
        SDL_RenderDrawRect(game->renderer, &rect);
        SDL_SetRenderDrawColor(game->renderer, 200, 50, 50, 255);
        rect.x = 21+40;
        rect.y = 21+6;
        rect.w = game->player.health;
        rect.h = 18;
        SDL_RenderFillRect(game->renderer, &rect);

        /* Draw player head */
        SDL_QueryTexture(gt->player[14], NULL, NULL, &w, &h);
        rect.x = 20;
        rect.y = 20;
        rect.w = w;
        rect.h = h;
        SDL_RenderCopyEx(game->renderer, gt->player[14],
                            NULL, &rect, 0, NULL, (game->player.facingLeft));

        /* Show current scroll */
        sprintf(text, "%04d", game->score+game->player.scoreValue);
        renderText(text, WIN_WIDTH-110, 20, colorWhite, 0,
                   game->font, game->renderer);

    }
    else if (game->state == GAME_STATE_OVER)
    {
        /* Background and clear */
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
        SDL_RenderClear(game->renderer);

        renderText("Game over", WIN_WIDTH/2, (WIN_HEIGHT/12)*2, colorWhite, 1,
                   game->font, game->renderer);

        sprintf(text, "Distance: %d", -game->scrollX);
        renderText(text, WIN_WIDTH/2, (WIN_HEIGHT/12)*5, colorWhite, 1,
                   game->font, game->renderer);

        sprintf(text, "Score: %d", game->score+game->player.scoreValue);
        renderText(text, WIN_WIDTH/2, (WIN_HEIGHT/12)*7, colorWhite, 1,
                   game->font, game->renderer);

        renderText("Press <Enter>", WIN_WIDTH/2, (WIN_HEIGHT/12)*10, colorWhite, 1,
                   game->font, game->renderer);
    }

    /* Render to screen */
    SDL_RenderPresent(game->renderer);

    /* Memory cleanup */
    gt = NULL;
}

/* Render text using the loaded TTF font */
void renderText(char *str, int x, int y, SDL_Color color, short center,
                TTF_Font *font, SDL_Renderer *renderer)
{
    /* Internal variables */
    SDL_Surface *surface = NULL;
    SDL_Texture *texture = NULL;
    int w, h;

    /* Load text into surface using supplied font, string
     * and color and then create the texture */
    surface = TTF_RenderText_Blended(font, str, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);

    /* Get the proper x,y,w,h */
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect rect = { 0,0,0,0 };
    rect.w = w;
    rect.h = h;
    /* Centralize position if wanted to */
    if (center)
    {
        rect.x = x-w/2;
        rect.y = y-h/2;
    }
    else
    {
        rect.x = x;
        rect.y = y;
    }

    /* Save onto renderer */
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    /* Free */
    SDL_DestroyTexture(texture);
    texture = NULL;
    SDL_FreeSurface(surface);
    surface = NULL;
}

void renderLoadedTexture(SDL_Texture *texture, int x, int y, short center,
                         short flip, SDL_Renderer *renderer)
{
    /* Get the proper x,y,w,h */
    int w, h;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    SDL_Rect rect = { 0,0,0,0 };
    rect.w = w;
    rect.h = h;
    /* Centralize position if wanted to.
     *
     * The +5 in y is to get rid of the ground bottom edges, and does
     * not interfere with visual collision, since everything is rendered +5 */
    if (center)
    {
        rect.x = x-w/2;
        rect.y = y-h/2+5;
    }
    else
    {
        rect.x = x;
        rect.y = y+5;
    }
    SDL_RenderCopyEx(renderer, texture, NULL, &rect,
                        0, NULL, flip);
}
