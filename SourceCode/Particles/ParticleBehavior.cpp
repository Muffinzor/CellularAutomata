//
// Created by leo on 7/17/25.
//

#include "ParticleBehavior.h"
#include "../Utility.h"
#include <cmath>

static bool destroyed = false;

bool straight_down(CellularMatrix& matrix, Particle* p, int x, int y) {
    int next_y = y + 1;
    if (next_y >= matrix.height) {
        destroyed = true;
        delete p;
        matrix.set_current_cell(x, y, nullptr);
        matrix.particles--;
        return false;
    }
    bool canMoveDown = matrix.get_current_cell(x, next_y) == nullptr;
    if (canMoveDown) {
        matrix.set_current_cell(x, next_y, p);
        matrix.set_current_cell(x, y, nullptr);
        return true;
    }
    Particle* blocking_particle = matrix.get_current_cell(x, next_y);
    float avg_velocity = (blocking_particle->velocity.y + p->velocity.y)/2;
    blocking_particle->velocity.y = avg_velocity;
    p->velocity.y = avg_velocity;
    return false;
}

int diagonal_slide(CellularMatrix& matrix, int x, int y, Particle* p) {
    bool can_slide_DR = matrix.get_current_cell(x + 1, y + 1) == nullptr || matrix.get_current_cell(x + 1, y + 1)->type == ParticleType::Liquid;
    bool can_slide_DL = matrix.get_current_cell(x - 1, y + 1) == nullptr || matrix.get_current_cell(x - 1, y + 1)->type == ParticleType::Liquid;
    int direction = 0;
    if (can_slide_DR && can_slide_DL) {
        Utility::random_bool() ? direction = -1 : direction = 1;
    } else if (can_slide_DR) {
        direction = 1;
    } else if (can_slide_DL) {
        direction = -1;
    }
    if (direction != 0) {
        int next_y = y + 1;
        if (next_y >= matrix.height || x + direction >= matrix.width || x + direction < 0) {
            destroyed = true;
            delete p;
            matrix.set_current_cell(x, y, nullptr);
            matrix.particles--;
            return 0;
        }
        Particle* replaced = matrix.get_current_cell(x + direction, next_y);
        matrix.set_current_cell(x + direction, next_y, p);
        matrix.set_current_cell(x, y, replaced);
        if (replaced != nullptr && replaced->type == ParticleType::Liquid) p->velocity.y = 1;
        return direction;
    }
    return direction;
}

int horizontal_slide(CellularMatrix& matrix, int x, int y, Particle* p) {
    int direction = 0;
    while (p->velocity.x == 0) {
        p->velocity.x = Utility::random_float(-1, 1);
    }
    if (p->velocity.x < 0) direction = -1;
    if (p->velocity.x > 0) direction = 1;
    bool can_move_in_direction = matrix.get_current_cell(x + direction, y) == nullptr;
    if (can_move_in_direction) {
        if (x + direction < 0 || x + direction >= matrix.width) {
            destroyed = true;
            delete p;
            matrix.set_current_cell(x, y, nullptr);
            matrix.particles--;
            return 0;
        }
        matrix.set_current_cell(x + direction, y, p);
        matrix.set_current_cell(x, y, nullptr);
        return direction;
    }
    p->velocity.x = -p->velocity.x;
    return 0;
}

void set_free_falling(CellularMatrix& matrix, Particle* current, int x, int y) {
    Particle* left_particle = matrix.get_current_cell(x - 1, y);
    Particle* right_particle = matrix.get_current_cell(x + 1, y);
    if (left_particle != nullptr) {
        float resistance = left_particle->inertial_resistance;
        if (Utility::random_float(0,1) > resistance)
            left_particle->free_falling = true;
    }
    if (right_particle != nullptr) {
        float resistance = right_particle->inertial_resistance;
        if (Utility::random_float(0,1) > resistance)
            right_particle->free_falling = true;
    }
}

bool friction_stop(CellularMatrix& matrix, Particle* current, int x, int y) {
    Particle* under_particle = matrix.get_current_cell(x, y + 1);
    if (under_particle == nullptr) return true;
    float stop_chance = under_particle->friction;
    if (Utility::random_float(0,1) < stop_chance) {
        current->free_falling = false;
        return true;
    }
    return false;
}

/** causes segfault, needs fix */
void swap_place_density(CellularMatrix& matrix, int x, int y) {
    Particle* under_particle = matrix.get_current_cell(x, y + 1);

    if (under_particle != nullptr && under_particle->free_falling) {
        Particle* current = matrix.get_current_cell(x, y);
        if (current->density > under_particle->density) {
            matrix.set_current_cell(x, y, under_particle);
            matrix.set_current_cell(x, y + 1, current);
        }
    }
}

bool solid_sink(CellularMatrix& matrix, int x, int y, Particle* current) {
    Particle* under_particle = matrix.get_current_cell(x, y + 1);
    if (under_particle != nullptr && under_particle->type == ParticleType::Liquid) {
        matrix.set_current_cell(x, y + 1, current);
        matrix.set_current_cell(x, y, under_particle);
        current->velocity.y = 1;
        return true;
    }
    return false;
}

void ParticleBehavior::solid_behavior(CellularMatrix& matrix, int x, int y) {
    Particle* current = matrix.get_current_cell(x, y);
    destroyed = false;
    bool has_moved = false;
    current->already_processed = true;
    if (current->immovable) {
        return;
    }
    current->velocity.y += 0.2f;

    int current_x = x;
    int current_y = y;
    int moves = std::floor(current->velocity.y);

    for (int i = 0; i < moves; ++i) {
        if (solid_sink(matrix, current_x, current_y, current)) {
            current_y++;
        }
        if (!destroyed && straight_down(matrix, current, current_x, current_y)) {
            current_y++;
            has_moved = true;
            set_free_falling(matrix, current, current_x, current_y);
        } else if (!destroyed && current->free_falling) {
            if (friction_stop(matrix, current, x, y)) break;
            int direction = diagonal_slide(matrix, current_x, current_y, current);
            if (!direction == 0) {
                current_x += direction;
                current_y++;
                has_moved = true;
                set_free_falling(matrix, current, current_x, current_y);
            } else {
                current_x += horizontal_slide(matrix, current_x, current_y, current);
                if (friction_stop(matrix, current, x, y)) break;
                i = i * 2;
            }
        }
    }
    if (!has_moved) {
        current->velocity.y = 1;
        current->velocity.x = 0;
        current->free_falling = false;
    } else {
        current->free_falling = true;
    }
}

int liquid_slide(CellularMatrix& matrix, int x, int y) {
    Particle* current = matrix.get_current_cell(x, y);
    int current_x = x;
    int current_y = y;
    int jump = 0;
    int direction = 0;
    bool check_right = true;
    bool check_left = true;

    while (jump <= current->dispersion) {
        jump++;
        bool can_slide_DR = false;
        bool can_slide_DL = false;
        if (check_right) {
            can_slide_DR = matrix.get_current_cell(current_x + jump, current_y + 1) == nullptr;
            check_right = matrix.get_current_cell(current_x + jump, current_y) == nullptr;
        }
        if (check_left) {
            can_slide_DL = matrix.get_current_cell(current_x - jump, current_y + 1) == nullptr;
            check_left = matrix.get_current_cell(current_x - jump, current_y) == nullptr;
        }
        if (can_slide_DR && can_slide_DL) {
            Utility::random_bool() ? direction = -jump : direction = jump;
        } else if (can_slide_DR) {
            direction = jump;
        } else if (can_slide_DL) {
            direction = -jump;
        }
        if (direction != 0) {
            int next_y = y + 1;
            if (next_y >= matrix.height || x + direction < 0 || x + direction >= matrix.width) {
                destroyed = true;
                delete current;
                matrix.set_current_cell(current_x, current_y, nullptr);
                matrix.particles--;
                break;
            }
            matrix.set_current_cell(current_x + direction, current_y + 1, current);
            matrix.set_current_cell(current_x, current_y, nullptr);
            return direction;
        }
    }
    return direction;
}

void ParticleBehavior::liquid_behavior(CellularMatrix& matrix, int x, int y) {
    Particle* current = matrix.get_current_cell(x, y);
    destroyed = false;
    bool has_moved = false;
    current->already_processed = true;
    if (current->immovable) {
        return;
    }
    current->velocity.y += 0.2f;

    int current_x = x;
    int current_y = y;
    int moves = std::floor(current->velocity.y);

    for (int i = 0; i < moves; ++i) {
        if (!destroyed && straight_down(matrix, current, current_x, current_y)) {
            current_y++;
            has_moved = true;
            set_free_falling(matrix, current, current_x, current_y);
        } else if (!destroyed) {
            int direction = liquid_slide(matrix, current_x, current_y);
            if (!direction == 0) {
                current_x += direction;
                current_y++;
                has_moved = true;
            } else {
                for (int j = 0; j < current->dispersion; ++j) {
                    current->velocity.y = current->velocity.y * 0.8f;
                    current_x += horizontal_slide(matrix, current_x, current_y, current);
                    if (matrix.get_current_cell(current_x, current_y + 1) == nullptr) {
                        break;
                    }
                }
            }
        }
    }
    if (!has_moved) {
        current->velocity.y = 1;
    }
}

