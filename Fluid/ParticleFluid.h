//
//  ParticleFluid.h
//  Fluid
//
//  Created by Peyton Randolph on 7/25/13.
//  Copyright (c) 2013 Peyton Randolph. All rights reserved.
//

/*
 An implementation of "Particle-based Viscoelastic Fluid Simulation" by Clavet, Beaudoin, Poulin
 
 http://www.ligum.umontreal.ca/Clavet-2005-PVFS/pvfs.pdf
 */

#ifndef Fluid_ParticleFluid_h
#define Fluid_ParticleFluid_h

#include "LinkedList.h"
#include "Springs.h"
#include "Vector.h"

typedef float TimeInterval;

typedef struct {
    Vec2d pos, prev_pos, vel;
} Particle;

void update(Particle ps[], size_t count, TimeInterval dt, Springs springs);

#endif
