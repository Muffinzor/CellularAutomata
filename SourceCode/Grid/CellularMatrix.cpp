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
    next_cells.resize(height);
	for (int row = 0; row < height; ++row) {
		cells[row].resize(width, nullptr);
	    next_cells[row].resize(width, nullptr);
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
    thread_vertex_arrays.resize(num_threads);
    for (auto& va : thread_vertex_arrays) {
        va.setPrimitiveType(sf::PrimitiveType::Triangles);
    }
}

/** Draws every particle in the grid in one draw call */
void CellularMatrix::display_matrix(sf::RenderWindow& window, int Cell_size, ThreadPool& pool) {
    const int chunk_width = width / num_threads;
    const int vertex_count = 6;

    for (auto& va : thread_vertex_arrays) {
        va.clear();
    }

    for (int i = 0; i < num_threads; ++i) {
        int actual_chunk_width = (i == num_threads - 1) ? width - i * chunk_width : chunk_width;
        thread_vertex_arrays[i].setPrimitiveType(sf::PrimitiveType::Triangles);
        thread_vertex_arrays[i].resize(actual_chunk_width * height * vertex_count);
    }

    for (int t = 0; t < num_threads; ++t) {
        pool.enqueue([this, t, chunk_width, vertex_count, Cell_size]() {
            int start_x = t * chunk_width;
            int end_x = (t == num_threads - 1) ? width : (t + 1) * chunk_width;
            auto& va = thread_vertex_arrays[t];

            int index = 0;

            for (int y = 0; y < height; ++y) {
                for (int x = start_x; x < end_x; ++x) {
                    Particle* p = cells[y][x];
                    if (!p) continue;

                    sf::Color color = p->color;

                    float xf = static_cast<float>(x * Cell_size);
                    float yf = static_cast<float>(y * Cell_size);

                    va[index + 0].position = sf::Vector2f(xf, yf);
                    va[index + 1].position = sf::Vector2f(xf + Cell_size, yf);
                    va[index + 2].position = sf::Vector2f(xf + Cell_size, yf + Cell_size);

                    va[index + 3].position = sf::Vector2f(xf, yf);
                    va[index + 4].position = sf::Vector2f(xf + Cell_size, yf + Cell_size);
                    va[index + 5].position = sf::Vector2f(xf, yf + Cell_size);

                    for (int i = 0; i < vertex_count; ++i) {
                        va[index + i].color = color;
                    }

                    index += vertex_count;
                }
            }

            va.resize(index);
        });
    }

    pool.wait_all();

    for (auto& va : thread_vertex_arrays) {
        window.draw(va);
    }
}

void CellularMatrix::set_current_cell(int x, int y, Particle* particle) {
	if(x >= 0 && x < width && y >= 0 && y < height)
		cells[y][x] = particle;
}
void CellularMatrix::set_next_cell(int x, int y, Particle* particle) {
    if(x >= 0 && x < width && y >= 0 && y < height)
        next_cells[y][x] = particle;
}
Particle* CellularMatrix::get_current_cell(int x, int y) {
	if (x >= 0 && x < width && y >= 0 && y < height)
		return cells[y][x];
	return nullptr;
}
Particle* CellularMatrix::get_next_cell(int x, int y) {
    if (x >= 0 && x < width && y >= 0 && y < height)
        return next_cells[y][x];
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

void CellularMatrix::update_chunk(int chunk_x, int chunk_y, Chunk* chunk) {
    if (chunk->wake_frames <= 0) return;
    int start_x = chunk_x * CHUNK_SIZE;
    int start_y = chunk_y * CHUNK_SIZE;

    for (int y = start_y + CHUNK_SIZE - 1; y >= start_y; --y) {
        bool left_to_right = Utility::random_bool();
        if (left_to_right) {
            for (int x = start_x; x < start_x + CHUNK_SIZE; ++x) {
                update_cell(x, y);
            }
        } else {
            for (int x = start_x + CHUNK_SIZE - 1; x >= start_x; --x) {
                update_cell(x, y);
            }
        }
    }
}

/**
void CellularMatrix::update_all_cells() {
    reset_particles(*this);
    cycle_chunks();

    for (int chunk_y = NBR_OF_CHUNKS - 1; chunk_y >= 0; --chunk_y) {
        bool left_to_right = Utility::random_bool();
        if (left_to_right) {
            for (int chunk_x = 0; chunk_x < NBR_OF_CHUNKS; ++chunk_x) {
                Chunk* chunk = get_chunk(chunk_x, chunk_y);
                if (chunk) update_chunk(chunk_x, chunk_y, chunk);
            }
        } else {
            for (int chunk_x = NBR_OF_CHUNKS - 1; chunk_x >= 0; --chunk_x) {
                Chunk* chunk = get_chunk(chunk_x, chunk_y);
                if (chunk) update_chunk(chunk_x, chunk_y, chunk);
            }
        }
    }
}
*/

void CellularMatrix::update_all_cells() {
    reset_particles(*this);
    cycle_chunks();

    for (int y = height - 1; y >= 0; --y) {
        bool left_to_right = Utility::random_bool();
        if (left_to_right) {
            for (int x = 0; x < width; ++x) {
                update_cell(x, y);
            }
        } else {
            for (int x = width - 1; x >= 0; --x) {
                update_cell(x, y);
            }
        }
    }
}

Chunk* CellularMatrix::get_chunk(int x, int y) {
    int chunk_x = x;
    int chunk_y = y;
    if (chunk_y < 0 || chunk_y >= (int)chunks.size() || chunk_x < 0 || chunk_x >= (int)chunks[0].size()) {
        return nullptr;
    }
    return &chunks[chunk_y][chunk_x];
}

void CellularMatrix::wake_chunks(int x, int y) {
    int chunk_x = x / CHUNK_SIZE;
    int chunk_y = y / CHUNK_SIZE;
    Chunk* chunk = get_chunk(chunk_x, chunk_y);

    int given_frames = 5;
    chunk->wake_frames = given_frames;

    Chunk* chunk_left = (chunk_x > 0) ? &chunks[chunk_y][chunk_x - 1] : nullptr;
    Chunk* chunk_right = (chunk_x < NBR_OF_CHUNKS - 1) ? &chunks[chunk_y][chunk_x + 1] : nullptr;
    Chunk* chunk_top = (chunk_y > 0) ? &chunks[chunk_y - 1][chunk_x] : nullptr;
    Chunk* chunk_bottom = (chunk_y < NBR_OF_CHUNKS - 1) ? &chunks[chunk_y + 1][chunk_x] : nullptr;

    if (chunk_left) chunk_left->wake_frames = given_frames;
    if (chunk_right) chunk_right->wake_frames = given_frames;
    if (chunk_top) chunk_top->wake_frames = given_frames;
    if (chunk_bottom) chunk_bottom->wake_frames = given_frames;
}

void CellularMatrix::update_cell(int x, int y) {
	Particle* p = get_current_cell(x, y);
	if (p != nullptr && !p->already_processed) {
		switch (p->type) {
		    case ParticleType::Solid:
			    ParticleBehavior::solid_behavior(*this, x, y);
			    break;
		    case ParticleType::Liquid:
		        ParticleBehavior::liquid_behavior(*this, x, y);
		        break;
		}
	}
}


bool CellularMatrix::is_next_cell_empty(int x, int y) {
	return x >= 0 && x < width && y >= 0 && y < height && next_cells[y][x] == nullptr;
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