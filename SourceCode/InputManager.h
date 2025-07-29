#pragma once
#include <SFML/Graphics.hpp>
#include "Grid/CellularMatrix.h"

class InputManager {
    CellularMatrix& matrix;
    sf::RenderWindow& window;
    int cellSize;

    int particle_spread = 3;
    int particle_multipler = 3;

public:
    InputManager(CellularMatrix& m, sf::RenderWindow& w, int cs)
        : matrix(m), window(w), cellSize(cs) {
    }
    void handle_input();
    bool left_click_add();
    void right_click_add();
    void draw_selection_rectangle(sf::RenderWindow& window);

private:
    void switch_particle();
    void delete_particle(int x, int y);
    bool is_within_bounds(int x, int y) const;
};