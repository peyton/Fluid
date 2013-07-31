//
//  Util.h
//  Fluid
//
//  Created by Peyton Randolph on 29/07/2013.
//  Copyright (c) 2013 Peyton Randolph. All rights reserved.
//

#ifndef Fluid_Util_h
#define Fluid_Util_h

#include <Box2D/Box2D.h>
#include "Common.h"

// adapted via ElectroDruid code from this post: http://www.box2d.org/forum/viewtopic.php?f=3&t=574&sid=0f208bac89ee07a05d5a524ef3b652cc&start=70

bool is_particle_fixture_collision(b2Fixture* fixture, Scalar radius, Vec2d pos, Vec2d nearest_pos, Vec2d impact_normal);

#endif
