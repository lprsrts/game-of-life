/**
 * Conway's Game of Life - Main Entry Point
 *
 * This program implements Conway's Game of Life, a cellular automaton created by
 * mathematician John Conway in 1970. It's a zero-player game where the evolution
 * is determined by its initial state, requiring no further input.
 *
 * The Game of Life consists of a grid of cells which can be either alive or dead.
 * Each cell interacts with its eight neighbors according to these simple rules:
 *
 * 1. Any live cell with 2-3 live neighbors survives
 * 2. Any dead cell with exactly 3 live neighbors becomes alive
 * 3. All other live cells die, and all other dead cells stay dead
 *
 * Despite these simple rules, the Game of Life can create complex patterns,
 * including oscillators, gliders that move across the grid, and even patterns
 * that can simulate universal computation.
 *
 * This implementation features:
 * - Responsive grid that scales with window size while maintaining aspect ratio
 * - Interactive mouse controls for toggling individual cells
 * - Clickable UI buttons for common operations
 * - Keyboard shortcuts for quick access to functions
 * - Multiple preset patterns including the famous glider
 * - Adjustable simulation speed and pause functionality
 * - Visual enhancements with subtle grid patterns and proper spacing
 *
 * The program uses SFML (Simple and Fast Multimedia Library) for graphics,
 * window management, and event handling.
 */

#include "core/GameEngine.hpp"
#include "patterns/PatternManager.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System/Angle.hpp>
#include <iostream>

/**
 * Main function - Entry point for the Game of Life simulation
 *
 * Initializes the game, displays control instructions to the user,
 * sets up an initial pattern, and starts the main game loop.
 *
 * @return 0 on successful program completion
 */
int main() {
  // Display comprehensive control instructions to help new users
  std::cout << "=====================================================" << std::endl;
  std::cout << "           Conway's Game of Life Simulator          " << std::endl;
  std::cout << "=====================================================" << std::endl;
  std::cout << std::endl;
  std::cout << "Welcome to the Game of Life! This cellular automaton" << std::endl;
  std::cout << "evolves based on simple rules but can create complex" << std::endl;
  std::cout << "and beautiful patterns. Experiment with different" << std::endl;
  std::cout << "initial configurations and watch them evolve!" << std::endl;
  std::cout << std::endl;
  std::cout << "CONTROLS:" << std::endl;
  std::cout << "─────────────────────────────────────────────────────" << std::endl;
  std::cout << "  Mouse Interaction:" << std::endl;
  std::cout << "    • Left Click Cell    - Toggle alive/dead state" << std::endl;
  std::cout << "    • Click UI Buttons   - Use control bar at top" << std::endl;
  std::cout << std::endl;
  std::cout << "  Keyboard Shortcuts:" << std::endl;
  std::cout << "    • SPACE             - Pause/Resume simulation" << std::endl;
  std::cout << "    • R                 - Generate random pattern" << std::endl;
  std::cout << "    • G                 - Create glider pattern" << std::endl;
  std::cout << "    • C                 - Clear entire grid" << std::endl;
  std::cout << "    • T                 - Test pattern (for debugging)" << std::endl;
  std::cout << "    • + or =            - Increase simulation speed" << std::endl;
  std::cout << "    • -                 - Decrease simulation speed" << std::endl;
  std::cout << std::endl;
  std::cout << "  UI Buttons (top center):" << std::endl;
  std::cout << "    • ||/▶              - Pause/Resume simulation" << std::endl;
  std::cout << "    • ▲                 - Speed up simulation" << std::endl;
  std::cout << "    • ▼                 - Speed down simulation" << std::endl;
  std::cout << "    • ●●●               - Generate random pattern" << std::endl;
  std::cout << std::endl;
  std::cout << "TIP: Try the glider pattern (G) to see a pattern that" << std::endl;
  std::cout << "     moves across the grid, or random (R) for chaos!" << std::endl;
  std::cout << "=====================================================" << std::endl;

  // Create the Game of Life simulation with modular architecture
  // The game engine coordinates all subsystems: grid, renderer, input, UI, and patterns
  GameEngine engine;

  // Initialize with a classic glider pattern to demonstrate the Game of Life
  // The glider is a 5-cell pattern that travels diagonally across the grid,
  // moving one cell every 4 generations - a perfect introduction to the game
  engine.getPatternManager().applyPattern(engine.getGrid(), "glider");

  // Start the main game loop - this will run until the user closes the window
  // The loop handles events, updates the simulation state, and renders graphics
  engine.run();

  // Program completed successfully
  return 0;
}
