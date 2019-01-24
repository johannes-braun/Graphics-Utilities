#ifndef PARTICLE_H
#define PARTICLE_H

struct particle_t
{
    vec3  position;
    float life;
    vec3  velocity;

    float max_lifetime;
};

#endif // PARTICLE_H