//
//  Springs.c
//  Fluid
//
//  Created by Peyton Randolph on 7/25/13.
//  Copyright (c) 2013 Peyton Randolph. All rights reserved.
//

#include "Springs.h"

#include <stdlib.h>
#include <stdio.h>

Springs *springs_create(size_t count)
{
    Springs *springs = (Springs *)malloc(sizeof(Springs));
    springs->count = count;
    springs->pairs = NULL;
    
    springs->lengths = (Scalar **)malloc(sizeof(Scalar *) * count);
    for (size_t i = 0; i < springs->count; ++i)
        springs->lengths[i] = (Scalar *)malloc(sizeof(Scalar) * count);
    
    return springs;
}

void springs_free(Springs *springs)
{
    for (size_t i = 0; i < springs->count; ++i)
        free(springs->lengths[i]);
    free(springs->lengths);
    list_free(springs->pairs);
    
    free(springs);
}
