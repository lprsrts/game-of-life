#ifndef GRID_HPP
#define GRID_HPP

#include <vector>

class Grid {
public:
    Grid(unsigned int width, unsigned int height);
    
    // Basic grid operations
    void toggleCell(unsigned int x, unsigned int y);
    void setCell(unsigned int x, unsigned int y, bool alive);
    bool getCell(unsigned int x, unsigned int y) const;
    void clear();
    
    // Game logic
    void nextGeneration();
    int countLiveNeighbors(unsigned int x, unsigned int y) const;
    
    // Getters
    unsigned int getWidth() const { return width; }
    unsigned int getHeight() const { return height; }
    
    // Grid access for rendering
    const std::vector<std::vector<bool>>& getCells() const { return cells; }

private:
    std::vector<std::vector<bool>> cells;
    unsigned int width;
    unsigned int height;
    
    bool isValidPosition(unsigned int x, unsigned int y) const;
};

#endif // GRID_HPP