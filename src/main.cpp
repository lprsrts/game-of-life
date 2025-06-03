#include "GameOfLife.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System/Angle.hpp>
#include <iostream>

int main() {
  // Print controls to console
  std::cout << "=== Conway's Game of Life ===" << std::endl;
  std::cout << "Controls:" << std::endl;
  std::cout << "  Mouse:" << std::endl;
  std::cout << "    Left Click     - Toggle cell" << std::endl;
  std::cout << "    Button Bar     - Use top buttons for controls" << std::endl;
  std::cout << "  Keyboard:" << std::endl;
  std::cout << "    SPACE          - Pause/Resume" << std::endl;
  std::cout << "    R              - Random pattern" << std::endl;
  std::cout << "    G              - Glider pattern" << std::endl;
  std::cout << "    C              - Clear grid" << std::endl;
  std::cout << "    T              - Test pattern (for coordinate verification)" << std::endl;
  std::cout << "    +/=            - Speed up" << std::endl;
  std::cout << "    -              - Slow down" << std::endl;
  std::cout << "  Buttons (top bar):" << std::endl;
  std::cout << "    ||/▶           - Pause/Resume" << std::endl;
  std::cout << "    ▲              - Speed up" << std::endl;
  std::cout << "    ▼              - Speed down" << std::endl;
  std::cout << "    ●●●            - Random pattern" << std::endl;
  std::cout << "=============================================" << std::endl;
  
  GameOfLife game;
  
  // Start with a glider pattern
  game.seedGliderPattern();
  
  game.run();
  return 0;
}
