#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

class Button {
public:
    Button(const std::string& label, sf::Vector2f position, sf::Vector2f size = sf::Vector2f(100.0f, 40.0f));
    
    // Interaction methods
    bool contains(sf::Vector2i point) const;
    void updateHover(sf::Vector2i mousePos);
    bool handleClick(sf::Vector2i mousePos);
    
    // Rendering
    void draw(sf::RenderWindow& window) const;
    
    // Properties
    void setPosition(sf::Vector2f position);
    void setSize(sf::Vector2f size);
    void setLabel(const std::string& label);
    void setColors(sf::Color normal, sf::Color hover);
    void setClickCallback(std::function<void()> callback);
    
    // Getters
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getSize() const { return size; }
    const std::string& getLabel() const { return label; }
    bool isHovered() const { return hovered; }

private:
    sf::RectangleShape shape;
    std::string label;
    sf::Vector2f position;
    sf::Vector2f size;
    bool hovered;
    sf::Color normalColor;
    sf::Color hoverColor;
    std::function<void()> clickCallback;
    
    // Helper methods
    void updateShape();
    void drawSymbol(sf::RenderWindow& window) const;
    void drawPauseSymbol(sf::RenderWindow& window, sf::Vector2f center) const;
    void drawPlaySymbol(sf::RenderWindow& window, sf::Vector2f center) const;
    void drawSpeedUpSymbol(sf::RenderWindow& window, sf::Vector2f center) const;
    void drawSpeedDownSymbol(sf::RenderWindow& window, sf::Vector2f center) const;
    void drawRandomSymbol(sf::RenderWindow& window, sf::Vector2f center) const;
};

#endif // BUTTON_HPP