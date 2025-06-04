#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include <SFML/Graphics.hpp>
#include <functional>

// Forward declarations
class GameEngine;
class Grid;
class UIManager;
class Renderer;

class InputHandler {
public:
    InputHandler(GameEngine& gameEngine);
    
    // Main event processing
    void processEvents();
    
    // Event handler registration
    void setOnCellToggle(std::function<void(int, int)> callback);
    void setOnPauseToggle(std::function<void()> callback);
    void setOnSpeedChange(std::function<void(bool)> callback);
    void setOnPatternSeed(std::function<void(const std::string&)> callback);
    void setOnGridClear(std::function<void()> callback);

private:
    GameEngine& gameEngine;
    
    // Event callbacks
    std::function<void(int, int)> onCellToggle;
    std::function<void()> onPauseToggle;
    std::function<void(bool)> onSpeedChange; // true = increase, false = decrease
    std::function<void(const std::string&)> onPatternSeed;
    std::function<void()> onGridClear;
    
    // Event processing methods
    void handleWindowEvents(const sf::Event& event);
    void handleMouseEvents(const sf::Event& event);
    void handleKeyboardEvents(const sf::Event& event);
    
    // Helper methods
    void handleMouseClick(const sf::Vector2i& mousePos);
    void handleMouseMove(const sf::Vector2i& mousePos);
    void handleKeyPress(sf::Keyboard::Key key);
    void handleWindowResize(const sf::Vector2u& newSize);
    void handleWindowClose();
    
    // Input state
    sf::Vector2i lastMousePos;
    bool isMousePressed;
};

#endif // INPUTHANDLER_HPP