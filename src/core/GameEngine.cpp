#include "GameEngine.hpp"
#include "Grid.hpp"
#include "../graphics/Renderer.hpp"
#include "../input/InputHandler.hpp"
#include "../ui/UIManager.hpp"
#include "../patterns/PatternManager.hpp"

GameEngine::GameEngine() 
    : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Conway's Game of Life", sf::Style::Default),
      paused(false),
      timePerGeneration(sf::seconds(1.0f)) {
    
    initialize();
}

GameEngine::~GameEngine() {
    cleanup();
}

void GameEngine::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void GameEngine::pause() {
    paused = true;
    uiManager->updatePauseButton(paused);
}

void GameEngine::resume() {
    paused = false;
    clock.restart();
    uiManager->updatePauseButton(paused);
}

void GameEngine::togglePause() {
    if (paused) {
        resume();
    } else {
        pause();
    }
}

void GameEngine::increaseSpeed() {
    if (!paused && timePerGeneration.asSeconds() > 0.1f) {
        timePerGeneration = sf::seconds(timePerGeneration.asSeconds() * 0.7f);
        uiManager->setSpeedDisplay(getSpeed());
    }
}

void GameEngine::decreaseSpeed() {
    if (!paused && timePerGeneration.asSeconds() < 3.0f) {
        timePerGeneration = sf::seconds(timePerGeneration.asSeconds() * 1.4f);
        uiManager->setSpeedDisplay(getSpeed());
    }
}

void GameEngine::setSpeed(float generationsPerSecond) {
    if (generationsPerSecond > 0.0f) {
        timePerGeneration = sf::seconds(1.0f / generationsPerSecond);
        uiManager->setSpeedDisplay(getSpeed());
    }
}

float GameEngine::getSpeed() const {
    return 1.0f / timePerGeneration.asSeconds();
}

void GameEngine::initialize() {
    // Create subsystems
    grid = std::make_unique<Grid>(GRID_WIDTH, GRID_HEIGHT);
    renderer = std::make_unique<Renderer>(window);
    uiManager = std::make_unique<UIManager>(*this);
    patternManager = std::make_unique<PatternManager>();
    inputHandler = std::make_unique<InputHandler>(*this);
    
    // Set up input callbacks
    inputHandler->setOnCellToggle([this](int x, int y) {
        grid->toggleCell(x, y);
    });
    
    inputHandler->setOnPauseToggle([this]() {
        togglePause();
    });
    
    inputHandler->setOnSpeedChange([this](bool increase) {
        if (increase) {
            increaseSpeed();
        } else {
            decreaseSpeed();
        }
    });
    
    inputHandler->setOnPatternSeed([this](const std::string& patternName) {
        patternManager->applyPattern(*grid, patternName);
    });
    
    inputHandler->setOnGridClear([this]() {
        patternManager->clearGrid(*grid);
    });
    
    // Initialize UI
    uiManager->initializeButtons();
    
    // Start paused to allow user to set up initial pattern
    paused = true;
}

void GameEngine::update() {
    if (!paused && clock.getElapsedTime() >= timePerGeneration) {
        grid->nextGeneration();
        clock.restart();
    }
    
    uiManager->update();
}

void GameEngine::processEvents() {
    inputHandler->processEvents();
}

void GameEngine::render() {
    renderer->render(*grid, *uiManager);
}

void GameEngine::cleanup() {
    // Unique pointers will automatically clean up
}