//
//  Common.h
//  Fluid
//
//  Created by Peyton Randolph on 29/07/2013.
//  Copyright (c) 2013 Peyton Randolph. All rights reserved.
//

#ifndef Fluid_Common_h
#define Fluid_Common_h

#include "Geometry.h"
#include "Springs.h"
#include "Vector.h"

typedef float TimeInterval;

typedef struct {
    Vec2d pos, prev_pos, vel;
} Particle;

#endif
