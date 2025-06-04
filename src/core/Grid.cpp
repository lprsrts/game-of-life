#include "Grid.hpp"

Grid::Grid(unsigned int width, unsigned int height)
    : width(width), height(height), cells(height, std::vector<bool>(width, false)) {
}

void Grid::toggleCell(unsigned int x, unsigned int y) {
    if (isValidPosition(x, y)) {
        cells[y][x] = !cells[y][x];
    }
}

void Grid::setCell(unsigned int x, unsigned int y, bool alive) {
    if (isValidPosition(x, y)) {
        cells[y][x] = alive;
    }
}

bool Grid::getCell(unsigned int x, unsigned int y) const {
    if (isValidPosition(x, y)) {
        return cells[y][x];
    }
    return false;
}

void Grid::clear() {
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            cells[y][x] = false;
        }
    }
}

void Grid::nextGeneration() {
    std::vector<std::vector<bool>> nextCells(height, std::vector<bool>(width, false));

    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            int liveNeighbors = countLiveNeighbors(x, y);

            if (cells[y][x]) {
                // Current cell is alive
                if (liveNeighbors == 2 || liveNeighbors == 3) {
                    nextCells[y][x] = true; // Survives to next generation
                }
            } else {
                // Current cell is dead
                if (liveNeighbors == 3) {
                    nextCells[y][x] = true; // Birth occurs with exactly 3 neighbors
                }
            }
        }
    }

    cells = std::move(nextCells);
}

int Grid::countLiveNeighbors(unsigned int x, unsigned int y) const {
    int liveNeighbors = 0;

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue; // Skip the center cell

            int nx = static_cast<int>(x) + dx;
            int ny = static_cast<int>(y) + dy;

            if (nx >= 0 && nx < static_cast<int>(width) &&
                ny >= 0 && ny < static_cast<int>(height)) {
                if (cells[ny][nx]) {
                    liveNeighbors++;
                }
            }
        }
    }

    return liveNeighbors;
}

bool Grid::isValidPosition(unsigned int x, unsigned int y) const {
    return x < width && y < height;
}
