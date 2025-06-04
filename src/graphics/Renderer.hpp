#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SFML/Graphics.hpp>

// Forward declarations
class Grid;
class UIManager;

class Renderer {
public:
    Renderer(sf::RenderWindow& window);

    // Main rendering methods
    void render(const Grid& grid, const UIManager& uiManager);
    void clear();
    void display();

    // Layout calculations
    sf::Vector2f calculateGridOffset() const;
    float calculateCellSize() const;
    std::pair<int, int> screenToGrid(const sf::Vector2i& screenPos) const;

    // Grid rendering settings
    void setGridVisible(bool visible) { showGrid = visible; }
    bool isGridVisible() const { return showGrid; }

    // Constants
    static constexpr float MARGIN = 60.0f;
    static constexpr float BUTTON_HEIGHT = 40.0f;
    static constexpr float BUTTON_SPACING = 10.0f;
    static constexpr unsigned int GRID_WIDTH = 60;
    static constexpr unsigned int GRID_HEIGHT = 40;

private:
    sf::RenderWindow& window;
    bool showGrid;

    // Rendering methods
    void renderBackground() const;
    void renderGridBorder() const;
    void renderCells(const Grid& grid) const;
    void renderUI(const UIManager& uiManager) const;

    // Helper methods
    sf::Vector2f getGridDimensions() const;
    sf::Color getBackgroundColor(unsigned int x, unsigned int y) const;
};

#endif // RENDERER_HPP
