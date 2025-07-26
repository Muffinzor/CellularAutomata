#pragma once
#include "Particle.h"

class SandParticle : public Particle {
public:
    SandParticle() {
        type = ParticleType::Solid;
        hue = Utility::random_int(55, 70);
        color = Utility::get_RGB(hue, 1, 1);
        base_color = Utility::get_RGB(hue, 1, 1);
        inertial_resistance = 0.15f;
        friction = 0.1f;
    }
};