//
//  SpatialHash.c
//  Fluid
//
//  Created by Peyton Randolph on 30/07/2013.
//  Copyright (c) 2013 Peyton Randolph. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "SpatialHash.h"

#ifndef hMAX
#define hMAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef hMIN
#define hMIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

typedef struct {
    size_t i, j;
} Hash;

#pragma mark - Helpers

size_t clamp(int n, size_t min, size_t max)
{
    if (n < min)
        return min;
    if (n >= max)
        return max - 1;
    return n;
}

Hash spatialhash_hash(SpatialHash *shash, Vec2d pos)
{
    int i = floor(pos.x / shash->h), j = floor(pos.y / shash->h);
    i = clamp(i, 0, shash->width);
    j = clamp(j, 0, shash->height);
    
    return (Hash){.i = (size_t)i, .j = (size_t)j};
}

void spatialhash_free_map(SpatialHash *shash)
{
    for (size_t i = 0; i < shash->width; ++i)
    {
        for (size_t j = 0; j < shash->height; ++j)
        {
            list_free(shash->map[i][j]);
        }
        free(shash->map[i]);
    }
    free(shash->map);
}

void spatialhash_create_map(SpatialHash *shash)
{
    shash->map = (List **)malloc(sizeof(List *) * shash->width);
    for (size_t i = 0; i < shash->width; ++i)
    {
        shash->map[i] = (List *)calloc(shash->height, sizeof(List));
    }
}

#pragma mark - Interface

SpatialHash *spatialhash_create(Scalar maxX, Scalar maxY, Scalar h)
{
    SpatialHash *shash = (SpatialHash *)malloc(sizeof(SpatialHash));
    shash->maxX = maxX, shash->maxY = maxY, shash->h = h;
    
    size_t width = ceil(maxX / h);
    size_t height = ceil(maxY / h);
    shash->width = width;
    shash->height = height;
    
    spatialhash_create_map(shash);
    
    return shash;
}

void spatialhash_insert(SpatialHash *shash, Particle *p, size_t index)
{
    Hash hash = spatialhash_hash(shash, p->pos);
    size_t i = hash.i, j = hash.j;
    shash->map[i][j] = list_prepend(shash->map[i][j], index, 0, p);
}

void spatialhash_clear(SpatialHash *shash)
{
    for (size_t i = 0; i < shash->width; ++i)
    {
        for (size_t j = 0; j < shash->height; ++j)
        {
            list_free(shash->map[i][j]);
            shash->map[i][j] = NULL;
        }
    }
}

void spatialhash_free(SpatialHash *shash)
{
    spatialhash_free_map(shash);
    free(shash);
}

size_t spatialhash_create_neighbors(SpatialHash *shash, Particle *p, struct ListNode ***neighbors)
{
    size_t count = 0;
    size_t buffer_size = 64;
    
    struct ListNode **buffer = (struct ListNode **)malloc(sizeof(struct ListNode *) * buffer_size);
    
    Hash hash = spatialhash_hash(shash, p->pos);
    
    for (size_t i = hash.i > 0 ? hash.i - 1 : hash.i; i < hMIN(hash.i + 2, shash->width); ++i)
    {
        for (size_t j = hash.j > 0 ? hash.j - 1 : hash.j; j < hMIN(hash.j + 2, shash->height); ++j)
        {
            List particles = shash->map[i][j];
            while (particles)
            {
                if (particles->data == p)
                {
                    particles = particles->next;
                    continue;
                }
                
                if (count == buffer_size - 1)
                {
                    buffer_size *= 2;
                    buffer = (struct ListNode **)realloc(buffer, sizeof(struct ListNode *) * buffer_size);
                }
                
                buffer[count] = particles;
                ++count;
                
                particles = particles->next;
            }
        }
    }
    
    *neighbors = buffer;
    
    return count;
}
