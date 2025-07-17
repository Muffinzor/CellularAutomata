#include "Utility.h"
#include <random>

namespace {
    std::mt19937& get_rng() {
        static std::mt19937 rng(std::random_device{}());
        return rng;
    }
}

int Utility::random_int(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max - 1);
    return dist(get_rng());
}

float Utility::random_float(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(get_rng());
}

bool Utility::random_bool() {
    std::bernoulli_distribution dist(0.5);
    return dist(get_rng());
}

sf::Color Utility::get_RGB(float hue, float sat, float val) {
    int h = static_cast<int>(hue / 60.0f) % 6;
    float f = (hue / 60.0f) - h;
    float p = val * (1 - sat);
    float q = val * (1 - f * sat);
    float t = val * (1 - (1 - f) * sat);

    float r = 0, g = 0, b = 0;

    switch (h) {
        case 0: r = val, g = t, b = p; break;
        case 1: r = q, g = val, b = p; break;
        case 2: r = p, g = val, b = t; break;
        case 3: r = p, g = q, b = val; break;
        case 4: r = t, g = p, b = val; break;
        case 5: r = val, g = p, b = q; break;
    }
    sf::Color color(r * 255, g * 255, b * 255);
    return color;
}