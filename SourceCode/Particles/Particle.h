#pragma once
#include <SFML/Graphics.hpp>

class CellularMatrix;

enum class ParticleType {
    Solid,
    Liquid,
    Gas
};

class Particle {
public:
    sf::Vector2f velocity{ 0,1 };
    sf::Color color;
    sf::Color base_color;
    int hue;
    int x_pos;
    int y_pos;
    int immobile_frames = 0;

    ParticleType type;
    bool immovable = false;
    float density;
    float friction;
    float inertial_resistance;
    int dispersion;

    bool free_falling = true;
    bool already_processed;

    virtual ~Particle() = default;
};