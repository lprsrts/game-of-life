#ifndef GAMEOFLIFE_HPP
#define GAMEOFLIFE_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class GameOfLife {
public:
  const unsigned int WINDOW_WIDTH = 1080 * 16 / 9;
  const unsigned int WINDOW_HEIGHT = 1080;
  const unsigned int GRID_WIDTH = 60;
  const unsigned int GRID_HEIGHT = 40;
  static constexpr float MARGIN = 60.0f; // Constant offset from window edges
  static constexpr float BUTTON_HEIGHT = 40.0f;
  static constexpr float BUTTON_WIDTH = 100.0f;
  static constexpr float BUTTON_SPACING = 10.0f;

  GameOfLife();
  void run();
  void toggleCell(unsigned int x, unsigned int y);
  void nextGeneration();
  void draw();
  void seedRandomPattern(float density = 0.3f);
  void seedGliderPattern();
  void seedTestPattern();
  void clearGrid();

  struct Button {
    sf::RectangleShape shape;
    std::string label;
    sf::Vector2f position;
    bool isHovered;
    sf::Color normalColor;
    sf::Color hoverColor;

    Button(const std::string& buttonLabel, sf::Vector2f pos);
    bool contains(sf::Vector2i point) const;
    void updateHover(sf::Vector2i mousePos);
    void draw(sf::RenderWindow& window) const;
  };

private:
  sf::RenderWindow window;
  std::vector<std::vector<bool>> cells;
  unsigned int width, height;
  sf::Time timePerGeneration;
  sf::Clock clock;
  bool isPaused;

  // UI Buttons
  std::vector<Button> buttons;

  void processEvents();
  void update();
  std::pair<int, int> screenToGrid(const sf::Vector2i& screenPos);
  void initializeButtons();
  void drawUI();
  void handleButtonClick(sf::Vector2i mousePos);
  sf::Vector2f calculateGridOffset();
};

#endif // GAMEOFLIFE_HPP
