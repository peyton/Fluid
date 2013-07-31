//
//  Vector.h
//  Fluid
//
//  Created by Peyton Randolph on 7/25/13.
//  Copyright (c) 2013 Peyton Randolph. All rights reserved.
//

#ifndef Fluid_Vector_h
#define Fluid_Vector_h

#include <stddef.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>

typedef float Scalar;

typedef struct {
    Scalar x, y;
} Vec2d;

inline Vec2d vec_add(Vec2d vec1, Vec2d vec2);
inline Vec2d vec_sub(Vec2d vec1, Vec2d vec2);
inline Vec2d vec_scale(Vec2d vec, Scalar a);
inline Scalar vec_dot(Vec2d vec1, Vec2d vec2);
inline Vec2d vec_unit(Vec2d vec);
inline Scalar vec_norm_2(Vec2d vec);
inline Scalar vec_norm(Vec2d vec);
inline Scalar vec_dist(Vec2d vec1, Vec2d vec2);
inline Scalar vec_dist_2(Vec2d vec1, Vec2d vec2);

inline Vec2d vec_add(Vec2d vec1, Vec2d vec2)
{
    vec1.x += vec2.x;
    vec1.y += vec2.y;
    return vec1;
}

inline Vec2d vec_sub(Vec2d vec1, Vec2d vec2)
{
    return vec_add(vec1, vec_scale(vec2, -1.));
}

inline Vec2d vec_scale(Vec2d vec, Scalar a)
{
    vec.x *= a;
    vec.y *= a;
    return vec;
}

inline Vec2d vec_unit(Vec2d vec)
{
    Scalar norm = vec_norm(vec);
    norm = fmax(norm, FLT_MIN);
    return vec_scale(vec, 1. / norm);
}

inline Scalar vec_dot(Vec2d vec1, Vec2d vec2)
{
    return vec1.x * vec2.x + vec1.y * vec2.y;
}

inline Scalar vec_norm_2(Vec2d vec)
{
    return vec_dot(vec, vec);
}

inline Scalar vec_norm(Vec2d vec)
{
    return sqrt(vec_norm_2(vec));
}

inline Scalar vec_dist(Vec2d vec1, Vec2d vec2)
{
    return sqrt(vec_dist_2(vec1, vec2));
}

inline Scalar vec_dist_2(Vec2d vec1, Vec2d vec2)
{
    return vec_norm_2(vec_sub(vec1, vec2));
}

#endif
