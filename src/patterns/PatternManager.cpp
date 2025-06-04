#include "PatternManager.hpp"
#include "../core/Grid.hpp"
#include <random>
#include <stdexcept>

Pattern::Pattern(const std::string& name, const std::string& desc,
                const std::vector<std::vector<bool>>& pattern)
    : name(name), description(desc), cells(pattern) {
    height = cells.size();
    width = height > 0 ? cells[0].size() : 0;
}

PatternManager::PatternManager() {
    initializeBuiltInPatterns();
}

void PatternManager::applyPattern(Grid& grid, const std::string& patternName) {
    if (patternName == "random") {
        applyRandomPattern(grid);
        return;
    }

    if (patternName == "clear") {
        clearGrid(grid);
        return;
    }

    if (!hasPattern(patternName)) {
        throw std::invalid_argument("Pattern not found: " + patternName);
    }

    clearGrid(grid);
    applyPatternCentered(grid, patternName);
}

void PatternManager::applyRandomPattern(Grid& grid, float density) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    clearGrid(grid);

    for (unsigned int y = 0; y < grid.getHeight(); ++y) {
        for (unsigned int x = 0; x < grid.getWidth(); ++x) {
            if (dis(gen) < density) {
                grid.setCell(x, y, true);
            }
        }
    }
}

void PatternManager::clearGrid(Grid& grid) {
    grid.clear();
}

void PatternManager::registerPattern(const std::string& name, const Pattern& pattern) {
    patterns.insert(std::make_pair(name, pattern));
}

void PatternManager::registerPattern(const std::string& name, const std::string& description,
                                   const std::vector<std::vector<bool>>& cells) {
    patterns.emplace(name, Pattern(name, description, cells));
}

bool PatternManager::hasPattern(const std::string& name) const {
    return patterns.find(name) != patterns.end();
}

std::vector<std::string> PatternManager::getPatternNames() const {
    std::vector<std::string> names;
    for (const auto& pair : patterns) {
        names.push_back(pair.first);
    }
    return names;
}

const Pattern& PatternManager::getPattern(const std::string& name) const {
    auto it = patterns.find(name);
    if (it == patterns.end()) {
        throw std::invalid_argument("Pattern not found: " + name);
    }
    return it->second;
}

void PatternManager::initializeBuiltInPatterns() {
    registerPattern("glider", createGliderPattern());
    registerPattern("beacon", createBeaconPattern());
    registerPattern("blinker", createBlinkerPattern());
    registerPattern("toad", createToadPattern());
    registerPattern("test", createTestPattern());
}

void PatternManager::applyPatternAt(Grid& grid, const std::string& patternName,
                                   unsigned int startX, unsigned int startY) {
    if (!hasPattern(patternName)) {
        throw std::invalid_argument("Pattern not found: " + patternName);
    }

    const Pattern& pattern = getPattern(patternName);

    if (!canFitPattern(grid, pattern, startX, startY)) {
        throw std::invalid_argument("Pattern does not fit at specified position");
    }

    placePattern(grid, pattern, startX, startY);
}

void PatternManager::applyPatternCentered(Grid& grid, const std::string& patternName) {
    if (!hasPattern(patternName)) {
        throw std::invalid_argument("Pattern not found: " + patternName);
    }

    const Pattern& pattern = getPattern(patternName);
    auto centerPos = calculateCenterPosition(grid, pattern);
    placePattern(grid, pattern, centerPos.first, centerPos.second);
}

Pattern PatternManager::createGliderPattern() {
    std::vector<std::vector<bool>> glider = {
        {false, true, false},
        {false, false, true},
        {true, true, true}
    };

    return Pattern("glider", "Classic glider pattern that moves diagonally", glider);
}

Pattern PatternManager::createBeaconPattern() {
    std::vector<std::vector<bool>> beacon = {
        {true, true, false, false},
        {true, true, false, false},
        {false, false, true, true},
        {false, false, true, true}
    };

    return Pattern("beacon", "Oscillating beacon pattern with period 2", beacon);
}

Pattern PatternManager::createBlinkerPattern() {
    std::vector<std::vector<bool>> blinker = {
        {true, true, true}
    };

    return Pattern("blinker", "Simple oscillating pattern with period 2", blinker);
}

Pattern PatternManager::createToadPattern() {
    std::vector<std::vector<bool>> toad = {
        {false, true, true, true},
        {true, true, true, false}
    };

    return Pattern("toad", "Oscillating toad pattern with period 2", toad);
}

Pattern PatternManager::createGliderGunPattern() {
    // This is a simplified version - the full Gosper glider gun is quite large
    std::vector<std::vector<bool>> gun(9, std::vector<bool>(36, false));

    // Left block
    gun[5][1] = gun[5][2] = gun[6][1] = gun[6][2] = true;

    // Left oscillator
    gun[3][13] = gun[3][14] = gun[4][12] = gun[4][16] = gun[5][11] = gun[5][17] = true;
    gun[6][11] = gun[6][15] = gun[6][17] = gun[6][18] = gun[7][11] = gun[7][17] = true;
    gun[8][12] = gun[8][16] = gun[9][13] = gun[9][14] = true;

    // Right block
    gun[3][25] = gun[4][23] = gun[4][25] = gun[5][21] = gun[5][22] = true;
    gun[6][21] = gun[6][22] = gun[7][21] = gun[7][22] = gun[8][23] = gun[8][25] = gun[9][25] = true;

    // Right block
    gun[1][35] = gun[1][36] = gun[2][35] = gun[2][36] = true;

    return Pattern("glider_gun", "Gosper glider gun - creates gliders", gun);
}

Pattern PatternManager::createTestPattern() {
    // Create a test pattern similar to the original seedTestPattern
    std::vector<std::vector<bool>> test(20, std::vector<bool>(20, false));

    // Corner markers
    test[0][0] = test[0][19] = test[19][0] = test[19][19] = true;

    // Center cross
    unsigned int centerX = 10;
    unsigned int centerY = 10;
    for (int i = -2; i <= 2; ++i) {
        if (centerX + i < 20) {
            test[centerY][centerX + i] = true;
        }
        if (centerY + i >= 0 && centerY + i < 20) {
            test[centerY + i][centerX] = true;
        }
    }

    // Border markers
    for (unsigned int x = 0; x < 20; x += 5) {
        test[0][x] = test[19][x] = true;
    }
    for (unsigned int y = 0; y < 20; y += 5) {
        test[y][0] = test[y][19] = true;
    }

    return Pattern("test", "Test pattern for coordinate verification", test);
}

bool PatternManager::canFitPattern(const Grid& grid, const Pattern& pattern,
                                  unsigned int startX, unsigned int startY) const {
    return (startX + pattern.width <= grid.getWidth() &&
            startY + pattern.height <= grid.getHeight());
}

std::pair<unsigned int, unsigned int> PatternManager::calculateCenterPosition(
    const Grid& grid, const Pattern& pattern) const {

    unsigned int centerX = (grid.getWidth() - pattern.width) / 2;
    unsigned int centerY = (grid.getHeight() - pattern.height) / 2;

    return std::make_pair(centerX, centerY);
}

void PatternManager::placePattern(Grid& grid, const Pattern& pattern,
                                 unsigned int startX, unsigned int startY) {
    for (unsigned int y = 0; y < pattern.height; ++y) {
        for (unsigned int x = 0; x < pattern.width; ++x) {
            if (pattern.cells[y][x]) {
                grid.setCell(startX + x, startY + y, true);
            }
        }
    }
}

bool PatternManager::loadPatternFromFile(const std::string& filename) {
    // TODO: Implement file loading functionality
    return false;
}

bool PatternManager::savePatternToFile(const std::string& patternName, const std::string& filename) const {
    // TODO: Implement file saving functionality
    return false;
}
