#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <functional>

// Forward declarations
class Button;
class GameEngine;

class UIManager {
public:
    UIManager(GameEngine& gameEngine);
    ~UIManager();
    
    // UI lifecycle methods
    void initializeButtons();
    void update();
    void draw(sf::RenderWindow& window) const;
    
    // Input handling
    bool handleClick(sf::Vector2i mousePos);
    void updateHover(sf::Vector2i mousePos);
    
    // Button management
    void addButton(const std::string& label, sf::Vector2f position, std::function<void()> callback);
    void removeButton(size_t index);
    void clearButtons();
    
    // UI state
    void updatePauseButton(bool isPaused);
    void setSpeedDisplay(float speed);
    
    // Layout constants
    static constexpr float BUTTON_WIDTH = 100.0f;
    static constexpr float BUTTON_HEIGHT = 40.0f;
    static constexpr float BUTTON_SPACING = 10.0f;
    static constexpr float MARGIN = 60.0f;

private:
    GameEngine& gameEngine;
    std::vector<std::unique_ptr<Button>> buttons;
    
    // Button creation helpers
    void createPauseButton(sf::Vector2f position);
    void createSpeedUpButton(sf::Vector2f position);
    void createSpeedDownButton(sf::Vector2f position);
    void createRandomButton(sf::Vector2f position);
    void createClearButton(sf::Vector2f position);
    
    // Layout calculation
    sf::Vector2f calculateButtonStartPosition() const;
    void repositionButtons();
    
    // Button callbacks
    void onPauseButtonClick();
    void onSpeedUpButtonClick();
    void onSpeedDownButtonClick();
    void onRandomButtonClick();
    void onClearButtonClick();
};

#endif // UIMANAGER_HPP