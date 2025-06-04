#ifndef PATTERNMANAGER_HPP
#define PATTERNMANAGER_HPP

#include <string>
#include <vector>
#include <map>

// Forward declarations
class Grid;

struct Pattern {
    std::string name;
    std::string description;
    std::vector<std::vector<bool>> cells;
    unsigned int width;
    unsigned int height;

    Pattern(const std::string& name, const std::string& desc,
            const std::vector<std::vector<bool>>& pattern);
};

class PatternManager {
public:
    PatternManager();

    // Pattern application methods
    void applyPattern(Grid& grid, const std::string& patternName);
    void applyRandomPattern(Grid& grid, float density = 0.3f);
    void clearGrid(Grid& grid);

    // Pattern registration and management
    void registerPattern(const std::string& name, const Pattern& pattern);
    void registerPattern(const std::string& name, const std::string& description,
                        const std::vector<std::vector<bool>>& cells);
    bool hasPattern(const std::string& name) const;
    std::vector<std::string> getPatternNames() const;
    const Pattern& getPattern(const std::string& name) const;

    // Built-in patterns
    void initializeBuiltInPatterns();

    // Pattern positioning
    void applyPatternAt(Grid& grid, const std::string& patternName,
                       unsigned int startX, unsigned int startY);
    void applyPatternCentered(Grid& grid, const std::string& patternName);

    // Pattern file I/O (for future expansion)
    bool loadPatternFromFile(const std::string& filename);
    bool savePatternToFile(const std::string& patternName, const std::string& filename) const;

private:
    std::map<std::string, Pattern> patterns;

    // Built-in pattern creation methods
    Pattern createGliderPattern();
    Pattern createBeaconPattern();
    Pattern createBlinkerPattern();
    Pattern createToadPattern();
    Pattern createGliderGunPattern();
    Pattern createTestPattern();

    // Helper methods
    bool canFitPattern(const Grid& grid, const Pattern& pattern,
                      unsigned int startX, unsigned int startY) const;
    std::pair<unsigned int, unsigned int> calculateCenterPosition(
        const Grid& grid, const Pattern& pattern) const;
    void placePattern(Grid& grid, const Pattern& pattern,
                     unsigned int startX, unsigned int startY);
};

#endif // PATTERNMANAGER_HPP
