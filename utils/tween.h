/*
 * tween.h
 *
 *  Created on: 21.07.2016
 *      Author: Korzo
 */

#ifndef UTILS_TWEEN_H_
#define UTILS_TWEEN_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

//----------------------------------------------

typedef float (*tween_easing_func_t)(float t, float b, float c, float d);

struct tween_params
{
    uint32_t duration;
    float initial;
    float final;
    tween_easing_func_t easing;
};

struct tween
{
    struct tween_params params;
    float value;
    uint32_t start_time;
    bool finished;
};

//----------------------------------------------

void tween_init(struct tween *obj, const struct tween_params *params);

float tween_start(struct tween *obj, uint32_t time);

float tween_process(struct tween *obj, uint32_t time);

bool tween_is_finished(struct tween *obj);

//----------------------------------------------

float tween_easing_linear(float t, float b, float c, float d);
float tween_easing_cubic_in(float t, float b, float c, float d);
float tween_easing_cubic_out(float t, float b, float c, float d);
float tween_easing_cubic_in_out(float t, float b, float c, float d);

//----------------------------------------------

#endif /* UTILS_TWEEN_H_ */
