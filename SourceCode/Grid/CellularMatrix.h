#pragma once
#include <thread>
#include <vector>
#include <SFML/Graphics.hpp>
#include "../Particles/Particle.h"
#include "../ThreadPool.h"
#include "Chunk.h"

class CellularMatrix {
public:
    int width;
    int height;
    int particles = 0;
    std::vector<std::vector<Particle*>> cells;
    std::vector<std::vector<Chunk>> chunks;

    int num_threads = std::thread::hardware_concurrency();
    std::vector<sf::VertexArray> thread_vertex_arrays;

    CellularMatrix(int width, int height);
    void display_matrix(sf::RenderWindow& window, int CELL_SIZE, ThreadPool& thread_pool);

    void set_cell(int x, int y, Particle* particle);
    Particle* get_cell(int x, int y);
    void update_all_cells();
    void delete_particle(Particle* p, int x, int y);


    Chunk* get_chunk(int x, int y);
    void wake_chunks(int x, int y);
    void display_chunk_debug(sf::RenderWindow& window, int CELL_SIZE);

private:
    bool is_cell_empty(int x, int y);
    void update_cell(int x, int y);
    void update_solid_cell(int x, int y);
    void cycle_chunks();
};
