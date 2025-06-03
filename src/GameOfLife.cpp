#include "GameOfLife.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <algorithm>
#include <random>

GameOfLife::GameOfLife()
    : window(
          sf::VideoMode({GameOfLife::WINDOW_WIDTH, GameOfLife::WINDOW_HEIGHT}),
          "Game of Life", sf::Style::Default),
      cells(GRID_HEIGHT, std::vector<bool>(GRID_WIDTH, false)), width(GRID_WIDTH),
      height(GRID_HEIGHT), timePerGeneration(sf::seconds(1.f)), isPaused(false) {

  initializeButtons();
}

void GameOfLife::run() {
  while (window.isOpen()) {
    processEvents();
    if (!isPaused && clock.getElapsedTime() >= timePerGeneration) {
      nextGeneration();
      clock.restart();
    }
    window.clear();
    draw();
    window.display();
  }
}

sf::Vector2f GameOfLife::calculateGridOffset() {
  sf::Vector2u windowSize = window.getSize();

  // Calculate available space for the grid (minus margins and UI space)
  float availableWidth = windowSize.x - (2 * MARGIN);
  float availableHeight = windowSize.y - (2 * MARGIN) - BUTTON_HEIGHT - BUTTON_SPACING;

  // Ensure minimum dimensions to prevent issues
  availableWidth = std::max(availableWidth, 100.0f);
  availableHeight = std::max(availableHeight, 100.0f);

  // Calculate the maximum cell size while maintaining 1:1 ratio
  float maxCellWidth = availableWidth / GRID_WIDTH;
  float maxCellHeight = availableHeight / GRID_HEIGHT;
  float cellSize = std::max(1.0f, std::min(maxCellWidth, maxCellHeight));

  // Calculate total grid dimensions
  float gridWidth = cellSize * GRID_WIDTH;
  float gridHeight = cellSize * GRID_HEIGHT;

  // Center the grid in the available space (accounting for UI at top)
  float offsetX = MARGIN + (availableWidth - gridWidth) / 2.0f;
  float offsetY = MARGIN + BUTTON_HEIGHT + BUTTON_SPACING + (availableHeight - gridHeight) / 2.0f;

  // Ensure offsets don't go negative
  offsetX = std::max(MARGIN, offsetX);
  offsetY = std::max(MARGIN + BUTTON_HEIGHT + BUTTON_SPACING, offsetY);

  return sf::Vector2f(offsetX, offsetY);
}

std::pair<int, int> GameOfLife::screenToGrid(const sf::Vector2i &screenPos) {
  sf::Vector2f gridOffset = calculateGridOffset();
  sf::Vector2u windowSize = window.getSize();

  // Calculate available space and cell size (same as calculateGridOffset and draw)
  float availableWidth = windowSize.x - (2 * MARGIN);
  float availableHeight = windowSize.y - (2 * MARGIN) - BUTTON_HEIGHT - BUTTON_SPACING;

  // Ensure minimum dimensions to prevent issues
  availableWidth = std::max(availableWidth, 100.0f);
  availableHeight = std::max(availableHeight, 100.0f);

  float maxCellWidth = availableWidth / GRID_WIDTH;
  float maxCellHeight = availableHeight / GRID_HEIGHT;
  float cellSize = std::max(1.0f, std::min(maxCellWidth, maxCellHeight));

  // Convert screen coordinates to grid coordinates
  int gridX = static_cast<int>((screenPos.x - gridOffset.x) / cellSize);
  int gridY = static_cast<int>((screenPos.y - gridOffset.y) / cellSize);

  // Validate grid coordinates are within bounds
  if (gridX < 0 || gridY < 0 || gridX >= static_cast<int>(GRID_WIDTH) || gridY >= static_cast<int>(GRID_HEIGHT)) {
    return std::make_pair(-1, -1);
  }

  return std::make_pair(gridX, gridY);
}

void GameOfLife::processEvents() {
  std::optional<sf::Event> event;
  while ((event = window.pollEvent())) {
    if (event->is<sf::Event::Closed>()) {
      window.close();
    } else if (event->is<sf::Event::Resized>()) {
      // Handle window resize - update view and reinitialize layout
      sf::Vector2u newSize = event->getIf<sf::Event::Resized>()->size;

      // Update the view to match the new window size
      sf::View newView(sf::FloatRect({0.f, 0.f}, {static_cast<float>(newSize.x), static_cast<float>(newSize.y)}));
      window.setView(newView);

      // Reinitialize buttons for new layout
      initializeButtons();
    } else if (event->is<sf::Event::MouseButtonPressed>()) {
      auto mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
      if (mouseEvent->button == sf::Mouse::Button::Left) {
        sf::Vector2i mousePos(mouseEvent->position.x, mouseEvent->position.y);

        // Check if click is on a button first
        handleButtonClick(mousePos);

        // Then check if click is on the grid to toggle cells
        auto gridPos = screenToGrid(mousePos);
        if (gridPos.first >= 0 && gridPos.first < static_cast<int>(width) &&
            gridPos.second >= 0 && gridPos.second < static_cast<int>(height) &&
            gridPos.first < static_cast<int>(GRID_WIDTH) && gridPos.second < static_cast<int>(GRID_HEIGHT)) {
          toggleCell(gridPos.first, gridPos.second);
        }
      }
    } else if (event->is<sf::Event::MouseMoved>()) {
      // Update button hover states
      auto moveEvent = event->getIf<sf::Event::MouseMoved>();
      sf::Vector2i mousePos(moveEvent->position.x, moveEvent->position.y);
      for (auto& button : buttons) {
        button.updateHover(mousePos);
      }
    } else if (event->is<sf::Event::KeyPressed>()) {
      // Handle keyboard input
      auto keyEvent = event->getIf<sf::Event::KeyPressed>();
      switch (keyEvent->code) {
      case sf::Keyboard::Key::Space:
        // Toggle pause
        isPaused = !isPaused;
        break;
      case sf::Keyboard::Key::R:
        // Seed random pattern
        seedRandomPattern();
        break;
      case sf::Keyboard::Key::G:
        // Seed glider pattern
        seedGliderPattern();
        break;
      case sf::Keyboard::Key::C:
        // Clear grid
        clearGrid();
        break;
      case sf::Keyboard::Key::T:
        // Test pattern for coordinate verification
        seedTestPattern();
        break;
      case sf::Keyboard::Key::Equal:
      case sf::Keyboard::Key::Add:
        // Speed up (decrease time per generation)
        if (!isPaused && timePerGeneration.asSeconds() > 0.1f) {
          timePerGeneration = sf::seconds(timePerGeneration.asSeconds() * 0.8f);
        }
        break;
      case sf::Keyboard::Key::Hyphen:
      case sf::Keyboard::Key::Subtract:
        // Slow down (increase time per generation)
        if (!isPaused && timePerGeneration.asSeconds() < 2.0f) {
          timePerGeneration =
              sf::seconds(timePerGeneration.asSeconds() * 1.25f);
        }
        break;
      default:
        break;
      }
    }
  }
}

void GameOfLife::toggleCell(unsigned int x, unsigned int y) {
  cells[y][x] = !cells[y][x];
}

void GameOfLife::nextGeneration() {
  // Create a new grid for the next generation
  std::vector<std::vector<bool>> nextCells(height, std::vector<bool>(width, false));

  // Iterate through each cell
  for (unsigned int y = 0; y < height; ++y) {
    for (unsigned int x = 0; x < width; ++x) {
      // Count live neighbors
      int liveNeighbors = 0;

      // Check all 8 neighboring cells
      for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
          // Skip the cell itself
          if (dx == 0 && dy == 0) continue;

          int nx = static_cast<int>(x) + dx;
          int ny = static_cast<int>(y) + dy;

          // Check bounds
          if (nx >= 0 && nx < static_cast<int>(width) &&
              ny >= 0 && ny < static_cast<int>(height)) {
            if (cells[ny][nx]) {
              liveNeighbors++;
            }
          }
        }
      }

      // Apply Game of Life rules
      if (cells[y][x]) {
        // Live cell
        if (liveNeighbors == 2 || liveNeighbors == 3) {
          nextCells[y][x] = true; // Survives
        }
        // Otherwise dies (underpopulation or overpopulation)
      } else {
        // Dead cell
        if (liveNeighbors == 3) {
          nextCells[y][x] = true; // Birth
        }
        // Otherwise stays dead
      }
    }
  }

  // Update the grid with the new generation
  cells = std::move(nextCells);
}

void GameOfLife::clearGrid() {
  for (unsigned int y = 0; y < GRID_HEIGHT; ++y) {
    for (unsigned int x = 0; x < GRID_WIDTH; ++x) {
      cells[y][x] = false;
    }
  }
}

void GameOfLife::seedRandomPattern(float density) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dis(0.0f, 1.0f);

  clearGrid();
  for (unsigned int y = 0; y < GRID_HEIGHT; ++y) {
    for (unsigned int x = 0; x < GRID_WIDTH; ++x) {
      cells[y][x] = dis(gen) < density;
    }
  }
}

void GameOfLife::seedGliderPattern() {
  clearGrid();

  // Create a glider pattern in the upper-left area
  if (width >= 5 && height >= 5) {
    unsigned int startX = 2;
    unsigned int startY = 2;

    // Glider pattern:
    //  X
    //   X
    // XXX
    cells[startY][startX + 1] = true;
    cells[startY + 1][startX + 2] = true;
    cells[startY + 2][startX] = true;
    cells[startY + 2][startX + 1] = true;
    cells[startY + 2][startX + 2] = true;
  }
}

void GameOfLife::seedTestPattern() {
  clearGrid();

  // Create a test pattern to verify coordinate system after resize
  if (width >= 10 && height >= 10) {
    // Corner markers
    cells[0][0] = true;                           // Top-left
    cells[0][width - 1] = true;                   // Top-right
    cells[height - 1][0] = true;                  // Bottom-left
    cells[height - 1][width - 1] = true;          // Bottom-right

    // Center cross
    unsigned int centerX = width / 2;
    unsigned int centerY = height / 2;
    for (int i = -2; i <= 2; ++i) {
      if (centerX + i < width) {
        cells[centerY][centerX + i] = true;       // Horizontal line
      }
      if (centerY + i < height && centerY + i >= 0) {
        cells[centerY + i][centerX] = true;       // Vertical line
      }
    }

    // Border outline (every 5th cell)
    for (unsigned int x = 0; x < width; x += 5) {
      cells[0][x] = true;                         // Top border
      cells[height - 1][x] = true;                // Bottom border
    }
    for (unsigned int y = 0; y < height; y += 5) {
      cells[y][0] = true;                         // Left border
      cells[y][width - 1] = true;                 // Right border
    }
  }
}

void GameOfLife::draw() {
  sf::Vector2u windowSize = window.getSize();
  sf::Vector2f gridOffset = calculateGridOffset();

  // Calculate available space and cell size
  float availableWidth = windowSize.x - (2 * MARGIN);
  float availableHeight = windowSize.y - (2 * MARGIN) - BUTTON_HEIGHT - BUTTON_SPACING;

  // Ensure minimum dimensions
  availableWidth = std::max(availableWidth, 100.0f);
  availableHeight = std::max(availableHeight, 100.0f);

  float maxCellWidth = availableWidth / GRID_WIDTH;
  float maxCellHeight = availableHeight / GRID_HEIGHT;
  float cellSize = std::max(1.0f, std::min(maxCellWidth, maxCellHeight));

  float gridWidth = cellSize * GRID_WIDTH;
  float gridHeight = cellSize * GRID_HEIGHT;

  // Draw the subtle chessboard background pattern
  for (unsigned int y = 0; y < GRID_HEIGHT; ++y) {
    for (unsigned int x = 0; x < GRID_WIDTH; ++x) {
      sf::RectangleShape backgroundCell(sf::Vector2f(cellSize, cellSize));
      backgroundCell.setPosition(sf::Vector2f(gridOffset.x + x * cellSize, gridOffset.y + y * cellSize));

      // Subtle chessboard pattern
      if ((x + y) % 2 == 0) {
        backgroundCell.setFillColor(sf::Color(245, 245, 245));
      } else {
        backgroundCell.setFillColor(sf::Color(250, 250, 250));
      }

      window.draw(backgroundCell);
    }
  }

  // Draw the thick outer border
  sf::RectangleShape outerBorder(sf::Vector2f(gridWidth, gridHeight));
  outerBorder.setPosition(gridOffset);
  outerBorder.setOutlineThickness(3.0f);
  outerBorder.setOutlineColor(sf::Color(200, 200, 200));
  outerBorder.setFillColor(sf::Color::Transparent);
  window.draw(outerBorder);

  // Draw live cells with shell offset
  const float cellPadding = cellSize * 0.1f;
  const float liveCellSize = cellSize - (cellPadding * 2);

  for (unsigned int y = 0; y < height && y < GRID_HEIGHT; ++y) {
    for (unsigned int x = 0; x < width && x < GRID_WIDTH; ++x) {
      if (cells[y][x]) {
        sf::RectangleShape liveCell(sf::Vector2f(liveCellSize, liveCellSize));
        liveCell.setPosition(sf::Vector2f(gridOffset.x + x * cellSize + cellPadding,
                                         gridOffset.y + y * cellSize + cellPadding));
        liveCell.setFillColor(sf::Color::Black);
        window.draw(liveCell);
      }
    }
  }

  // Draw UI elements
  drawUI();
}

void GameOfLife::initializeButtons() {
  buttons.clear();
  sf::Vector2u windowSize = window.getSize();

  // Calculate button positions centered at the top
  float totalButtonWidth = (4 * BUTTON_WIDTH) + (3 * BUTTON_SPACING);
  float startX = (windowSize.x - totalButtonWidth) / 2.0f;
  float buttonY = MARGIN / 2.0f; // Position buttons higher up

  // Ensure startX fits within window bounds with some padding
  float minStartX = MARGIN / 4.0f;
  float maxStartX = windowSize.x - totalButtonWidth - MARGIN / 4.0f;
  startX = std::max(minStartX, std::min(startX, maxStartX));

  // Ensure buttonY fits within the margin area
  buttonY = std::max(MARGIN / 4.0f, std::min(buttonY, MARGIN - BUTTON_HEIGHT - MARGIN / 4.0f));

  buttons.emplace_back(isPaused ? "Resume" : "Pause", sf::Vector2f(startX, buttonY));
  buttons.emplace_back("Speed+", sf::Vector2f(startX + BUTTON_WIDTH + BUTTON_SPACING, buttonY));
  buttons.emplace_back("Speed-", sf::Vector2f(startX + 2 * (BUTTON_WIDTH + BUTTON_SPACING), buttonY));
  buttons.emplace_back("Random", sf::Vector2f(startX + 3 * (BUTTON_WIDTH + BUTTON_SPACING), buttonY));
}

void GameOfLife::drawUI() {
  for (const auto& button : buttons) {
    button.draw(window);
  }
}

void GameOfLife::handleButtonClick(sf::Vector2i mousePos) {
  for (size_t i = 0; i < buttons.size(); ++i) {
    if (buttons[i].contains(mousePos)) {
      switch (i) {
        case 0: // Pause/Resume
          isPaused = !isPaused;
          initializeButtons(); // Update button state and layout
          break;
        case 1: // Speed+
          if (!isPaused && timePerGeneration.asSeconds() > 0.1f) {
            timePerGeneration = sf::seconds(timePerGeneration.asSeconds() * 0.7f);
          }
          break;
        case 2: // Speed-
          if (!isPaused && timePerGeneration.asSeconds() < 3.0f) {
            timePerGeneration = sf::seconds(timePerGeneration.asSeconds() * 1.4f);
          }
          break;
        case 3: // Random
          seedRandomPattern();
          break;
      }
      break;
    }
  }
}

GameOfLife::Button::Button(const std::string& buttonLabel, sf::Vector2f pos)
    : label(buttonLabel), position(pos), isHovered(false),
      normalColor(220, 220, 220), hoverColor(240, 240, 240) {
  shape.setSize(sf::Vector2f(GameOfLife::BUTTON_WIDTH, GameOfLife::BUTTON_HEIGHT));
  shape.setPosition(position);
  shape.setFillColor(normalColor);
  shape.setOutlineThickness(2.0f);
  shape.setOutlineColor(sf::Color(180, 180, 180));
}

bool GameOfLife::Button::contains(sf::Vector2i point) const {
  return shape.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(point.x), static_cast<float>(point.y)));
}

void GameOfLife::Button::updateHover(sf::Vector2i mousePos) {
  isHovered = contains(mousePos);
  if (isHovered) {
    shape.setFillColor(hoverColor);
  } else {
    shape.setFillColor(normalColor);
  }
}

void GameOfLife::Button::draw(sf::RenderWindow& window) const {
  window.draw(shape);

  // Draw simple visual indicators for each button type
  sf::Vector2f center = position + sf::Vector2f(GameOfLife::BUTTON_WIDTH / 2.0f, GameOfLife::BUTTON_HEIGHT / 2.0f);

  if (label == "Pause" || label == "Resume") {
    if (label == "Pause") {
      // Draw pause symbol (two vertical bars)
      sf::RectangleShape bar1(sf::Vector2f(6, 16));
      sf::RectangleShape bar2(sf::Vector2f(6, 16));
      bar1.setPosition(sf::Vector2f(center.x - 10, center.y - 8));
      bar2.setPosition(sf::Vector2f(center.x + 4, center.y - 8));
      bar1.setFillColor(sf::Color::Black);
      bar2.setFillColor(sf::Color::Black);
      window.draw(bar1);
      window.draw(bar2);
    } else {
      // Draw play symbol (triangle)
      sf::CircleShape triangle(8, 3);
      triangle.setPosition(sf::Vector2f(center.x + 6, center.y - 8));
      triangle.setFillColor(sf::Color::Black);
      triangle.rotate(sf::degrees(90));
      window.draw(triangle);
    }
  } else if (label == "Speed+") {
    // Draw up arrow
    sf::CircleShape arrow(8, 3);
    arrow.setPosition(sf::Vector2f(center.x - 10, center.y - 6));
    arrow.setFillColor(sf::Color::Black);
    window.draw(arrow);
  } else if (label == "Speed-") {
    // Draw down arrow
    sf::CircleShape arrow(8, 3);
    arrow.setPosition(sf::Vector2f(center.x + 6, center.y + 6));
    arrow.setFillColor(sf::Color::Black);
    arrow.rotate(sf::degrees(180));
    window.draw(arrow);
  } else if (label == "Random") {
    // Draw random dots pattern
    for (int i = 0; i < 3; ++i) {
      sf::CircleShape dot(2);
      float x = center.x - 12 + i * 12.0f;
      float y = center.y;
      dot.setPosition(sf::Vector2f(x, y));
      dot.setFillColor(sf::Color::Black);
      window.draw(dot);
    }
  }
}
