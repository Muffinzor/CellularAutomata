#include "Chunk.h"

Chunk::Chunk(int x, int y, int size) {
    x_pos = x;
    y_pos = y;
    this->size = size;
    step = true;
    step_next_frame = true;
}