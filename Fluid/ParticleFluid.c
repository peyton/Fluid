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

// Simulation
void adjust_springs(Particle ps[], size_t count, TimeInterval dt, Springs *springs, Scalar h, Scalar alpha, Scalar gamma);
void apply_gravity(Particle ps[], size_t count, TimeInterval dt, Vec2d g);
void apply_spring_displacements(Particle ps[], size_t count, TimeInterval dt, Springs *springs, Scalar k_spring, Scalar h);
void apply_viscosity(Particle ps[], size_t count, TimeInterval dt, Scalar sigma, Scalar beta, Scalar h);
void compute_velocity(Particle ps[], size_t count, TimeInterval dt);
void double_density_relaxation(Particle ps[], size_t count, TimeInterval dt, Scalar rho_0, Scalar k, Scalar k_near, Scalar h);
void predict_positions(Particle ps[], size_t count, TimeInterval dt);
void resolve_collisions(Particle ps[], size_t count, TimeInterval dt);

#pragma mark - Simulation functions

void update(Particle ps[], size_t count, TimeInterval dt, Springs springs)
{
    const Vec2d g = {.x=0., .y=100.};
    const Scalar alpha = 0.1;
    const Scalar sigma = 0.;
    const Scalar beta = 0.01;
    const Scalar h = 10.;
    const Scalar gamma = 0.2;
    const Scalar rho_0 = 0.;
    const Scalar k = 0.;
    const Scalar k_near = 0.;
    const Scalar k_spring = 0;
    
    apply_gravity(ps, count, dt, g);
    apply_viscosity(ps, count, dt, sigma, beta, h);
    predict_positions(ps, count, dt);
    adjust_springs(ps, count, dt, &springs, h, alpha, gamma);
    apply_spring_displacements(ps, count, dt, &springs, k_spring, h);
    double_density_relaxation(ps, count, dt, rho_0, k, k_near, h);
    resolve_collisions(ps, count, dt);
    compute_velocity(ps, count, dt);
}

#pragma mark - Steps

void adjust_springs(Particle ps[], size_t count, TimeInterval dt, Springs *springs, Scalar h, Scalar alpha, Scalar gamma)
{
    for (size_t i = 1; i < count; ++i)
    {
        for (size_t j = 0; i < j; ++i)
        {
            Scalar r = vec_dist(ps[i].pos, ps[j].pos);
            Scalar q = r / h;
            if (q < 1.)
            {
                Scalar L = springs->lengths[i][j];
                if (L <= 0.)
                {
                    L = h;
                    springs->lengths[i][j] = h;
                    springs->pairs = list_prepend(springs->pairs, i, j);
                }
                
                Scalar d = gamma * L;
                if (r > L + d) // stretch
                {
                    springs->lengths[i][j] += dt * alpha * (r - L - d);
                } else if (r < L - d) // compress
                {
                    springs->lengths[i][j] -= dt * alpha * (L - d - r);
                }
            }
        }
    }
    struct ListNode *pair_node = springs->pairs;
    while (pair_node != NULL)
    {
        struct ListNode *next = pair_node->next;
        if (springs->lengths[pair_node->i][pair_node->j] > h)
        {
            springs->lengths[pair_node->i][pair_node->j] = 0.;
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
        Vec2d r_hat = vec_unit(vec_sub(ps[i].pos, ps[j].pos));
        
        Vec2d D = vec_scale(r_hat, a * 0.5);
        ps[i].pos = vec_sub(ps[i].pos, D);
        ps[j].pos = vec_add(ps[j].pos, D);
        
        pair_node = pair_node->next;
    }
}

void apply_viscosity(Particle ps[], size_t count, TimeInterval dt, Scalar sigma, Scalar beta, Scalar h)
{
    for (size_t i = 1; i < count; ++i)
    {
        for (size_t j = 0; i < j; ++i)
        {
            Scalar q = vec_dist(ps[i].pos, ps[j].pos) / h;
            if (q < 1.)
            {
                Vec2d r_hat = vec_unit(vec_sub(ps[i].pos, ps[j].pos));
                Scalar u = vec_dot(vec_sub(ps[i].vel, ps[j].vel), r_hat);
                if (u > 0.)
                {
                    Scalar a = dt * (1. - q) * (sigma * u + beta * u * u);
                    Vec2d I_2 = vec_scale(r_hat, 0.5 * a);
                    ps[i].vel = vec_sub(ps[i].vel, I_2);
                    ps[j].vel = vec_add(ps[j].vel, I_2);
                }
            }
        }
    }
}

void compute_velocity(Particle ps[], size_t count, TimeInterval dt)
{
    for (size_t i = 0; i < count; ++i)
    {
        ps[i].vel = vec_scale(vec_sub(ps[i].pos, ps[i].prev_pos), 1./dt);
    }
}

void double_density_relaxation(Particle ps[], size_t count, TimeInterval dt, Scalar rho_0, Scalar k, Scalar k_near, Scalar h)
{
    for (size_t i = 0; i < count; ++i)
    {
        Scalar rho = 0.;
        Scalar rho_near = 0.;
        for (size_t j = 0; j < count; j++) // Neighboring particles
        {
            if (j == i) continue;
            Scalar q = vec_dist(ps[i].pos, ps[j].pos) / h;
            if (q < 1.)
            {
                rho += (1. - q) * (1. - q);
            }
        }
        Scalar P = k * (rho - rho_0);
        Scalar P_near = k_near * rho_near;
        Vec2d dx = {.x = 0., .y = 0.};
        for (size_t j = 0; j < count; j++) // Neighboring particles
        {
            Scalar q = vec_dist(ps[i].pos, ps[j].pos) / h;
            if (q < 1.)
            {
                Vec2d r_hat = vec_unit(vec_sub(ps[i].pos, ps[j].pos));
                Scalar a = dt * dt * (P * (1. - q) + P_near * (1. - q) * (1. - q));
                Vec2d D = vec_scale(r_hat, a * 0.5);
                ps[j].pos = vec_add(ps[j].pos, D);
                dx = vec_sub(dx, D);
            }
        }
        
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

void resolve_collisions(Particle ps[], size_t count, TimeInterval dt)
{
    
}

