#include "CellularMatrix.h"

#include <cmath>
#include <iostream>

#include "../Utility.h"
#include "../Particles/ParticleBehavior.h"

#define CHUNK_SIZE 16 // number of cells per edge of a chunk
#define NBR_OF_CHUNKS 25  // number of chunks per edge of the screen

CellularMatrix::CellularMatrix(int width, int height)
	: width(width), height(height) {

	cells.resize(height);
	for (int row = 0; row < height; ++row) {
		cells[row].resize(width, nullptr);
	}

    int chunk_pixels = CHUNK_SIZE;
    int chunks_x = NBR_OF_CHUNKS;
    int chunks_y = NBR_OF_CHUNKS;

    chunks.resize(chunks_y);
    for (int cy = 0; cy < chunks_y; ++cy) {
        chunks[cy].reserve(chunks_x);
        for (int cx = 0; cx < chunks_x; ++cx) {
            int chunk_x_pos = cx * chunk_pixels;
            int chunk_y_pos = cy * chunk_pixels;
            chunks[cy].emplace_back(chunk_x_pos, chunk_y_pos, CHUNK_SIZE);
        }
    }
}

/** Draws every particle in the grid in one draw call */
void CellularMatrix::display_matrix(sf::RenderWindow& window, int CELL_SIZE) {
    sf::VertexArray vertices(sf::PrimitiveType::Triangles);

    // reserve max vertices per triangle
    vertices.resize(width * height * 6);

    int vertexCount = 0;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Particle* p = get_cell(x, y);
            if (p != nullptr) {
                float xPos = static_cast<float>(x * CELL_SIZE);
                float yPos = static_cast<float>(y * CELL_SIZE);
                sf::Color color = p->color;

                vertices[vertexCount + 0].position = sf::Vector2f(xPos, yPos);
                vertices[vertexCount + 0].color = color;

                vertices[vertexCount + 1].position = sf::Vector2f(xPos + CELL_SIZE, yPos);
                vertices[vertexCount + 1].color = color;

                vertices[vertexCount + 2].position = sf::Vector2f(xPos + CELL_SIZE, yPos + CELL_SIZE);
                vertices[vertexCount + 2].color = color;

                vertices[vertexCount + 3].position = sf::Vector2f(xPos, yPos);
                vertices[vertexCount + 3].color = color;

                vertices[vertexCount + 4].position = sf::Vector2f(xPos + CELL_SIZE, yPos + CELL_SIZE);
                vertices[vertexCount + 4].color = color;

                vertices[vertexCount + 5].position = sf::Vector2f(xPos, yPos + CELL_SIZE);
                vertices[vertexCount + 5].color = color;

                vertexCount += 6;
            }
        }
    }
    vertices.resize(vertexCount);
    window.draw(vertices);
}

void CellularMatrix::set_cell(int x, int y, Particle* particle) {
	if(x >= 0 && x < width && y >= 0 && y < height)
		cells[y][x] = particle;
}

Particle* CellularMatrix::get_cell(int x, int y) {
	if (x >= 0 && x < width && y >= 0 && y < height)
		return cells[y][x];
	return nullptr;
}

void reset_particles(CellularMatrix& matrix) {
	int height = matrix.height;
	int width = matrix.width;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			Particle* current = matrix.cells[y][x];
			if (current != nullptr) {
				current->already_processed = false;
			}
		}
	}
}

void CellularMatrix::update_all_cells() {
    reset_particles(*this);

    for (int cy = NBR_OF_CHUNKS - 1; cy >= 0; cy--) {
        for (int cx = 0; cx < NBR_OF_CHUNKS; ++cx) {
            Chunk& chunk = chunks[cy][cx];
            if (!chunk.wake_frames > 0) continue;

            int start_y = cy * CHUNK_SIZE;
            int end_y = std::min(height, start_y + CHUNK_SIZE);

            int start_x = cx * CHUNK_SIZE;
            int end_x = std::min(width, start_x + CHUNK_SIZE);

            for (int y = end_y - 1; y >= start_y; --y) {
                bool leftToRight = Utility::random_bool();
                if (leftToRight) {
                    for (int x = start_x; x < end_x; ++x) {
                        update_cell(x, y);
                    }
                }
                else {
                    for (int x = end_x - 1; x >= start_x; --x) {
                        update_cell(x, y);
                    }
                }
            }
        }
    }

    cycle_chunks();
}

Chunk* CellularMatrix::get_chunk(int x, int y) {
    int chunk_x = x / CHUNK_SIZE;
    int chunk_y = y / CHUNK_SIZE;
    if (chunk_y < 0 || chunk_y >= (int)chunks.size() || chunk_x < 0 || chunk_x >= (int)chunks[0].size()) {
        return nullptr;
    }
    return &chunks[chunk_y][chunk_x];
}

void CellularMatrix::wake_chunks(int x, int y) {
    Chunk* chunk = get_chunk(x, y);  // x,y in cells coord

    int chunk_x = x / CHUNK_SIZE;
    int chunk_y = y / CHUNK_SIZE;

    int given_frames = 5;
    chunk->wake_frames = given_frames;

    Chunk* chunk_left = (chunk_x > 0) ? &chunks[chunk_y][chunk_x - 1] : nullptr;
    Chunk* chunk_right = (chunk_x < 25 - 1) ? &chunks[chunk_y][chunk_x + 1] : nullptr;
    Chunk* chunk_top = (chunk_y > 0) ? &chunks[chunk_y - 1][chunk_x] : nullptr;
    Chunk* chunk_bottom = (chunk_y < 25 - 1) ? &chunks[chunk_y + 1][chunk_x] : nullptr;

    if (chunk_left) chunk_left->wake_frames = given_frames;
    if (chunk_right) chunk_right->wake_frames = given_frames;
    if (chunk_top) chunk_top->wake_frames = given_frames;
    if (chunk_bottom) chunk_bottom->wake_frames = given_frames;
}

void CellularMatrix::update_cell(int x, int y) {
	Particle* p = get_cell(x, y);
	if (p != nullptr && !p->already_processed) {
		switch (p->type) {
		case ParticleType::Solid:
			ParticleBehavior::solid_behavior(*this, x, y);
			break;
		}
	}
}

bool CellularMatrix::is_cell_empty(int x, int y) {
	return x >= 0 && x < width && y >= 0 && y < height && cells[y][x] == nullptr;
}

void CellularMatrix::delete_particle(Particle* p, int x, int y) {
    delete p;
    particles--;
    set_cell(x, y, nullptr);
}

void CellularMatrix::cycle_chunks() {
    for (int i = 0; i < NBR_OF_CHUNKS; i++) {
        for (int j = 0; j < NBR_OF_CHUNKS; j++) {
            chunks[i][j].wake_frames--;
        }
    }
}

void CellularMatrix::display_chunk_debug(sf::RenderWindow& window, int CELL_SIZE) {
    for (int cy = 0; cy < chunks.size(); ++cy) {
        for (int cx = 0; cx < chunks[cy].size(); ++cx) {
            if (chunks[cy][cx].wake_frames > 0) {
                float x = cx * CHUNK_SIZE * CELL_SIZE;
                float y = cy * CHUNK_SIZE * CELL_SIZE;

                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(CHUNK_SIZE * CELL_SIZE, CHUNK_SIZE * CELL_SIZE));
                sf::Vector2f pos{ x, y };
                rect.setPosition(pos);
                rect.setFillColor(sf::Color::Transparent);
                rect.setOutlineColor(sf::Color::Green);
                rect.setOutlineThickness(1.0f);

                window.draw(rect);
            }
        }
    }
}