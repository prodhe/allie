/*
 *  collision.c
 */

#include "main.h"
#include "collision.h"

/* Quick 2D collision check to see if two rectangles collide */
float collide2d(float ax, float ay, float aw, float ah,
                float bx, float by, float bw, float bh)
{
    return (!((ay+ah) < by || ay > (by+bh) || ax > (bx+bw) || (ax+aw) < bx));
}

/* Check for collision against an array of solid objects
 * Return 1 on vertical collision, 2 on horizontal and 0 otherwise */
int collision(game_t *game, Object *obj, Object targets[], int n,
              short checkCliff, short debug)
{
    int collisionDetected = 0;
    int crashLeft = 0;
    int crashRight = 0;
    Object *target = NULL;

    for (int i=0; i < n; i+=1)
    {
        target = &targets[i];

        /* Skip this target if it's not near screen or if it's checking
         * against itself */
        if (!nearScreen(target, game) || obj == target)
            continue;

        /* Check collision only if both objects are solid */
        if (obj->solid && target->solid)
        {
            if (collisionDetected == 0 && (obj->dx != 0 || target->dx != 0))
            {
                if (horizontalCollision(obj, target, game->time, debug))
                    collisionDetected += 1;
            }

            if (collisionDetected == 0 && (obj->dy != 0 || target->dy != 0))
            {
                if (verticalCollision(obj, target, game->time, debug))
                    collisionDetected += 1;
            }

            /* Cliff check */
            if (checkCliff && obj->onPlatform && !obj->aerial)
            {
                switch (isObjectCrashing(obj, target))
                {
                    case 1:
                        crashLeft += 1;
                        break;
                    case 2:
                        crashRight += 1;
                        break;
                    case 3:
                        crashLeft += 1;
                        crashRight += 1;
                        break;
                }
            }

            /* No vertical or horizontal collision */
            if (collisionDetected == 0)
            {
                /* We're falling or jumping without hitting anything */
                if (obj->dy != 0)
                    obj->onPlatform = 0;
            }

        }

        /* Reset target pointer for next run */
        target = NULL;
    }

    /* Check if object can crash against anything, either left or right */
    if (checkCliff && !obj->aerial)
    {
        /* Crash to the left only, means we will fall off to the right */
        if (crashLeft && !crashRight)
        {
            /* Reposition to the left */
            obj->x -= 1;
            obj->dx = 0;
        }
        else if (!crashLeft && crashRight)
        {
            /* Reposition to the right */
            obj->x += 1;
            obj->dx = 0;
        }
    }

    /* Return possible collision */
    return collisionDetected;
}

/* Vertical collision */
int verticalCollision(Object* obj, Object *target, unsigned int gameTime, short debug)
{
    int detected = 0;

    /* Shorthand variable names... */ 
    float ox = obj->x; 
    float oy = obj->y; 
    int ow = obj->w; 
    int oh = obj->h; 
    float tx = target->x; 
    float ty = target->y; 
    int tw = target->w; 
    int th = target->h; 

    /* vertical collision */
    if (collide2d(tx, ty, tw, th, ox, oy+obj->dy, ow, oh))
    {
        /* We've collided with an object and taken action */
        detected = 1;

        /* If no vertical movement except for 1 round of gravity */
        if (obj->dy == GRAVITY)
        {
            /* gravity is applied after movement control in general
                * processing, so if we clear it now, no y-movement will
                * happen */
            obj->dy = 0;
            obj->onPlatform = 1;
        }

        /* If target is above (bouncing object's head) */
        if (oy > ty+th && gameTime-(obj->timeHit) > 30)
        {
            /* if (debug) printf("%u: i'm below\n", gameTime); */
            if (debug) dprint("i'm below", gameTime);
            
            /* Force y-position and stop dy */
            obj->y = ty+th+GRAVITY;
            obj->dy = 0;

            /* If target does damage and object is weak, hurt the object */
            if (target->damage &&
                (target->damageSides & BOTTOM) &&
                (obj->fragileSides & TOP))
            {
                if (debug) dprint("hit in the head", gameTime);
                hurtObject(obj, target, gameTime);
            }
        }
        
        /* if target is below */
        else if (oy+oh < ty)
        {
            if (debug) dprint("i'm above", gameTime);

            /* Force y-position and stop dy */
            obj->y = ty-oh-GRAVITY;
            obj->dy = 0;

            /* Start by checking if target does damage and if it doesn't and
             * if object does damage, hurt the target (if it's not
             * already dead */
            if (target->damage > 0 && obj->health > 0 &&
                (target->damageSides & TOP) &&
                (obj->fragileSides & BOTTOM))
            {
                if (debug) dprint("hit from below", gameTime);
                hurtObject(obj, target, gameTime);
            }
            /* object hurting target */
            else if (obj->damage > 0 && target->health > 0 &&
                (target->fragileSides & TOP) &&
                (obj->damageSides & BOTTOM))
            {
                if (debug) dprint("I killed it", gameTime);
                hurtObject(target, obj, gameTime);
            }
            /* Otherwise we stand unharmed on a solid platform */
            else
                obj->onPlatform = 1;
        }
    }

    return detected;
}

/* Horizontal collision */
int horizontalCollision(Object* obj, Object *target, unsigned int gameTime, short debug)
{
    int detected = 0;

    /* Shorthand variable names... */ 
    float ox = obj->x; 
    float oy = obj->y; 
    int ow = obj->w; 
    int oh = obj->h; 
    float tx = target->x; 
    float ty = target->y; 
    int tw = target->w; 
    int th = target->h; 

    /* horizontal collision */
    if (collide2d(tx, ty, tw, th, ox+obj->dx, oy, ow, oh))
    {
        /* We've collided with an object and has yet to see some battle */
        detected = 1;
        int battle = 0;

        /* Start off by stopping the movement and see what happens next */
        obj->dx = 0;

        /* Object is left of target */
        if (obj->x <= target->x)
        {
            if (debug) dprint("i'm on the left", gameTime);

            /* Does target do damage and is object allowed to get hit */
            if (!battle && target->damage > 0 && (gameTime)-(obj->timeHit) > 30)
            {
                /* Does target to damage to this side and is object fragile */
                if ((target->damageSides & BACK) &&
                (obj->fragileSides & FRONT))
                {
                    if (debug) dprint("target hit obj's right", gameTime);
                    hurtObject(obj, target, gameTime);
                    battle = 1;
                }
            }
            /* Reverse: does object do damage and is target vulnerable */
            if (!battle && obj->damage > 0 && (gameTime)-(target->timeHit) > 30)
            {
                if ((obj->damageSides & (FRONT)) &&
                (target->fragileSides & (BACK)))
                {
                    if (debug) dprint("obj hit target's left", gameTime);
                    hurtObject(target, obj, gameTime);
                    battle = 1;
                }
            }
        }
        /* Object is on the right */
        else if (obj->x > target->x)
        {
            if (debug) dprint("obj on the right", gameTime);
            /* Does target do damage and is object allowed to get hit */
            if (!battle && target->damage > 0 && (gameTime)-(obj->timeHit) > 30)
            {
                /* Does target to damage to this side and is object fragile */
                if ((target->damageSides & FRONT) &&
                (obj->fragileSides & BACK))
                {
                    if (debug) dprint("target hit obj's left", gameTime);
                    hurtObject(obj, target, gameTime);
                    battle = 1;
                }
            }
            /* Reverse: does object do damage and is target vulnerable */
            if (!battle && obj->damage > 0 && (gameTime)-(target->timeHit) > 30)
            {
                if ((obj->damageSides & FRONT) &&
                (target->fragileSides & BACK))
                {
                    if (debug) dprint("obj hit target's right", gameTime);
                    hurtObject(target, obj, gameTime);
                    battle = 1;
                }
            }
        }

        /* If we're still standing still, then no battle has been fought. But
         * maybe we're getting pushed by the target? Let's move in the same
         * direction */
        if (!battle && target->dx != 0)
            obj->dx += target->dx;
    }

    return detected;
}

/* Check if object would fall on target, if it were to move
 * sideways equivalent to it's own width.
 * Returns 1 for left, 2 for right (3 for both) and 0 for none */
int isObjectCrashing(Object *o, Object *t)
{
    int crash = 0;

    /* If object is below target, just stop here */
    if (o->y > t->y)
        return 0;

    /* Will we collide if object move left? */
    if (collide2d(t->x, t->y, t->w, t->h,
                   (o->x - o->w), (o->y + GRAVITY), o->w, o->h))
        crash += 1;

    /* Right? */
    if (collide2d(t->x, t->y, t->w, t->h,
                       (o->x + o->w), (o->y + GRAVITY), o->w, o->h))
        crash += 2;

    /* Return */
    return crash;
}

/* Damage an object with the strength of another object */
void hurtObject(Object *reciever, Object *dealer, unsigned int gameTime)
{
    /* Drop health */
    reciever->health -= dealer->damage;

    /* Save the hit, so we can control how long it
    * takes to get hit again */
    reciever->timeHit = gameTime;

    /* Make the dealer bounce a little if not on a platform */
    if (!dealer->onPlatform)
        dealer->dy = -3;

    /* If reciever dies */
    if (reciever->health <= 0)
    {
        /* Add the score */
        dealer->scoreValue += reciever->scoreValue;

        /* Zeroize */
        reciever->health = 0;
        reciever->solid = 0;

        /* Bounce up and stop x movement */
        reciever->dy = -2;
        reciever->dx = 0;
    }
    /* If reciever is still alive after the hit */
    else
    {
        /* reverse direction (bounce back) */
        if (reciever->dx < 0)
            reciever->dx = 8;
        else
            reciever->dx = -8;

        /* always bounce up a little if hit and on a platform */
        if (reciever->onPlatform)
            reciever->dy = -3;
    }
}

/* EOF */
