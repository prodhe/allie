/*
 * init.c
 */

#include "init.h"
#include "main.h"

/* Initialization of game state and automatic level creation */
void initGame(game_t *game)
{
    /* if (!Mix_PlayingMusic()) */
    /* Play music (channel 1 and 3rd argument -1 loops forever */
    if (!Mix_Playing(1) && game->soundMusic != NULL)
    {
        /* Mix_PlayMusic(game->soundMusic, -1); */
        Mix_PlayChannel(1, game->soundMusic, -1);
    }

    /* game_t init */
    game->time = 0;
    game->scrollX = 0;
    game->score = 0;
    game->state = GAME_STATE_START;
    game->pause = 0;

    /* Player init */
    initObject(&game->player);
    game->player.x = WIN_WIDTH/6;
    game->player.y = -100;
    /* game->player.w = 75; */
    /* game->player.h = 96; */
    game->player.w = 66; /* smaller than ground, to be able to fit between 2 blocks */
    game->player.h = 96;
    game->player.solid = 1;
    game->player.gravityBound = 1;
    game->player.aerial = 1;
    game->player.facingLeft = 0;
    game->player.health = 100;
    game->player.damage = 10;
    game->player.damageSides = 0;
    game->player.fragileSides = 0;
    game->player.speed = 8;

    /* Placeholder for current object */
    Object *obj = NULL;

    /* Placeholder for width and height of queried texture */
    int w = 0;
    int h = 0;

    /* For loops */
    int i = 0;

    /* Ground init */
    int groundLevel = 1;
    int makeWater = 0;
    for (i=0; i < NUM_GROUND; i+=1)
    {
        obj = &game->ground[i];
        initObject(obj);
        obj->textures = game->textures.ground;

        /* Random chance for making water */
        if (!makeWater && random()%12 == 0 && i > (WIN_WIDTH/2)/70)
        {
            makeWater = i;
        }

        /* Choose between ground and water */
        if (makeWater)
        {
            obj->animFrame = 5;
            obj->solid = 0;
            /* groundLevel = 1; /1* force lowest ground level *1/ */
        }
        else
        {
            obj->animFrame = 0;
            obj->solid = 1;
        }

        /* Get properties of chosen texture */
        SDL_QueryTexture(obj->textures[obj->animFrame], NULL, NULL, &w, &h);

        obj->x = i*w;
        obj->y = WIN_HEIGHT - (h*groundLevel);
        obj->w = w;
        obj->h = h;

        /* Alternate between first and second row of ground */
        /* groundLevel = (groundLevel) ? 0 : 1; */

        /* Stop making water after a minimum of x+2 */
        if (i-makeWater > 1)
            makeWater = 0;


        obj = NULL;
    }

    /* Platform init */
    int platformLevel = 1;
    int previousPlatformLevel = 1;
    int lastPlatformChange = 0;
    int skippedBlock = 0;
    int newX = 0;
    for (i=0; i < NUM_PLATFORM; i+=1)
    {
        obj = &game->platform[i];
        initObject(obj);
        obj->textures = game->textures.platform;

        /* Change platform level after 2-6 blocks (the +1 does that) */
        if (i-lastPlatformChange > (random()%5)+1)
        {
            /* Randomly choose new level height, but only if it's max 3 levels
             * away from previous. Otherwise, the player can't reach it. Maybe
             * he shouldn't be able to? */
            while (abs((platformLevel = (random()%4)) - previousPlatformLevel) > 2);

            /* Randomly choose new skin for the platform, if it's a new level */
            if (platformLevel != previousPlatformLevel)
                obj->animFrame = (random()%2);
            else
                obj->animFrame = (obj-1)->animFrame;

            /* Save values to check against during next loop */
            previousPlatformLevel = platformLevel;
            lastPlatformChange = i;

            /* Always leave a hole of at least +x blocks and then
             * randomly make it bigger */
            skippedBlock += random()%3 + 1; /* size of hole */
        }
        else
        {
            /* Reset the hole */
            skippedBlock = 0;

            /* Use the same texture as the last object and zero check
             * for the first run to not get array out of bounds */
            if (i != 0)
                obj->animFrame = (obj-1)->animFrame;
                /* obj->animFrame = game->platform[i-1].animFrame; */
        }

        /* Get width and height of platform texture */
        SDL_QueryTexture(obj->textures[obj->animFrame], NULL, NULL, &w, &h);

        /* Set the proper X, since the platforms vary in length */
        if (newX > 0)
            obj->x = newX + (w*skippedBlock);
        else
            obj->x = WIN_WIDTH/2 + (w*skippedBlock);
        newX = obj->x+w;

        obj->y = WIN_HEIGHT-game->ground[0].h*(platformLevel+3);
        obj->w = w;
        obj->h = h;
        obj->solid = 1;
        obj->aerial = 1;

        obj = NULL;
    }

    /* Ground clutter init */
    for (i=0; i < NUM_GROUNDCLUTTER; i+=1)
    {
        obj = &game->groundClutter[i];
        initObject(obj);
        obj->textures = game->textures.groundClutter;

        /* Choose a random image and get the width and height */
        obj->animFrame = random()%3;
        SDL_QueryTexture(obj->textures[obj->animFrame], NULL, NULL, &w, &h);

        obj->x = i*400 + 150*(random()%5);
        obj->y = WIN_HEIGHT-game->ground[0].h-h;
        obj->w = w;
        obj->h = h;
        obj->solid = 1;
        obj->gravityBound = 1;

        obj = NULL;
    }

    /* Obstacles init */
    for (i=0; i < NUM_OBSTACLE; i+=1)
    {
        obj = &game->obstacle[i];
        initObject(obj);
        obj->textures = game->textures.obstacle;

        /* Choose a random image/obstacle and get the width and height */
        obj->animFrame = random()%4;
        SDL_QueryTexture(obj->textures[obj->animFrame], NULL, NULL, &w, &h);

        obj->x = WIN_WIDTH/2 + i*421 + 153*(random()%5);

        /* if obstacle is supposed to be on ground (hills: 0,1),
         * make it not fall all the way from the sky */
        if (obj->animFrame < 2)
            obj->y = WIN_HEIGHT-game->ground[0].h-h;
        else
            obj->y = 0-((random()%2)+1)*(h/2);

        /* If a dangerous object (eg spikes), make it do damage */
        if (obj->animFrame == 3)
        {
            obj->damage = 25;
            obj->damageSides = TOP;
        }

        obj->w = w;
        obj->h = h;
        obj->solid = 1;
        obj->gravityBound = 1;

        obj = NULL;
    }

    /* Cloud init */
    for (i=0; i < NUM_CLOUD; i+=1)
    {
        obj = &game->cloud[i];
        initObject(obj);
        obj->textures = game->textures.cloud;

        /* Choose a random image and get the width and height */
        obj->animFrame = random()%3;
        SDL_QueryTexture(obj->textures[obj->animFrame], NULL, NULL, &w, &h);

        obj->x = i*300 + 100*(random()%3);
        obj->y = 0+(5*(random()%30));
        obj->w = w;
        obj->h = h;
        obj->aerial = 1;

        obj = NULL;
    }

    /* Enemy init */
    for (i=0; i < NUM_ENEMY; i+=1)
    {
        obj = &game->enemy[i];
        initObject(obj);

        /* Randomize aerial creature or ground dweller */
        obj->aerial = (random()%3)?0:1;

        /* Initialize the rest based on whether it's flying or not */
        if (!obj->aerial)
        {
            /* Ground enemies */
            obj->textures = game->textures.enemySlime;

            /* Get the width and height */
            SDL_QueryTexture(obj->textures[obj->animFrame], NULL, NULL, &w, &h);

            obj->speed = 1;
            obj->y = -WIN_HEIGHT/2;
            obj->dx = ((random()%2)?1:-1)*(obj->speed);
            obj->damage = 10;
            obj->scoreValue = 10;
        }
        else
        {
            /* Aerial enemies */
            obj->textures = game->textures.enemyFly;

            /* Get the width and height */
            SDL_QueryTexture(obj->textures[obj->animFrame], NULL, NULL, &w, &h);

            obj->speed = 2;
            obj->y = 20+(random()%(WIN_HEIGHT-WIN_HEIGHT/4));
            obj->dx = ((random()%2)?1:-1)*(obj->speed);
            obj->damage = 20;
            obj->scoreValue = 15;
        }

        /* Settings that is the same for all enemies */
        obj->x = WIN_WIDTH-WIN_WIDTH/4 + i*WIN_WIDTH/2;
        obj->w = w;
        obj->h = h;
        obj->gravityBound = 1;
        obj->solid = 1;
        obj->health = 1;
        obj->damageSides = FRONT | BACK | BOTTOM;
        obj->fragileSides = TOP;

        obj = NULL;
    }
}

/* Basic zero-initialization of game object */
void initObject(Object *obj)
{
    obj->x = 0;
    obj->y = 0;
    obj->dx = 0;
    obj->dy = 0;
    obj->w = 0;
    obj->h = 0;
    obj->solid = 0;
    obj->aerial = 0;
    obj->gravityBound = 0;
    obj->animFrame = 0;
    obj->onPlatform = 0;
    obj->facingLeft = (random()%2)?1:0;
    obj->health = 0;
    obj->damage = 0;
    obj->speed = 0;
    obj->timeHit = 0;
    obj->scoreValue = 0;
    obj->textures = NULL;
    obj->damageSides = 0;
    obj->fragileSides = 0;
}

/* EOF */
