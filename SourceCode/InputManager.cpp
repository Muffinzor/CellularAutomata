#include "InputManager.h"
#include "Utility.h"
#include "Particles/SandParticle.h"
#include "Particles/StoneParticle.h"
#include "Particles/MudParticle.h"
#include "Particles/Particle.h"


bool right_click_held = false;
sf::Vector2i right_click_start;

int particle_type = 0;

void InputManager::handle_input() {
    if (!left_click_add()) right_click_add();
    switch_particle();
}

bool InputManager::is_within_bounds(int x, int y) const {
    if (x < 0 || x >= matrix.width || y < 0 || y >= matrix.height) return false;
    return true;
}

bool InputManager::left_click_add() {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        for (int i = 0; i < particle_multipler; i++) {
            int gridX = mousePos.x / cellSize;
            int gridY = mousePos.y / cellSize;
            gridX += Utility::random_int(-particle_spread, particle_spread);
            gridY += Utility::random_int(-particle_spread, particle_spread);
            if (matrix.get_current_cell(gridX, gridY) == nullptr && is_within_bounds(gridX, gridY)) {
                switch (particle_type) {
                    case 0:
                        matrix.set_current_cell(gridX, gridY, new SandParticle());
                        break;
                    case 1:
                        matrix.set_current_cell(gridX, gridY, new SandParticle());
                        break;
                    case 2:
                        matrix.set_current_cell(gridX, gridY, new MudParticle());
                        break;
                    case 3:
                        matrix.set_current_cell(gridX, gridY, new SandParticle());
                        break;
                }
                matrix.particles++;
                matrix.wake_chunks(gridX, gridY);
            }
        }
        return true;
    }
    return false;
}

void InputManager::right_click_add() {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
        if (!right_click_held) {
            right_click_held = true;
            right_click_start = sf::Mouse::getPosition(window);
        }
    } else {
        if (right_click_held) {
            sf::Vector2i right_click_end = sf::Mouse::getPosition(window);
            int startX = std::min(right_click_start.x, right_click_end.x) / cellSize;
            int endX   = std::max(right_click_start.x, right_click_end.x) / cellSize;
            int startY = std::min(right_click_start.y, right_click_end.y) / cellSize;
            int endY   = std::max(right_click_start.y, right_click_end.y) / cellSize;

            for (int y = startY; y <= endY; ++y) {
                for (int x = startX; x <= endX; ++x) {
                    if (matrix.get_current_cell(x, y) == nullptr && is_within_bounds(x, y) && particle_type < 3) {
                        switch (particle_type) {
                            case 0:
                                matrix.set_current_cell(x, y, new SandParticle());
                                break;
                            case 1:
                                matrix.set_current_cell(x, y, new StoneParticle());
                                break;
                            case 2:
                                matrix.set_current_cell(x, y, new MudParticle());
                                break;
                        }
                        matrix.particles++;
                        matrix.wake_chunks(x, y);
                    } else if (is_within_bounds(x, y) && particle_type == 3 && !(matrix.get_current_cell(x, y) == nullptr)) {
                        delete matrix.get_current_cell(x,y);
                        matrix.set_current_cell(x, y, nullptr);
                        matrix.particles--;
                    }
                }
            }
            right_click_held = false;
        }
    }
}

void InputManager::draw_selection_rectangle(sf::RenderWindow& window) {
    if (right_click_held) {
        sf::Vector2i current = sf::Mouse::getPosition(window);
        sf::Vector2i start = right_click_start;

        int x = std::min(start.x, current.x);
        int y = std::min(start.y, current.y);
        int width = std::abs(current.x - start.x);
        int height = std::abs(current.y - start.y);

        sf::RectangleShape rect;
        sf::Vector2f pos{(float)x, (float)y};
        rect.setPosition(pos);
        rect.setSize(sf::Vector2f(static_cast<float>(width), static_cast<float>(height)));
        rect.setFillColor(sf::Color(0, 100, 255, 60));
        rect.setOutlineColor(sf::Color(0, 100, 255, 120));
        rect.setOutlineThickness(2);

        window.draw(rect);
    }
}

void InputManager::switch_particle() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) particle_type = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) particle_type = 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) particle_type = 2;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) particle_type = 3;

}

