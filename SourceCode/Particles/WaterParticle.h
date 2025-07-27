#pragma once
#include "Particle.h"

class WaterParticle : public Particle {
public:
    WaterParticle() {
        type = ParticleType::Liquid;
        hue = 190;
        float saturation = 0.85f;
        color = Utility::get_RGB(hue, saturation, 1);
        base_color = Utility::get_RGB(hue, saturation, 1);
        density = 15;
        dispersion = 5;
    }
};