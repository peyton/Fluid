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

typedef float Scalar;

typedef struct {
    Scalar x, y;
} Vec2d;

Vec2d vec_add(Vec2d vec1, Vec2d vec2);
Vec2d vec_sub(Vec2d vec1, Vec2d vec2);
Vec2d vec_scale(Vec2d vec, Scalar a);
Scalar vec_dot(Vec2d vec1, Vec2d vec2);
Vec2d vec_unit(Vec2d vec);
Scalar vec_norm_2(Vec2d vec);
Scalar vec_norm(Vec2d vec);
Scalar vec_dist(Vec2d vec1, Vec2d vec2);

#endif
