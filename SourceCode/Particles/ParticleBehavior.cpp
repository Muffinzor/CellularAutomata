//
// Created by leo on 7/17/25.
//

#include "ParticleBehavior.h"

#include <cmath>

void ParticleBehavior::solid_behavior(CellularMatrix& matrix, int x, int y) {
    Particle* current = matrix.get_cell(x, y);
    current->already_processed = true;
    bool has_moved;
    if (current->immovable) return;

    current->velocity.y += 0.2f;

    int fall_distance = std::floor(current->velocity.y);
    int current_y = y;
    int current_x = x;

    for (int i = 0; i < fall_distance; ++i) {
        int next_y = current_y + 1;
        if (next_y >= matrix.height) {
            matrix.delete_particle(current, current_x, current_y);
            return;
        }
        bool canMoveDown = matrix.get_cell(current_x, next_y) == nullptr;
        if (canMoveDown) {
            matrix.cells[next_y][current_x] = current;
            matrix.cells[current_y][current_x] = nullptr;
            current_y = next_y;
            has_moved = true;
        } else {
            Particle* blocking_particle = matrix.cells[next_y][current_x];
            if (blocking_particle != nullptr && blocking_particle->velocity.y < current->velocity.y) {
                float avg_velocity = (blocking_particle->velocity.y + current->velocity.y) / 2;
                blocking_particle->velocity.y = avg_velocity;
                current->velocity.y = avg_velocity;
            }
        }
    }

    if (has_moved) {
        matrix.wake_chunks(current_x, current_y);
    }
}
