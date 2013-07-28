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
    Springs *springs = malloc(sizeof(Springs));
    springs->lengths = malloc(sizeof(Scalar) * count * count);
    springs->pairs = NULL;
    
    return springs;
}

void springs_free(Springs *springs)
{
    free(springs->lengths);
    list_free(springs->pairs);
    
    free(springs);
}
