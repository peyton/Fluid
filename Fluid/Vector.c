//
//  Vector.c
//  Fluid
//
//  Created by Peyton Randolph on 7/25/13.
//  Copyright (c) 2013 Peyton Randolph. All rights reserved.
//

#include "Vector.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

Vec2d vec_add(Vec2d vec1, Vec2d vec2)
{
    vec1.x += vec2.x;
    vec1.y += vec2.y;
    return vec1;
}

Vec2d vec_sub(Vec2d vec1, Vec2d vec2)
{
    return vec_add(vec1, vec_scale(vec2, -1.));
}

Vec2d vec_scale(Vec2d vec, Scalar a)
{
    vec.x *= a;
    vec.y *= a;
    return vec;
}

Vec2d vec_unit(Vec2d vec)
{
    return vec_scale(vec, 1. / vec_norm(vec));
}

Scalar vec_dot(Vec2d vec1, Vec2d vec2)
{
    return vec1.x * vec2.x + vec1.y * vec2.y;
}

Scalar vec_norm_2(Vec2d vec)
{
    return vec_dot(vec, vec);
}

Scalar vec_norm(Vec2d vec)
{
    return sqrt(vec_norm_2(vec));
}

Scalar vec_dist(Vec2d vec1, Vec2d vec2)
{
    return vec_norm(vec_sub(vec1, vec2));
}