//
// Created by leo on 7/17/25.
//

#include "ParticleBehavior.h"
#include "../Utility.h"

#include <cmath>

bool diagonal_slide(CellularMatrix& matrix, int x, int y, Particle* p) {
    bool can_slide_DR = matrix.get_current_cell(x + 1, y + 1) == nullptr;
    bool can_slide_DL = matrix.get_current_cell(x - 1, y + 1) == nullptr;
    int direction = 0;
    bool has_slided = false;
    if (can_slide_DR && can_slide_DL) {
        Utility::random_bool() ? direction = -1 : direction = 1;
    } else if (can_slide_DR) {
        direction = 1;
    } else if (can_slide_DL) {
        direction = -1;
    }
    if (direction != 0) {
        matrix.set_current_cell(x + direction, y + 1, p);
        matrix.set_current_cell(x, y, nullptr);
        has_slided = true;
    }
    return has_slided;
}

void ParticleBehavior::solid_behavior(CellularMatrix& matrix, int x, int y) {
    Particle* current = matrix.get_current_cell(x, y);
    bool has_moved = false;
    current->already_processed = true;
    if (current->immovable) {
        return;
    };

    current->velocity.y += 0.2f;

    int fall_distance = std::floor(current->velocity.y);
    int current_y = y;
    int current_x = x;

    for (int i = 0; i < fall_distance; ++i) {
        int next_y = current_y + 1;
        if (next_y >= matrix.height) {
            delete current;
            matrix.set_current_cell(current_x, current_y, nullptr);
            matrix.particles--;
            return;
        }
        bool canMoveDown = matrix.get_current_cell(current_x, next_y) == nullptr;
        if (canMoveDown) {
            matrix.set_current_cell(current_x, next_y, current);
            matrix.set_current_cell(current_x, current_y, nullptr);
            current_y = next_y;
            has_moved = true;
        } else {
            Particle* blocking_particle = matrix.cells[next_y][current_x];
            if (blocking_particle != nullptr && blocking_particle->free_falling) {
                if (blocking_particle->velocity.y <= current->velocity.y) {
                    if (diagonal_slide(matrix, current_x, current_y, current)) has_moved = true;
                    if (has_moved) break;
                }
            }
            if (!has_moved) {
                if (diagonal_slide(matrix, current_x, current_y, current)) has_moved = true;
                if (has_moved) break;
            }
        }
    }

    if (has_moved) {
        matrix.wake_chunks(current_x, current_y);
        current->free_falling = true;
    } else {
        current->free_falling = false;
        current->velocity.y = 1;
    }
}
