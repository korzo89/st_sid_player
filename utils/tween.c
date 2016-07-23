/*
 * tween.c
 *
 *  Created on: 21.07.2016
 *      Author: Korzo
 */

#include "tween.h"
#include <assert.h>
#include <string.h>

//----------------------------------------------

void tween_init(struct tween *obj, const struct tween_params *params)
{
    ASSERT_WARN(obj != NULL);
    ASSERT_WARN(params != NULL);
    ASSERT_WARN(params->easing != NULL);

    memcpy(&obj->params, params, sizeof(*params));
}

//----------------------------------------------

float tween_start(struct tween *obj, uint32_t time)
{
    ASSERT_WARN(obj != NULL);

    obj->finished = false;
    obj->start_time = time;
    obj->value = obj->params.initial;

    return obj->value;
}

//----------------------------------------------

float tween_process(struct tween *obj, uint32_t time)
{
    ASSERT_WARN(obj != NULL);

    if (obj->finished)
        goto finish;

    if (time >= obj->start_time + obj->params.duration)
    {
        obj->value = obj->params.final;
        obj->finished = true;
        goto finish;
    }

    uint32_t dt = time - obj->start_time;
    float change = obj->params.final - obj->params.initial;
    obj->value = obj->params.easing(dt, obj->params.initial, change, obj->params.duration);

finish:
    return obj->value;
}

//----------------------------------------------

bool tween_is_finished(struct tween *obj)
{
    ASSERT_WARN(obj != NULL);
    return obj->finished;
}

//----------------------------------------------

float tween_easing_linear(float t, float b, float c, float d)
{
    return c * t / d + b;
}

float tween_easing_cubic_in(float t, float b, float c, float d)
{
    t /= d;
    return c * t * t * t + b;
}

float tween_easing_cubic_out(float t, float b, float c, float d)
{
    t = t / d - 1;
    return c * (t * t * t + 1) + b;
}

float tween_easing_cubic_in_out(float t, float b, float c, float d)
{
    t /= d;
    if ((t / 2) < 1)
        return c / 2 * t * t * t + b;
    t -= 2;
    return c / 2 * (t * t * t + 2) + b;
}
