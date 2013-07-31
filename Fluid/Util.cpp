//
//  Util.c
//  Fluid
//
//  Created by Peyton Randolph on 29/07/2013.
//  Copyright (c) 2013 Peyton Randolph. All rights reserved.
//

#include "Util.h"

bool is_particle_fixture_collision(b2Fixture *fixture, Scalar radius, Vec2d pos, Vec2d *nearest_pos, Vec2d *impact_normal)
{
    b2Shape::Type shapeType = fixture->GetShape()->GetType();
    b2Vec2 particlePos = b2Vec2(pos.x, pos.y);
    switch (shapeType)
    {
        case b2Shape::e_circle:
        {
            b2CircleShape *circleShape = static_cast<b2CircleShape *>(fixture->GetShape());
            const b2Transform xf = fixture->GetBody()->GetTransform();
            Scalar totalRadius = circleShape->m_radius + radius;
            b2Vec2 circlePos = xf.p + circleShape->m_p;
            b2Vec2 delta = particlePos - circlePos;
            if (delta.LengthSquared() > totalRadius * totalRadius)
                return false;
            
            delta.Normalize();
            delta *= totalRadius;
            
            b2Vec2 nearest_pos_b2 = delta + circleShape->m_p;
            b2Vec2 impact_normal_b2 = nearest_pos_b2 - circlePos;
            impact_normal_b2.Normalize();
            
            *nearest_pos = {.x = nearest_pos_b2.x, .y = nearest_pos_b2.y};
            *impact_normal = {.x = impact_normal_b2.x, .y = impact_normal_b2.y};
            
            return true;
        }
        case b2Shape::e_polygon:
        {
            b2PolygonShape *polygonShape = static_cast<b2PolygonShape *>(fixture->GetShape());
            const b2Transform xf = fixture->GetBody()->GetTransform();
            int numVerts = polygonShape->GetVertexCount();
            
            b2Vec2 vertices[b2_maxPolygonVertices];
            b2Vec2 normals[b2_maxPolygonVertices];
            
            for (size_t i = 0; i < numVerts; ++i)
            {
                vertices[i] = b2Mul(xf, polygonShape->m_vertices[i]);
                normals[i] = b2Mul(xf.q, polygonShape->m_normals[i]);
            }
            
            Scalar shortestDistance = FLT_MAX;
            
            for (size_t i = 0; i < numVerts; ++i)
            {
                b2Vec2 vertex = vertices[i] + radius * normals[i] - particlePos;
                Scalar distance = b2Dot(normals[i], vertex);
                
                if (distance < 0.)
                    return false;
                
                if (distance < shortestDistance)
                {
                    shortestDistance = distance;
                    
                    *nearest_pos = {.x = normals[i].x * distance + particlePos.x, .y = normals[i].y * distance + particlePos.y};
                    *impact_normal = {.x = normals[i].x, .y = normals[i].y};
                }
            }
            
            return true;
        }
        default:
            // unsupported shape
            assert(false);
            return false;
    }
}