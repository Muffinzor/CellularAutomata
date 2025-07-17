#pragma once
#include "Particle.h"

class StoneParticle : public Particle {
public:
    StoneParticle() {
        type = ParticleType::Solid;
        immovable = true;
        hue = Utility::random_int(55, 70);
        float brightness = Utility::random_float(0.4f, 0.55f);
        color = Utility::get_RGB(hue, 0, brightness);
        base_color = Utility::get_RGB(hue, 0, brightness);
    }
};