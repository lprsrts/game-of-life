/**
 * Conway's Game of Life - SFML Implementation
 *
 * This file implements a complete Game of Life simulation with:
 * - Responsive grid layout that maintains aspect ratios during window resize
 * - Interactive UI with clickable buttons for simulation control
 * - Mouse interaction for cell toggling and button clicking
 * - Multiple seeding patterns and keyboard shortcuts
 * - Visual enhancements with subtle grid patterns and proper spacing
 *
 * The grid is always centered in the window with constant margins,
 * ensuring the board never touches the window edges regardless of size.
 *
 * Controls:
 * - Mouse: Click cells to toggle, click buttons for controls
 * - Keyboard: SPACE (pause), R (random), G (glider), C (clear), T (test), +/- (speed)
 * - Buttons: Pause/Resume, Speed+, Speed-, Random pattern
 */

#include "GameOfLife.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <algorithm>
#include <random>

/**
 * Initialize the Game of Life simulation
 * Creates the SFML window, initializes the cell grid, and sets up the UI
 */
GameOfLife::GameOfLife()
    : window(
          sf::VideoMode({GameOfLife::WINDOW_WIDTH, GameOfLife::WINDOW_HEIGHT}),
          "Game of Life", sf::Style::Default),
      cells(GRID_WIDTH, std::vector<bool>(GRID_WIDTH, false)), width(GRID_WIDTH),
      height(GRID_HEIGHT), timePerGeneration(sf::seconds(1.f)), isPaused(false) {

  initializeButtons();
}

/**
 * Main game loop - runs until window is closed
 * Handles events, updates simulation state, and renders the current frame
 * The simulation advances to the next generation based on the current speed setting
 */
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

/**
 * Calculate where the grid should be positioned in the window
 * Ensures the grid is centered with proper margins and never touches window edges
 * Maintains 1:1 aspect ratio for cells and accounts for UI button space at the top
 *
 * @return Vector2f containing the top-left position of the grid
 */
sf::Vector2f GameOfLife::calculateGridOffset() {
  sf::Vector2u windowSize = window.getSize();

  // Calculate available space for the grid (minus margins and UI space)
  float availableWidth = windowSize.x - (2 * MARGIN);
  float availableHeight = windowSize.y - (2 * MARGIN) - BUTTON_HEIGHT - BUTTON_SPACING;

  // Ensure minimum dimensions to prevent division by zero or negative values
  availableWidth = std::max(availableWidth, 100.0f);
  availableHeight = std::max(availableHeight, 100.0f);

  // Calculate the maximum cell size while maintaining 1:1 ratio (square cells)
  float maxCellWidth = availableWidth / GRID_WIDTH;
  float maxCellHeight = availableHeight / GRID_HEIGHT;
  float cellSize = std::max(1.0f, std::min(maxCellWidth, maxCellHeight));

  // Calculate total grid dimensions based on cell size
  float gridWidth = cellSize * GRID_WIDTH;
  float gridHeight = cellSize * GRID_HEIGHT;

  // Center the grid in the available space (accounting for UI buttons at top)
  float offsetX = MARGIN + (availableWidth - gridWidth) / 2.0f;
  float offsetY = MARGIN + BUTTON_HEIGHT + BUTTON_SPACING + (availableHeight - gridHeight) / 2.0f;

  // Ensure offsets maintain minimum margins even in very small windows
  offsetX = std::max(MARGIN, offsetX);
  offsetY = std::max(MARGIN + BUTTON_HEIGHT + BUTTON_SPACING, offsetY);

  return sf::Vector2f(offsetX, offsetY);
}

/**
 * Convert screen pixel coordinates to grid cell coordinates
 * Used for mouse interaction to determine which cell was clicked
 *
 * @param screenPos Mouse position in window pixel coordinates
 * @return Pair of grid coordinates (x, y), or (-1, -1) if outside grid bounds
 */
std::pair<int, int> GameOfLife::screenToGrid(const sf::Vector2i &screenPos) {
  sf::Vector2f gridOffset = calculateGridOffset();
  sf::Vector2u windowSize = window.getSize();

  // Calculate cell size using the same method as grid positioning
  float availableWidth = windowSize.x - (2 * MARGIN);
  float availableHeight = windowSize.y - (2 * MARGIN) - BUTTON_HEIGHT - BUTTON_SPACING;

  // Ensure minimum dimensions to prevent division errors
  availableWidth = std::max(availableWidth, 100.0f);
  availableHeight = std::max(availableHeight, 100.0f);

  float maxCellWidth = availableWidth / GRID_WIDTH;
  float maxCellHeight = availableHeight / GRID_HEIGHT;
  float cellSize = std::max(1.0f, std::min(maxCellWidth, maxCellHeight));

  // Convert screen coordinates to grid coordinates
  int gridX = static_cast<int>((screenPos.x - gridOffset.x) / cellSize);
  int gridY = static_cast<int>((screenPos.y - gridOffset.y) / cellSize);

  // Return invalid coordinates if click is outside the grid bounds
  if (gridX < 0 || gridY < 0 || gridX >= static_cast<int>(GRID_WIDTH) || gridY >= static_cast<int>(GRID_HEIGHT)) {
    return std::make_pair(-1, -1);
  }

  return std::make_pair(gridX, gridY);
}

/**
 * Process all pending events from SFML window
 * Handles user input including window management, mouse interaction, and keyboard shortcuts
 * This method is called once per frame in the main game loop
 */
void GameOfLife::processEvents() {
  std::optional<sf::Event> event;
  while ((event = window.pollEvent())) {

    // Window close button clicked - exit the application
    if (event->is<sf::Event::Closed>()) {
      window.close();

    // Window was resized - update layout to maintain proper proportions
    } else if (event->is<sf::Event::Resized>()) {
      sf::Vector2u newSize = event->getIf<sf::Event::Resized>()->size;

      // Update the SFML view to match new window dimensions
      // This ensures coordinate system remains consistent after resize
      sf::View newView(sf::FloatRect({0.f, 0.f}, {static_cast<float>(newSize.x), static_cast<float>(newSize.y)}));
      window.setView(newView);

      // Recalculate button positions for the new window size
      initializeButtons();

    // Left mouse button pressed - handle button clicks and cell toggling
    } else if (event->is<sf::Event::MouseButtonPressed>()) {
      auto mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
      if (mouseEvent->button == sf::Mouse::Button::Left) {
        sf::Vector2i mousePos(mouseEvent->position.x, mouseEvent->position.y);

        // Priority: check if a UI button was clicked
        handleButtonClick(mousePos);

        // Second priority: check if a grid cell was clicked to toggle its state
        auto gridPos = screenToGrid(mousePos);
        if (gridPos.first >= 0 && gridPos.first < static_cast<int>(width) &&
            gridPos.second >= 0 && gridPos.second < static_cast<int>(height) &&
            gridPos.first < static_cast<int>(GRID_WIDTH) && gridPos.second < static_cast<int>(GRID_HEIGHT)) {
          toggleCell(gridPos.first, gridPos.second);
        }
      }

    // Mouse moved - update button hover states for visual feedback
    } else if (event->is<sf::Event::MouseMoved>()) {
      auto moveEvent = event->getIf<sf::Event::MouseMoved>();
      sf::Vector2i mousePos(moveEvent->position.x, moveEvent->position.y);
      for (auto& button : buttons) {
        button.updateHover(mousePos);
      }

    // Keyboard shortcuts for quick access to common functions
    } else if (event->is<sf::Event::KeyPressed>()) {
      auto keyEvent = event->getIf<sf::Event::KeyPressed>();
      switch (keyEvent->code) {
      case sf::Keyboard::Key::Space:
        // SPACE: Toggle simulation pause/resume
        isPaused = !isPaused;
        break;
      case sf::Keyboard::Key::R:
        // R: Generate a random pattern with 30% cell density
        seedRandomPattern();
        break;
      case sf::Keyboard::Key::G:
        // G: Create a classic glider pattern that moves across the grid
        seedGliderPattern();
        break;
      case sf::Keyboard::Key::C:
        // C: Clear all cells (set entire grid to dead state)
        clearGrid();
        break;
      case sf::Keyboard::Key::T:
        // T: Create a test pattern to verify the coordinate system after resize
        seedTestPattern();
        break;
      case sf::Keyboard::Key::Equal:
      case sf::Keyboard::Key::Add:
        // +/=: Increase simulation speed (only if not paused and not already at max speed)
        if (!isPaused && timePerGeneration.asSeconds() > 0.1f) {
          timePerGeneration = sf::seconds(timePerGeneration.asSeconds() * 0.8f);
        }
        break;
      case sf::Keyboard::Key::Hyphen:
      case sf::Keyboard::Key::Subtract:
        // -: Decrease simulation speed (only if not paused and not already at min speed)
        if (!isPaused && timePerGeneration.asSeconds() < 2.0f) {
          timePerGeneration =
              sf::seconds(timePerGeneration.asSeconds() * 1.25f);
        }
        break;
      default:
        // Ignore any other key presses
        break;
      }
    }
  }
}

/**
 * Toggle the state of a single cell between alive and dead
 * @param x Grid x-coordinate of the cell
 * @param y Grid y-coordinate of the cell
 */
void GameOfLife::toggleCell(unsigned int x, unsigned int y) {
  cells[y][x] = !cells[y][x];
}

/**
 * Advance the simulation by one generation using Conway's Game of Life rules:
 * 1. Live cells with 2-3 neighbors survive
 * 2. Dead cells with exactly 3 neighbors become alive
 * 3. All other cells die or remain dead
 */
void GameOfLife::nextGeneration() {
  // Create a new grid to store the next generation state
  std::vector<std::vector<bool>> nextCells(height, std::vector<bool>(width, false));

  // Process each cell in the grid to determine its next state
  for (unsigned int y = 0; y < height; ++y) {
    for (unsigned int x = 0; x < width; ++x) {
      int liveNeighbors = 0;

      // Count living neighbors in all 8 adjacent cells
      for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
          // Skip the center cell (not a neighbor of itself)
          if (dx == 0 && dy == 0) continue;

          int nx = static_cast<int>(x) + dx;
          int ny = static_cast<int>(y) + dy;

          // Only count neighbors that are within grid bounds
          if (nx >= 0 && nx < static_cast<int>(width) &&
              ny >= 0 && ny < static_cast<int>(height)) {
            if (cells[ny][nx]) {
              liveNeighbors++;
            }
          }
        }
      }

      // Apply Conway's Game of Life rules to determine next state
      if (cells[y][x]) {
        // Current cell is alive
        if (liveNeighbors == 2 || liveNeighbors == 3) {
          nextCells[y][x] = true; // Survives to next generation
        }
        // Otherwise dies from underpopulation (<2) or overpopulation (>3)
      } else {
        // Current cell is dead
        if (liveNeighbors == 3) {
          nextCells[y][x] = true; // Birth occurs with exactly 3 neighbors
        }
        // Otherwise remains dead
      }
    }
  }

  // Replace current generation with the newly calculated generation
  cells = std::move(nextCells);
}

/**
 * Clear all cells in the grid, setting them to dead state
 * Resets the entire simulation to an empty grid
 */
void GameOfLife::clearGrid() {
  for (unsigned int y = 0; y < height; ++y) {
    for (unsigned int x = 0; x < width; ++x) {
      cells[y][x] = false;
    }
  }
}

/**
 * Fill the grid with a random pattern of living cells
 * Each cell has a probability equal to the density parameter of being alive
 *
 * @param density Probability (0.0 to 1.0) of each cell being alive (default: 0.3 = 30%)
 */
void GameOfLife::seedRandomPattern(float density) {
  std::random_device rd;                                 // Hardware random seed
  std::mt19937 gen(rd());                               // Mersenne Twister random generator
  std::uniform_real_distribution<float> dis(0.0f, 1.0f); // Uniform distribution [0.0, 1.0]

  clearGrid();
  // Set each cell to alive if random number is less than density threshold
  for (unsigned int y = 0; y < height; ++y) {
    for (unsigned int x = 0; x < width; ++x) {
      cells[y][x] = dis(gen) < density;
    }
  }
}

/**
 * Create a classic "glider" pattern - a 5-cell pattern that moves diagonally
 * The glider is one of the most famous patterns in Conway's Game of Life
 * It travels one cell diagonally every 4 generations, creating perpetual motion
 *
 * Pattern layout:
 *   X     (. = dead, X = alive)
 *    X
 *  XXX
 */
void GameOfLife::seedGliderPattern() {
  clearGrid();

  // Only create glider if grid is large enough to contain the 3x3 pattern plus padding
  if (width >= 5 && height >= 5) {
    unsigned int startX = 2;  // Start position (leaves room for movement)
    unsigned int startY = 2;

    // Create the glider pattern manually
    cells[startY][startX + 1] = true;     // Top center
    cells[startY + 1][startX + 2] = true; // Middle right
    cells[startY + 2][startX] = true;     // Bottom left
    cells[startY + 2][startX + 1] = true; // Bottom center
    cells[startY + 2][startX + 2] = true; // Bottom right
  }
}

/**
 * Create a test pattern for verifying coordinate system accuracy
 * Useful for testing that mouse clicks register correctly after window resize
 * Creates visible markers at key grid positions to validate coordinate mapping
 *
 * Pattern includes:
 * - Corner markers at all four grid corners
 * - Cross pattern at the center of the grid
 * - Border outline with markers every 5 cells
 */
void GameOfLife::seedTestPattern() {
  clearGrid();

  // Only create test pattern if grid is large enough for meaningful testing
  if (width >= 10 && height >= 10) {

    // Place corner markers to verify grid boundaries
    cells[0][0] = true;                           // Top-left corner
    cells[0][width - 1] = true;                   // Top-right corner
    cells[height - 1][0] = true;                  // Bottom-left corner
    cells[height - 1][width - 1] = true;          // Bottom-right corner

    // Create center cross to verify grid center calculation
    unsigned int centerX = width / 2;
    unsigned int centerY = height / 2;
    for (int i = -2; i <= 2; ++i) {
      // Horizontal line through center
      if (centerX + i < width) {
        cells[centerY][centerX + i] = true;
      }
      // Vertical line through center
      if (centerY + i < height && centerY + i >= 0) {
        cells[centerY + i][centerX] = true;
      }
    }

    // Create border outline markers to verify edge detection
    for (unsigned int x = 0; x < width; x += 5) {
      cells[0][x] = true;                         // Top border markers
      cells[height - 1][x] = true;                // Bottom border markers
    }
    for (unsigned int y = 0; y < height; y += 5) {
      cells[y][0] = true;                         // Left border markers
      cells[y][width - 1] = true;                 // Right border markers
    }
  }
}

/**
 * Render the complete Game of Life visualization to the window
 *
 * This method performs the complete rendering pipeline:
 * 1. Calculate grid positioning and cell dimensions
 * 2. Draw subtle chessboard background pattern
 * 3. Draw grid border outline
 * 4. Render living cells with visual padding
 * 5. Draw user interface elements
 *
 * The grid maintains constant margins from window edges and automatically
 * scales to fit the window while preserving 1:1 cell aspect ratio.
 */
void GameOfLife::draw() {
  sf::Vector2u windowSize = window.getSize();
  sf::Vector2f gridOffset = calculateGridOffset();

  // Calculate cell dimensions using the same logic as coordinate conversion
  float availableWidth = windowSize.x - (2 * MARGIN);
  float availableHeight = windowSize.y - (2 * MARGIN) - BUTTON_HEIGHT - BUTTON_SPACING;

  // Prevent division by zero or negative dimensions in extreme window sizes
  availableWidth = std::max(availableWidth, 100.0f);
  availableHeight = std::max(availableHeight, 100.0f);

  // Calculate optimal cell size to fit grid while maintaining square cells
  float maxCellWidth = availableWidth / GRID_WIDTH;
  float maxCellHeight = availableHeight / GRID_HEIGHT;
  float cellSize = std::max(1.0f, std::min(maxCellWidth, maxCellHeight));

  // Calculate actual grid dimensions based on cell size
  float gridWidth = cellSize * GRID_WIDTH;
  float gridHeight = cellSize * GRID_HEIGHT;

  // STEP 1: Draw subtle chessboard background to visualize grid structure
  // This creates alternating light gray squares that help distinguish cells
  for (unsigned int y = 0; y < GRID_HEIGHT; ++y) {
    for (unsigned int x = 0; x < GRID_WIDTH; ++x) {
      sf::RectangleShape backgroundCell(sf::Vector2f(cellSize, cellSize));
      backgroundCell.setPosition(sf::Vector2f(gridOffset.x + x * cellSize, gridOffset.y + y * cellSize));

      // Alternate between two very similar light gray colors
      if ((x + y) % 2 == 0) {
        backgroundCell.setFillColor(sf::Color(245, 245, 245)); // Slightly darker
      } else {
        backgroundCell.setFillColor(sf::Color(250, 250, 250)); // Slightly lighter
      }

      window.draw(backgroundCell);
    }
  }

  // STEP 2: Draw thick border around the entire grid
  // This clearly defines the game area and adds visual polish
  sf::RectangleShape outerBorder(sf::Vector2f(gridWidth, gridHeight));
  outerBorder.setPosition(gridOffset);
  outerBorder.setOutlineThickness(3.0f);
  outerBorder.setOutlineColor(sf::Color(200, 200, 200)); // Medium gray border
  outerBorder.setFillColor(sf::Color::Transparent);       // Transparent fill
  window.draw(outerBorder);

  // STEP 3: Draw living cells with visual shell offset for clean appearance
  // Cells are rendered smaller than their grid squares to create separation
  const float cellPadding = cellSize * 0.1f;              // 10% padding around each cell
  const float liveCellSize = cellSize - (cellPadding * 2); // Cell size minus padding on all sides

  for (unsigned int y = 0; y < height && y < GRID_HEIGHT; ++y) {
    for (unsigned int x = 0; x < width && x < GRID_WIDTH; ++x) {
      if (cells[y][x]) {
        sf::RectangleShape liveCell(sf::Vector2f(liveCellSize, liveCellSize));
        // Position cell with padding offset to center it within its grid square
        liveCell.setPosition(sf::Vector2f(gridOffset.x + x * cellSize + cellPadding,
                                         gridOffset.y + y * cellSize + cellPadding));
        liveCell.setFillColor(sf::Color::Black); // Black cells for high contrast
        window.draw(liveCell);
      }
    }
  }

  // STEP 4: Draw user interface elements (buttons and controls)
  drawUI();
}

/**
 * Create and position UI control buttons for the current window size
 * Buttons are arranged horizontally at the top center of the window
 * This method is called during initialization and after window resize events
 */
void GameOfLife::initializeButtons() {
  buttons.clear();
  sf::Vector2u windowSize = window.getSize();

  // Calculate total width needed for all buttons including spacing
  float totalButtonWidth = (4 * BUTTON_WIDTH) + (3 * BUTTON_SPACING);
  float startX = (windowSize.x - totalButtonWidth) / 2.0f;
  float buttonY = MARGIN / 2.0f; // Position buttons in upper portion of top margin

  // Ensure buttons fit within window bounds even for very small windows
  float minStartX = MARGIN / 4.0f;
  float maxStartX = windowSize.x - totalButtonWidth - MARGIN / 4.0f;
  startX = std::max(minStartX, std::min(startX, maxStartX));

  // Keep buttons within the top margin area
  buttonY = std::max(MARGIN / 4.0f, std::min(buttonY, MARGIN - BUTTON_HEIGHT - MARGIN / 4.0f));

  // Create four control buttons with appropriate spacing
  buttons.emplace_back(isPaused ? "Resume" : "Pause", sf::Vector2f(startX, buttonY));
  buttons.emplace_back("Speed+", sf::Vector2f(startX + BUTTON_WIDTH + BUTTON_SPACING, buttonY));
  buttons.emplace_back("Speed-", sf::Vector2f(startX + 2 * (BUTTON_WIDTH + BUTTON_SPACING), buttonY));
  buttons.emplace_back("Random", sf::Vector2f(startX + 3 * (BUTTON_WIDTH + BUTTON_SPACING), buttonY));
}

/**
 * Render all user interface elements to the window
 * Currently handles drawing all control buttons with their visual indicators
 */
void GameOfLife::drawUI() {
  for (const auto& button : buttons) {
    button.draw(window);
  }
}

/**
 * Process mouse clicks on UI buttons and execute corresponding actions
 * Checks each button to see if the click position intersects with its bounds
 *
 * @param mousePos Mouse click position in window coordinates
 */
void GameOfLife::handleButtonClick(sf::Vector2i mousePos) {
  for (size_t i = 0; i < buttons.size(); ++i) {
    if (buttons[i].contains(mousePos)) {
      switch (i) {
        case 0: // Pause/Resume button
          isPaused = !isPaused;
          initializeButtons(); // Recreate buttons to update pause/resume label
          break;
        case 1: // Speed+ button
          // Increase speed only if simulation is running and not already at maximum
          if (!isPaused && timePerGeneration.asSeconds() > 0.1f) {
            timePerGeneration = sf::seconds(timePerGeneration.asSeconds() * 0.7f);
          }
          break;
        case 2: // Speed- button
          // Decrease speed only if simulation is running and not already at minimum
          if (!isPaused && timePerGeneration.asSeconds() < 3.0f) {
            timePerGeneration = sf::seconds(timePerGeneration.asSeconds() * 1.4f);
          }
          break;
        case 3: // Random pattern button
          seedRandomPattern();
          break;
      }
      break; // Exit loop after handling the clicked button
    }
  }
}

/**
 * Button constructor - Create a new interactive UI button
 * Initializes the button's visual appearance, position, and interaction state
 *
 * @param buttonLabel Text identifier for the button type (e.g., "Pause", "Speed+")
 * @param pos Position in window coordinates where the button should be placed
 */
GameOfLife::Button::Button(const std::string& buttonLabel, sf::Vector2f pos)
    : label(buttonLabel), position(pos), isHovered(false),
      normalColor(220, 220, 220), hoverColor(240, 240, 240) {
  // Set button dimensions using class constants
  shape.setSize(sf::Vector2f(GameOfLife::BUTTON_WIDTH, GameOfLife::BUTTON_HEIGHT));
  shape.setPosition(position);
  shape.setFillColor(normalColor);                    // Start with normal (unhovered) color
  shape.setOutlineThickness(2.0f);                   // Add subtle border
  shape.setOutlineColor(sf::Color(180, 180, 180));   // Medium gray border
}

/**
 * Check if a screen coordinate point is inside this button's bounds
 * Used for mouse click detection and hover state updates
 *
 * @param point Screen coordinates to test (typically mouse position)
 * @return true if the point is within the button's rectangular area
 */
bool GameOfLife::Button::contains(sf::Vector2i point) const {
  return shape.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(point.x), static_cast<float>(point.y)));
}

/**
 * Update the button's hover state based on current mouse position
 * Changes the button's visual appearance to provide user feedback
 * Called every frame when the mouse moves to update button appearance
 *
 * @param mousePos Current mouse position in window coordinates
 */
void GameOfLife::Button::updateHover(sf::Vector2i mousePos) {
  isHovered = contains(mousePos);
  if (isHovered) {
    shape.setFillColor(hoverColor);    // Lighter color when mouse is over button
  } else {
    shape.setFillColor(normalColor);   // Normal color when mouse is elsewhere
  }
}

/**
 * Render the button to the window with appropriate visual indicators
 * Draws the button background and a symbolic representation of its function
 * Since we don't use text rendering, each button type has a unique visual symbol
 *
 * Button symbols:
 * - Pause: Two vertical bars (||)
 * - Resume: Right-pointing triangle (▶)
 * - Speed+: Upward-pointing triangle (▲)
 * - Speed-: Downward-pointing triangle (▼)
 * - Random: 3x3 grid of dots (●●●)
 *
 * @param window SFML render window to draw the button on
 */
void GameOfLife::Button::draw(sf::RenderWindow& window) const {
  // Draw the button background rectangle (with current hover state color)
  window.draw(shape);

  // Calculate center point for positioning visual indicators
  // Use local copies to ensure coordinates aren't accidentally modified
  float centerX = position.x + GameOfLife::BUTTON_WIDTH / 2.0f;
  float centerY = position.y + GameOfLife::BUTTON_HEIGHT / 2.0f;

  // Draw different symbols based on button function
  if (label == "Pause" || label == "Resume") {
    if (label == "Pause") {
      // Draw pause symbol: two vertical bars (||)
      sf::RectangleShape bar1(sf::Vector2f(6, 16));
      sf::RectangleShape bar2(sf::Vector2f(6, 16));
      bar1.setPosition(sf::Vector2f(centerX - 10, centerY - 8));
      bar2.setPosition(sf::Vector2f(centerX + 4, centerY - 8));
      bar1.setFillColor(sf::Color::Black);
      bar2.setFillColor(sf::Color::Black);
      window.draw(bar1);
      window.draw(bar2);
    } else {
      // Draw play/resume symbol: right-pointing triangle (▶)
      sf::CircleShape triangle(8, 3);
      triangle.setPosition(sf::Vector2f(centerX - 6, centerY - 8));
      triangle.setFillColor(sf::Color::Black);
      triangle.rotate(sf::degrees(90));  // Rotate to point right
      window.draw(triangle);
    }
  } else if (label == "Speed+") {
    // Draw speed increase symbol: upward-pointing triangle (▲)
    sf::CircleShape arrow(8, 3);
    arrow.setPosition(sf::Vector2f(centerX - 8, centerY - 4));
    arrow.setFillColor(sf::Color::Black);
    window.draw(arrow);
  } else if (label == "Speed-") {
    // Draw speed decrease symbol: downward-pointing triangle (▼)
    sf::CircleShape arrow(8, 3);
    arrow.setPosition(sf::Vector2f(centerX - 8, centerY - 4));
    arrow.setFillColor(sf::Color::Black);
    arrow.rotate(sf::degrees(180));  // Rotate to point down
    window.draw(arrow);
  } else if (label == "Random") {
    // Draw random pattern symbol: 3x3 grid of dots (●●●)
    for (int i = 0; i < 9; ++i) {
      sf::CircleShape dot(2);
      float x = centerX - 12 + (i % 3) * 12.0f;        // Grid column position
      float y = centerY - 8 + (i / 3.0f) * 8.0f;       // Grid row position
      dot.setPosition(sf::Vector2f(x, y));
      dot.setFillColor(sf::Color::Black);
      window.draw(dot);
    }
  }
}
