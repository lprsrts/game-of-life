#include "UIManager.hpp"
#include "Button.hpp"
#include "../core/GameEngine.hpp"
#include "../patterns/PatternManager.hpp"
#include <algorithm>

UIManager::UIManager(GameEngine& gameEngine) : gameEngine(gameEngine) {
}

UIManager::~UIManager() {
    clearButtons();
}

void UIManager::initializeButtons() {
    clearButtons();
    
    sf::Vector2f startPos = calculateButtonStartPosition();
    
    createPauseButton(startPos);
    createSpeedUpButton(sf::Vector2f(startPos.x + BUTTON_WIDTH + BUTTON_SPACING, startPos.y));
    createSpeedDownButton(sf::Vector2f(startPos.x + 2 * (BUTTON_WIDTH + BUTTON_SPACING), startPos.y));
    createRandomButton(sf::Vector2f(startPos.x + 3 * (BUTTON_WIDTH + BUTTON_SPACING), startPos.y));
    createClearButton(sf::Vector2f(startPos.x + 4 * (BUTTON_WIDTH + BUTTON_SPACING), startPos.y));
}

void UIManager::update() {
    // Update button states if needed
    // This can be expanded for animations or state changes
}

void UIManager::draw(sf::RenderWindow& window) const {
    for (const auto& button : buttons) {
        button->draw(window);
    }
}

bool UIManager::handleClick(sf::Vector2i mousePos) {
    for (auto& button : buttons) {
        if (button->handleClick(mousePos)) {
            return true;
        }
    }
    return false;
}

void UIManager::updateHover(sf::Vector2i mousePos) {
    for (auto& button : buttons) {
        button->updateHover(mousePos);
    }
}

void UIManager::addButton(const std::string& label, sf::Vector2f position, std::function<void()> callback) {
    auto button = std::make_unique<Button>(label, position);
    button->setClickCallback(callback);
    buttons.push_back(std::move(button));
}

void UIManager::removeButton(size_t index) {
    if (index < buttons.size()) {
        buttons.erase(buttons.begin() + index);
    }
}

void UIManager::clearButtons() {
    buttons.clear();
}

void UIManager::updatePauseButton(bool isPaused) {
    if (!buttons.empty()) {
        buttons[0]->setLabel(isPaused ? "Resume" : "Pause");
    }
}

void UIManager::setSpeedDisplay(float speed) {
    // This could be expanded to show actual speed values
    // For now, we just update the buttons if needed
}

void UIManager::createPauseButton(sf::Vector2f position) {
    auto button = std::make_unique<Button>(gameEngine.isPaused() ? "Resume" : "Pause", position);
    button->setClickCallback([this]() { onPauseButtonClick(); });
    buttons.push_back(std::move(button));
}

void UIManager::createSpeedUpButton(sf::Vector2f position) {
    auto button = std::make_unique<Button>("Speed+", position);
    button->setClickCallback([this]() { onSpeedUpButtonClick(); });
    buttons.push_back(std::move(button));
}

void UIManager::createSpeedDownButton(sf::Vector2f position) {
    auto button = std::make_unique<Button>("Speed-", position);
    button->setClickCallback([this]() { onSpeedDownButtonClick(); });
    buttons.push_back(std::move(button));
}

void UIManager::createRandomButton(sf::Vector2f position) {
    auto button = std::make_unique<Button>("Random", position);
    button->setClickCallback([this]() { onRandomButtonClick(); });
    buttons.push_back(std::move(button));
}

void UIManager::createClearButton(sf::Vector2f position) {
    auto button = std::make_unique<Button>("Clear", position);
    button->setClickCallback([this]() { onClearButtonClick(); });
    buttons.push_back(std::move(button));
}

sf::Vector2f UIManager::calculateButtonStartPosition() const {
    sf::Vector2u windowSize = gameEngine.getWindow().getSize();
    
    float totalButtonWidth = (5 * BUTTON_WIDTH) + (4 * BUTTON_SPACING);
    float startX = (windowSize.x - totalButtonWidth) / 2.0f;
    float buttonY = MARGIN / 2.0f;
    
    float minStartX = MARGIN / 4.0f;
    float maxStartX = windowSize.x - totalButtonWidth - MARGIN / 4.0f;
    startX = std::max(minStartX, std::min(startX, maxStartX));
    
    buttonY = std::max(MARGIN / 4.0f, std::min(buttonY, MARGIN - BUTTON_HEIGHT - MARGIN / 4.0f));
    
    return sf::Vector2f(startX, buttonY);
}

void UIManager::repositionButtons() {
    if (buttons.empty()) return;
    
    sf::Vector2f startPos = calculateButtonStartPosition();
    
    for (size_t i = 0; i < buttons.size(); ++i) {
        sf::Vector2f newPos(startPos.x + i * (BUTTON_WIDTH + BUTTON_SPACING), startPos.y);
        buttons[i]->setPosition(newPos);
    }
}

void UIManager::onPauseButtonClick() {
    gameEngine.togglePause();
    updatePauseButton(gameEngine.isPaused());
}

void UIManager::onSpeedUpButtonClick() {
    gameEngine.increaseSpeed();
}

void UIManager::onSpeedDownButtonClick() {
    gameEngine.decreaseSpeed();
}

void UIManager::onRandomButtonClick() {
    gameEngine.getPatternManager().applyRandomPattern(gameEngine.getGrid());
}

void UIManager::onClearButtonClick() {
    gameEngine.getPatternManager().clearGrid(gameEngine.getGrid());
}