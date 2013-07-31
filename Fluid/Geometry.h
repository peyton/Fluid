//
//  Geometry.h
//  Fluid
//
//  Created by Peyton Randolph on 30/07/2013.
//  Copyright (c) 2013 Peyton Randolph. All rights reserved.
//

#ifndef Fluid_Geometry_h
#define Fluid_Geometry_h

#include "Vector.h"

typedef Vec2d PointF;

typedef struct {
    Scalar width, height;
} SizeF;

typedef struct {
    PointF origin;
    SizeF size;
} RectF;

#endif
