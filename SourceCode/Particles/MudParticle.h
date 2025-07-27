#pragma once
#include "Particle.h"

class MudParticle : public Particle {
public:
    MudParticle() {
        type = ParticleType::Solid;
        hue = Utility::random_int(30, 45);
        float saturation = Utility::random_float(0.6f, 0.85f);
        color = Utility::get_RGB(hue, saturation, 0.4f);
        base_color = Utility::get_RGB(hue, saturation, 0.4f);
        inertial_resistance = 0.55f;
        friction = 0.4f;
        density = 15;
    }
};