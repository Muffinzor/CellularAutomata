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

    return false;
}

int diagonal_slide(CellularMatrix& matrix, int x, int y, Particle* p) {
    bool can_slide_DR = matrix.get_current_cell(x + 1, y + 1) == nullptr;
    bool can_slide_DL = matrix.get_current_cell(x - 1, y + 1) == nullptr;
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
        if (next_y >= matrix.height) {
            destroyed = true;
            delete p;
            matrix.set_current_cell(x, y, nullptr);
            matrix.particles--;
            return 0;
        }
        matrix.set_current_cell(x + direction, y + 1, p);
        matrix.set_current_cell(x, y, nullptr);
        return direction;
    }
    return direction;
}

void ParticleBehavior::solid_behavior(CellularMatrix& matrix, int x, int y) {
    Particle* current = matrix.get_current_cell(x, y);
    destroyed = false;
    bool has_moved = false;
    current->already_processed = true;
    if (current->immovable) {
        return;
    };
    current->velocity.y += 0.2f;

    int current_x = x;
    int current_y = y;
    int moves = std::floor(current->velocity.y);

    for (int i = 0; i < moves; ++i) {
        if (!destroyed && straight_down(matrix, current, current_x, current_y)) {
            current_y++;
            has_moved = true;
        } else if (!destroyed) {
            int direction = diagonal_slide(matrix, current_x, current_y, current);
            if (direction == 0) break;
            current_x += direction;
            current_y++;
            has_moved = true;
        }

    }
    if (!has_moved) {
        current->velocity.y = 1;
    }
}

