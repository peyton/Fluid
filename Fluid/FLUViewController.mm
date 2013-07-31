//
//  FLUViewController.m
//  Fluid
//
//  Created by Peyton Randolph on 7/25/13.
//  Copyright (c) 2013 Peyton Randolph. All rights reserved.
//

#import "FLUViewController.h"

#import "ParticleFluid.h"

@interface FLUViewController ()
{
    Particle *particles;
    size_t particles_count;
    Springs *springs;
}

@property (nonatomic, weak) NSTimer *updateTimer;
@property (nonatomic, copy) NSArray *particleLayers;

@end

@implementation FLUViewController

- (id)initWithCoder:(NSCoder *)aDecoder;
{
    if (!(self = [super initWithCoder:aDecoder]))
        return nil;
    
    particles_count = 2000;
    particles = (Particle *)calloc(particles_count, sizeof(Particle));
    springs = springs_create(particles_count);
    
    self.updateTimer = [NSTimer timerWithTimeInterval:0.01 target:self selector:@selector(_updateTimerFired:) userInfo:nil repeats:YES];
    [[NSRunLoop mainRunLoop] addTimer:self.updateTimer forMode:NSRunLoopCommonModes];
    
    return self;
}

- (void)dealloc;
{
    [self.updateTimer invalidate];
    free(particles);
    springs_free(springs);
}

- (void)viewDidLoad
{
    #define ARC4RANDOM_MAX      0x100000000
    [super viewDidLoad];
	
    NSMutableArray *particleLayers = [NSMutableArray arrayWithCapacity:particles_count];
    for (size_t i = 0; i < particles_count; ++i)
    {
        particles[i].pos = {.x = ((Scalar)arc4random() / ARC4RANDOM_MAX * 300), .y = ((Scalar)arc4random() / ARC4RANDOM_MAX * 500)};
        
        CAShapeLayer *particle = [[CAShapeLayer alloc] init];
        particle.path = [UIBezierPath bezierPathWithOvalInRect:CGRectMake(0.f, 0.f, 10.f, 10.f)].CGPath;
        particle.fillColor = [UIColor blueColor].CGColor;
        [particleLayers addObject:particle];
        [self.view.layer addSublayer:particle];
    }
    self.particleLayers = particleLayers;
}

#pragma mark - Updating

- (void)_updateTimerFired:(id)sender;
{
    [self update:0.01];
}

- (void)update:(TimeInterval)dt;
{
    RectF bounds = {.size = {.width = 320., .height = 500.}, .origin = {.x = 0., .y = 0.}};
    update(particles, particles_count, dt, springs, bounds);
    
    [CATransaction begin];
    [CATransaction setAnimationDuration:0.0];
    for (size_t i = 0; i < [self.particleLayers count]; ++i)
    {
//        NSLog(@"%zu, %f, %f", i, particles[i].pos.x, particles[i].pos.y);
        [self.particleLayers[i] setPosition:CGPointMake(particles[i].pos.x, particles[i].pos.y)];
    }
    
    [CATransaction commit];
}

@end
