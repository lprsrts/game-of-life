#include "InputHandler.hpp"
#include "../core/GameEngine.hpp"
#include "../core/Grid.hpp"
#include "../ui/UIManager.hpp"
#include "../graphics/Renderer.hpp"

InputHandler::InputHandler(GameEngine& gameEngine)
    : gameEngine(gameEngine), isMousePressed(false) {
}

void InputHandler::processEvents() {
    sf::RenderWindow& window = gameEngine.getWindow();
    std::optional<sf::Event> event;
    
    while ((event = window.pollEvent())) {
        handleWindowEvents(*event);
        handleMouseEvents(*event);
        handleKeyboardEvents(*event);
    }
}

void InputHandler::setOnCellToggle(std::function<void(int, int)> callback) {
    onCellToggle = callback;
}

void InputHandler::setOnPauseToggle(std::function<void()> callback) {
    onPauseToggle = callback;
}

void InputHandler::setOnSpeedChange(std::function<void(bool)> callback) {
    onSpeedChange = callback;
}

void InputHandler::setOnPatternSeed(std::function<void(const std::string&)> callback) {
    onPatternSeed = callback;
}

void InputHandler::setOnGridClear(std::function<void()> callback) {
    onGridClear = callback;
}

void InputHandler::handleWindowEvents(const sf::Event& event) {
    if (event.is<sf::Event::Closed>()) {
        handleWindowClose();
    } else if (event.is<sf::Event::Resized>()) {
        auto resizeEvent = event.getIf<sf::Event::Resized>();
        handleWindowResize(resizeEvent->size);
    }
}

void InputHandler::handleMouseEvents(const sf::Event& event) {
    if (event.is<sf::Event::MouseButtonPressed>()) {
        auto mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2i mousePos(mouseEvent->position.x, mouseEvent->position.y);
            handleMouseClick(mousePos);
            isMousePressed = true;
        }
    } else if (event.is<sf::Event::MouseButtonReleased>()) {
        auto mouseEvent = event.getIf<sf::Event::MouseButtonReleased>();
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            isMousePressed = false;
        }
    } else if (event.is<sf::Event::MouseMoved>()) {
        auto moveEvent = event.getIf<sf::Event::MouseMoved>();
        sf::Vector2i mousePos(moveEvent->position.x, moveEvent->position.y);
        handleMouseMove(mousePos);
        lastMousePos = mousePos;
    }
}

void InputHandler::handleKeyboardEvents(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        auto keyEvent = event.getIf<sf::Event::KeyPressed>();
        handleKeyPress(keyEvent->code);
    }
}

void InputHandler::handleMouseClick(const sf::Vector2i& mousePos) {
    // First check if UI button was clicked
    UIManager& uiManager = gameEngine.getUIManager();
    if (uiManager.handleClick(mousePos)) {
        return; // UI handled the click
    }
    
    // Then check if grid cell was clicked
    // We need to get the renderer to convert screen to grid coordinates
    // For now, we'll implement a basic version - this should be improved
    // to use the Renderer's screenToGrid method
    if (onCellToggle) {
        // This is a simplified version - in a full implementation,
        // we'd get the Renderer instance and use its screenToGrid method
        Grid& grid = gameEngine.getGrid();
        
        // Basic coordinate conversion (this should use Renderer::screenToGrid)
        sf::RenderWindow& window = gameEngine.getWindow();
        sf::Vector2u windowSize = window.getSize();
        
        // Simplified calculation - should be moved to Renderer
        const float MARGIN = 60.0f;
        const float BUTTON_HEIGHT = 40.0f;
        const float BUTTON_SPACING = 10.0f;
        const unsigned int GRID_WIDTH = 60;
        const unsigned int GRID_HEIGHT = 40;
        
        float availableWidth = windowSize.x - (2 * MARGIN);
        float availableHeight = windowSize.y - (2 * MARGIN) - BUTTON_HEIGHT - BUTTON_SPACING;
        
        availableWidth = std::max(availableWidth, 100.0f);
        availableHeight = std::max(availableHeight, 100.0f);
        
        float maxCellWidth = availableWidth / GRID_WIDTH;
        float maxCellHeight = availableHeight / GRID_HEIGHT;
        float cellSize = std::max(1.0f, std::min(maxCellWidth, maxCellHeight));
        
        float gridWidth = cellSize * GRID_WIDTH;
        float gridHeight = cellSize * GRID_HEIGHT;
        
        float offsetX = MARGIN + (availableWidth - gridWidth) / 2.0f;
        float offsetY = MARGIN + BUTTON_HEIGHT + BUTTON_SPACING + (availableHeight - gridHeight) / 2.0f;
        
        offsetX = std::max(MARGIN, offsetX);
        offsetY = std::max(MARGIN + BUTTON_HEIGHT + BUTTON_SPACING, offsetY);
        
        int gridX = static_cast<int>((mousePos.x - offsetX) / cellSize);
        int gridY = static_cast<int>((mousePos.y - offsetY) / cellSize);
        
        if (gridX >= 0 && gridY >= 0 && gridX < static_cast<int>(GRID_WIDTH) && 
            gridY < static_cast<int>(GRID_HEIGHT)) {
            onCellToggle(gridX, gridY);
        }
    }
}

void InputHandler::handleMouseMove(const sf::Vector2i& mousePos) {
    UIManager& uiManager = gameEngine.getUIManager();
    uiManager.updateHover(mousePos);
}

void InputHandler::handleKeyPress(sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::Space:
            if (onPauseToggle) {
                onPauseToggle();
            }
            break;
            
        case sf::Keyboard::Key::R:
            if (onPatternSeed) {
                onPatternSeed("random");
            }
            break;
            
        case sf::Keyboard::Key::G:
            if (onPatternSeed) {
                onPatternSeed("glider");
            }
            break;
            
        case sf::Keyboard::Key::C:
            if (onGridClear) {
                onGridClear();
            }
            break;
            
        case sf::Keyboard::Key::T:
            if (onPatternSeed) {
                onPatternSeed("test");
            }
            break;
            
        case sf::Keyboard::Key::Equal:
        case sf::Keyboard::Key::Add:
            if (onSpeedChange) {
                onSpeedChange(true); // increase speed
            }
            break;
            
        case sf::Keyboard::Key::Hyphen:
        case sf::Keyboard::Key::Subtract:
            if (onSpeedChange) {
                onSpeedChange(false); // decrease speed
            }
            break;
            
        default:
            break;
    }
}

void InputHandler::handleWindowResize(const sf::Vector2u& newSize) {
    sf::RenderWindow& window = gameEngine.getWindow();
    sf::View newView(sf::FloatRect({0.f, 0.f}, {static_cast<float>(newSize.x), static_cast<float>(newSize.y)}));
    window.setView(newView);
    
    // Notify UI manager to reinitialize buttons
    UIManager& uiManager = gameEngine.getUIManager();
    uiManager.initializeButtons();
}

void InputHandler::handleWindowClose() {
    sf::RenderWindow& window = gameEngine.getWindow();
    window.close();
}