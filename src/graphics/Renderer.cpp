#include "Renderer.hpp"
#include "../core/Grid.hpp"
#include "../ui/UIManager.hpp"
#include <algorithm>

Renderer::Renderer(sf::RenderWindow& window)
    : window(window), showGrid(true) {
}

void Renderer::render(const Grid& grid, const UIManager& uiManager) {
    clear();
    renderBackground();
    renderGridBorder();
    renderCells(grid);
    renderUI(uiManager);
    display();
}

void Renderer::clear() {
    window.clear();
}

void Renderer::display() {
    window.display();
}

sf::Vector2f Renderer::calculateGridOffset() const {
    sf::Vector2u windowSize = window.getSize();

    float availableWidth = windowSize.x - (2 * MARGIN);
    float availableHeight = windowSize.y - (2 * MARGIN) - BUTTON_HEIGHT - BUTTON_SPACING;

    availableWidth = std::max(availableWidth, 100.0f);
    availableHeight = std::max(availableHeight, 100.0f);

    float cellSize = calculateCellSize();
    float gridWidth = cellSize * GRID_WIDTH;
    float gridHeight = cellSize * GRID_HEIGHT;

    float offsetX = MARGIN + (availableWidth - gridWidth) / 2.0f;
    float offsetY = MARGIN + BUTTON_HEIGHT + BUTTON_SPACING + (availableHeight - gridHeight) / 2.0f;

    offsetX = std::max(MARGIN, offsetX);
    offsetY = std::max(MARGIN + BUTTON_HEIGHT + BUTTON_SPACING, offsetY);

    return sf::Vector2f(offsetX, offsetY);
}

float Renderer::calculateCellSize() const {
    sf::Vector2u windowSize = window.getSize();

    float availableWidth = windowSize.x - (2 * MARGIN);
    float availableHeight = windowSize.y - (2 * MARGIN) - BUTTON_HEIGHT - BUTTON_SPACING;

    availableWidth = std::max(availableWidth, 100.0f);
    availableHeight = std::max(availableHeight, 100.0f);

    float maxCellWidth = availableWidth / GRID_WIDTH;
    float maxCellHeight = availableHeight / GRID_HEIGHT;

    return std::max(1.0f, std::min(maxCellWidth, maxCellHeight));
}

std::pair<int, int> Renderer::screenToGrid(const sf::Vector2i& screenPos) const {
    sf::Vector2f gridOffset = calculateGridOffset();
    float cellSize = calculateCellSize();

    int gridX = static_cast<int>((screenPos.x - gridOffset.x) / cellSize);
    int gridY = static_cast<int>((screenPos.y - gridOffset.y) / cellSize);

    if (gridX < 0 || gridY < 0 || gridX >= static_cast<int>(GRID_WIDTH) ||
        gridY >= static_cast<int>(GRID_HEIGHT)) {
        return std::make_pair(-1, -1);
    }

    return std::make_pair(gridX, gridY);
}

void Renderer::renderBackground() const {
    if (!showGrid) return;

    sf::Vector2f gridOffset = calculateGridOffset();
    float cellSize = calculateCellSize();

    for (unsigned int y = 0; y < GRID_HEIGHT; ++y) {
        for (unsigned int x = 0; x < GRID_WIDTH; ++x) {
            sf::RectangleShape backgroundCell(sf::Vector2f(cellSize, cellSize));
            backgroundCell.setPosition(sf::Vector2f(
                gridOffset.x + x * cellSize,
                gridOffset.y + y * cellSize
            ));
            backgroundCell.setFillColor(getBackgroundColor(x, y));
            window.draw(backgroundCell);
        }
    }
}

void Renderer::renderGridBorder() const {
    sf::Vector2f gridOffset = calculateGridOffset();
    sf::Vector2f gridDimensions = getGridDimensions();

    sf::RectangleShape outerBorder(gridDimensions);
    outerBorder.setPosition(gridOffset);
    outerBorder.setOutlineThickness(3.0f);
    outerBorder.setOutlineColor(sf::Color(200, 200, 200));
    outerBorder.setFillColor(sf::Color::Transparent);
    window.draw(outerBorder);
}

void Renderer::renderCells(const Grid& grid) const {
    sf::Vector2f gridOffset = calculateGridOffset();
    float cellSize = calculateCellSize();

    const float cellPadding = cellSize * 0.1f;
    const float liveCellSize = cellSize - (cellPadding * 2);

    //const auto& cells = grid.getCells();

    for (unsigned int y = 0; y < grid.getHeight() && y < GRID_HEIGHT; ++y) {
        for (unsigned int x = 0; x < grid.getWidth() && x < GRID_WIDTH; ++x) {
            if (grid.getCell(x, y)) {
                sf::RectangleShape liveCell(sf::Vector2f(liveCellSize, liveCellSize));
                liveCell.setPosition(sf::Vector2f(
                    gridOffset.x + x * cellSize + cellPadding,
                    gridOffset.y + y * cellSize + cellPadding
                ));
                liveCell.setFillColor(sf::Color::Black);
                window.draw(liveCell);
            }
        }
    }
}

void Renderer::renderUI(const UIManager& uiManager) const {
    uiManager.draw(window);
}

sf::Vector2f Renderer::getGridDimensions() const {
    float cellSize = calculateCellSize();
    return sf::Vector2f(cellSize * GRID_WIDTH, cellSize * GRID_HEIGHT);
}

sf::Color Renderer::getBackgroundColor(unsigned int x, unsigned int y) const {
    if ((x + y) % 2 == 0) {
        return sf::Color(245, 245, 245);
    } else {
        return sf::Color(250, 250, 250);
    }
}
