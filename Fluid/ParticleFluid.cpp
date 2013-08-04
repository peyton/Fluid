//
//  ParticleFluid.m
//  Fluid
//
//  Created by Peyton Randolph on 7/25/13.
//  Copyright (c) 2013 Peyton Randolph. All rights reserved.
//

#include "ParticleFluid.h"

#include <stdlib.h>
#include <stdio.h>

#include "SpatialHash.h"
#include "Util.h"

// Simulation
void adjust_springs(Particle ps[], size_t count, TimeInterval dt, Springs *springs, SpatialHash *shash, Scalar h, Scalar alpha, Scalar gamma, RectF bounds);
void apply_gravity(Particle ps[], size_t count, TimeInterval dt, Vec2d g);
void apply_spring_displacements(Particle ps[], size_t count, TimeInterval dt, Springs *springs, Scalar k_spring, Scalar h);
void apply_viscosity(Particle ps[], size_t count, TimeInterval dt, SpatialHash *shash, Scalar sigma, Scalar beta, Scalar h, RectF bounds);
void compute_velocity(Particle ps[], size_t count, TimeInterval dt);
void double_density_relaxation(Particle ps[], size_t count, TimeInterval dt, SpatialHash *shash, Scalar rho_0, Scalar k, Scalar k_near, Scalar h, RectF bounds);
void predict_positions(Particle ps[], size_t count, TimeInterval dt);
void resolve_collisions(Particle ps[], size_t count, TimeInterval dt, Scalar mu, Scalar restitution, RectF bounds);



#pragma mark - Simulation functions

void update(Particle ps[], size_t count, TimeInterval dt, Springs *springs, SpatialHash *shash, RectF bounds)
{
    const Vec2d g = {.x=0.1, .y=1000.};
    const Scalar alpha = 0.3;
    const Scalar sigma = 0.0;
    const Scalar beta = 0.1;
    const Scalar h = 20.;
    const Scalar gamma = 0.2;
    const Scalar rho_0 = 0.5;
    const Scalar mu = 0.5;
    const Scalar k = 2000;
    const Scalar k_near = 3000;
    const Scalar k_spring = 0.03;
    const Scalar restitution = 0.6;
//    const Scalar particle_radius = 0.2;
//    const Scalar particle_mass = 1.;
    apply_gravity(ps, count, dt, g);
    apply_viscosity(ps, count, dt, shash, sigma, beta, h, bounds);
    predict_positions(ps, count, dt);
    adjust_springs(ps, count, dt, springs, shash, h, alpha, gamma, bounds);
    apply_spring_displacements(ps, count, dt, springs, k_spring, h);
    double_density_relaxation(ps, count, dt, shash, rho_0, k, k_near, h, bounds);
    resolve_collisions(ps, count, dt, mu, restitution, bounds);
    compute_velocity(ps, count, dt);
}

#pragma mark - Steps

void adjust_springs(Particle ps[], size_t count, TimeInterval dt, Springs *springs, SpatialHash *shash, Scalar h, Scalar alpha, Scalar gamma, RectF bounds)
{
    spatialhash_clear(shash);
    
    for (size_t i = 0; i < count; ++i)
        spatialhash_insert(shash, ps + i, i);
    
    for (size_t i = 1; i < count; ++i)
    {
        struct ListNode **neighbors;
        size_t neighbors_count = spatialhash_create_neighbors(shash, ps + i, &neighbors);
        
        for (size_t j = 0; j < neighbors_count; j++) // Neighboring particles
        {
            size_t neighbor_index = neighbors[j]->i;
            if (i >= neighbor_index) continue;
            Particle *neighbor = (Particle *)neighbors[j]->data;
            
            Scalar q = vec_dist_2(ps[i].pos, neighbor->pos);
            if (q < h)
            {
                Scalar r = sqrt(q);
                q = r / h;
                Scalar L = springs->lengths[i][neighbor_index];
                if (L <= 0.)
                {
                    L = h;
                    springs->lengths[i][neighbor_index] = h;
                    springs->pairs = list_prepend(springs->pairs, i, neighbor_index, springs->lengths[i] + neighbor_index);
                }
                
                Scalar d = gamma * L;
                if (r > L + d) // stretch
                {
                    springs->lengths[i][neighbor_index] += dt * alpha * (r - L - d);
                } else if (r < L - d) // compress
                {
                    springs->lengths[i][neighbor_index] -= dt * alpha * (L - d - r);
                }
            }
        }
        free(neighbors);
    }

    struct ListNode *pair_node = springs->pairs;
    while (pair_node != NULL)
    {
        struct ListNode *next = pair_node->next;
        size_t i = pair_node->i; size_t j = pair_node->j;
        if (springs->lengths[i][j] > h)
        {
            springs->lengths[i][j] = 0.;
            springs->pairs = list_remove_node(pair_node, springs->pairs);
        }
        pair_node = next;
    }
}

void apply_gravity(Particle ps[], size_t count, TimeInterval dt, Vec2d g)
{
    g.x *= dt;
    g.y *= dt;
    for (size_t i = 0; i < count; ++i)
    {
        ps[i].vel.x += g.x;
        ps[i].vel.y += g.y;
    }
}

void apply_spring_displacements(Particle ps[], size_t count, TimeInterval dt, Springs *springs, Scalar k_spring, Scalar h)
{
    struct ListNode *pair_node = springs->pairs;
    while (pair_node != NULL)
    {
        size_t i = pair_node->i, j = pair_node->j;
        Scalar L = springs->lengths[i][j];
        Scalar r = vec_dist(ps[i].pos, ps[j].pos);
        Scalar a = dt * dt * k_spring * (1 - L / h) * (L - r);
        Vec2d r_hat = vec_unit(vec_sub(ps[j].pos, ps[i].pos));
        
        Vec2d D = vec_scale(r_hat, a * 0.5);
        ps[i].pos = vec_sub(ps[i].pos, D);
        ps[j].pos = vec_add(ps[j].pos, D);
        
        pair_node = pair_node->next;
    }
}

void apply_viscosity(Particle ps[], size_t count, TimeInterval dt, SpatialHash *shash, Scalar sigma, Scalar beta, Scalar h, RectF bounds)
{
    spatialhash_clear(shash);
    
    for (size_t i = 0; i < count; ++i)
        spatialhash_insert(shash, ps + i, i);
    
    for (size_t i = 1; i < count; ++i)
    {
        struct ListNode **neighbors;
        size_t neighbors_count = spatialhash_create_neighbors(shash, ps + i, &neighbors);
        
        for (size_t j = 0; j < neighbors_count; j++) // Neighboring particles
        {
            if (i >= neighbors[j]->i) continue;
            Particle *neighbor = (Particle *)neighbors[j]->data;
            
            Scalar q = vec_dist_2(ps[i].pos, neighbor->pos);
            if (q < h)
            {
                q = sqrt(q) / h;
                Vec2d r_hat = vec_unit(vec_sub(neighbor->pos, ps[i].pos));
                Scalar u = vec_dot(vec_sub(ps[i].vel, neighbor->vel), r_hat);
                if (u > 0. && u < 10000.)
                {
                    Scalar a = dt * (1. - q) * (sigma * u + beta * u * u);
                    Vec2d I_2 = vec_scale(r_hat, 0.5 * a);
                    ps[i].vel = vec_sub(ps[i].vel, I_2);
                    neighbor->vel = vec_add(neighbor->vel, I_2);
                }
            }
        }
        free(neighbors);
    }
}

void compute_velocity(Particle ps[], size_t count, TimeInterval dt)
{
    for (size_t i = 0; i < count; ++i)
    {
        ps[i].vel = vec_scale(vec_sub(ps[i].pos, ps[i].prev_pos), 1./dt);
    }
}


void double_density_relaxation(Particle ps[], size_t count, TimeInterval dt, SpatialHash *shash, Scalar rho_0, Scalar k, Scalar k_near, Scalar h, RectF bounds)
{
    spatialhash_clear(shash);
    
    for (size_t i = 0; i < count; ++i)
        spatialhash_insert(shash, ps + i, i);
    
    for (size_t i = 0; i < count; ++i)
    {
        Scalar rho = 0.;
        Scalar rho_near = 0.;
        
        struct ListNode **neighbors;
        size_t neighbors_count = spatialhash_create_neighbors(shash, ps + i, &neighbors);
        
        for (size_t j = 0; j < neighbors_count; j++) // Neighboring particles
        {
            if (neighbors[j]->i == i) continue;
            Particle *neighbor = (Particle *)neighbors[j]->data;
            Scalar q = vec_dist_2(ps[i].pos, neighbor->pos);
            if (q < h)
            {
                q = sqrt(q) / h;
                rho += (1. - q) * (1. - q);
                rho_near += (1. - q) * (1. - q) * (1. - q);
            }
        }
        Scalar P = k * (rho - rho_0);
        Scalar P_near = k_near * rho_near;
        Vec2d dx = {.x = 0., .y = 0.};
        for (size_t j = 0; j < neighbors_count; j++) // Neighboring particles
        {
            if (neighbors[j]->i == i) continue;
            Particle *neighbor = (Particle *)neighbors[j]->data;
            Scalar q = vec_dist_2(ps[i].pos, neighbor->pos);
            if (q < h)
            {
                q = sqrt(q) / h;
                Vec2d r_hat = vec_unit(vec_sub(neighbor->pos, ps[i].pos));
                Scalar a = dt * dt * (P * (1. - q) + P_near * (1. - q) * (1. - q));
                Vec2d D = vec_scale(r_hat, a * 0.5);
                neighbor->pos = vec_add(neighbor->pos, D);
                dx = vec_sub(dx, D);
            }
        }
        
        free(neighbors);
        
        ps[i].pos = vec_add(ps[i].pos, dx);
    }
}

void predict_positions(Particle ps[], size_t count, TimeInterval dt)
{
    for (size_t i = 0; i < count; ++i)
    {
        ps[i].prev_pos = ps[i].pos;
        ps[i].pos = vec_add(ps[i].pos, vec_scale(ps[i].vel, dt));
    }
}

Scalar clampBetween(Scalar val, Scalar min, Scalar max)
{
    if (val < min)
        return min + FLT_EPSILON;
    else if (val > max)
        return max - FLT_EPSILON;
    return val;
}

void resolve_collisions(Particle ps[], size_t count, TimeInterval dt, Scalar mu, Scalar restitution, RectF bounds)
{
    const Scalar minX = bounds.origin.x;
    const Scalar maxX = bounds.origin.x + bounds.size.width;
    const Scalar minY = bounds.origin.y;
    const Scalar maxY = bounds.origin.y + bounds.size.height;
    for (size_t i = 0; i < count; ++i)
    {
        Vec2d pos = ps[i].pos;
        Vec2d norm;
        
        if (pos.x < minX)
        {
            norm = {.x = 1., .y = 0.};
        } else if (pos.x > maxX)
        {
            norm = {.x = -1., .y = 0.};
        } else if (pos.y < minY)
        {
            norm = {.x = 0., .y = 1.};
        } else if (pos.y > maxY)
        {
            norm = {.x = 0., .y = -1.};
        } else {
            continue;
        }
        
        ps[i].pos.x = clampBetween(ps[i].pos.x, minX + FLT_MIN, maxX - FLT_MIN);
        ps[i].pos.y = clampBetween(ps[i].pos.y, minY + FLT_MIN, maxY - FLT_MIN);
        
        Vec2d v_normal = vec_scale(norm, vec_dot(ps[i].vel, norm));
        Vec2d v_tangent = vec_sub(ps[i].vel, v_normal);
        Vec2d I = vec_sub(v_normal, vec_scale(v_tangent, mu));
        I.y = -I.y;
        ps[i].vel = I;
    }
}


