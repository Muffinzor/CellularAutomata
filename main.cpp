#include <iostream>
#include <SFML/Graphics.hpp>

#include "SourceCode/InputManager.h"
#include "SourceCode/ThreadPool.h"
#include "SourceCode/Grid/CellularMatrix.h"

const int CELL_SIZE = 2;
const int GRID_WIDTH = 400;
const int GRID_HEIGHT = 400;

int main() {
    sf::RenderWindow window(sf::VideoMode({ GRID_WIDTH * CELL_SIZE, GRID_WIDTH * CELL_SIZE }), "Cellular Automata");
    ThreadPool pool(std::thread::hardware_concurrency());

    CellularMatrix matrix(GRID_WIDTH, GRID_HEIGHT);
    InputManager inputManager(matrix, window, CELL_SIZE);

    sf::Clock clock;
    const sf::Time timePerUpdate = sf::seconds(1.f / 60.f);
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    int frameCount = 0;
    sf::Clock fpsClock;


    while (window.isOpen()) {
        frameCount++;
        if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
            std::cout << "FPS: " << frameCount << "Particles: " << matrix.particles << std::endl;
            frameCount = 0;
            fpsClock.restart();
        }

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        timeSinceLastUpdate += clock.restart();
        while (timeSinceLastUpdate >= timePerUpdate) {
            inputManager.handle_input();
            matrix.update_all_cells();
            timeSinceLastUpdate -= timePerUpdate;
        }

        window.clear();
        matrix.display_matrix(window, CELL_SIZE, pool);
        //matrix.display_chunk_debug(window, CELL_SIZE);
        inputManager.draw_selection_rectangle(window);
        window.display();
    }
}
