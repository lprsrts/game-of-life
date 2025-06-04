#ifndef GAMEENGINE_HPP
#define GAMEENGINE_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <memory>

// Forward declarations
class Grid;
class Renderer;
class InputHandler;
class UIManager;
class PatternManager;

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    // Main game loop
    void run();

    // Game state control
    void pause();
    void resume();
    void togglePause();
    bool isPaused() const { return paused; }

    // Speed control
    void increaseSpeed();
    void decreaseSpeed();
    void setSpeed(float generationsPerSecond);
    float getSpeed() const;

    // Window management
    sf::RenderWindow& getWindow() { return window; }
    const sf::RenderWindow& getWindow() const { return window; }

    // Subsystem access
    Grid& getGrid() { return *grid; }
    UIManager& getUIManager() { return *uiManager; }
    PatternManager& getPatternManager() { return *patternManager; }

private:
    // Core systems
    sf::RenderWindow window;
    std::unique_ptr<Grid> grid;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<InputHandler> inputHandler;
    std::unique_ptr<UIManager> uiManager;
    std::unique_ptr<PatternManager> patternManager;

    // Game state
    bool paused;
    sf::Time timePerGeneration;
    sf::Clock clock;

    // Constants
    static constexpr unsigned int WINDOW_WIDTH = 1080 * 16 / 9;
    static constexpr unsigned int WINDOW_HEIGHT = 1080;
    static constexpr unsigned int GRID_WIDTH = 60;
    static constexpr unsigned int GRID_HEIGHT = 40;

    // Private methods
    void initialize();
    void update();
    void processEvents();
    void render();
    void cleanup();
};

#endif // GAMEENGINE_HPP
