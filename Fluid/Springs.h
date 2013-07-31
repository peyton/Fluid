//
//  Springs.h
//  Fluid
//
//  Created by Peyton Randolph on 7/25/13.
//  Copyright (c) 2013 Peyton Randolph. All rights reserved.
//

#ifndef Fluid_Springs_h
#define Fluid_Springs_h

#include "LinkedList.h"
#include "Vector.h"

typedef struct {
    Scalar **lengths;
    size_t count;
    List pairs;
} Springs;

Springs *springs_create(size_t count);
void springs_free(Springs *springs);

#endif
