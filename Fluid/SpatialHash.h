//
//  SpatialHash.h
//  Fluid
//
//  Created by Peyton Randolph on 30/07/2013.
//  Copyright (c) 2013 Peyton Randolph. All rights reserved.
//

#ifndef Fluid_SpatialHash_h
#define Fluid_SpatialHash_h

#include <stddef.h>
#include "Common.h"
#include "LinkedList.h"

typedef struct {
    size_t width, height;
    Scalar maxX, maxY, h;
    List **map;
} SpatialHash;

SpatialHash *spatialhash_create(Scalar maxX, Scalar maxY, Scalar h);
void spatialhash_free(SpatialHash *shash);

void spatialhash_insert(SpatialHash *shash, Particle *p, size_t index);
void spatialhash_clear(SpatialHash *shash);

size_t spatialhash_create_neighbors(SpatialHash *shash, Particle *p, struct ListNode ***neighbors);

#endif
