#include "Button.hpp"

Button::Button(const std::string& label, sf::Vector2f position, sf::Vector2f size)
    : label(label), position(position), size(size), hovered(false),
      normalColor(220, 220, 220), hoverColor(240, 240, 240) {
    updateShape();
}

bool Button::contains(sf::Vector2i point) const {
    return shape.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(point.x), static_cast<float>(point.y)));
}

void Button::updateHover(sf::Vector2i mousePos) {
    hovered = contains(mousePos);
    if (hovered) {
        shape.setFillColor(hoverColor);
    } else {
        shape.setFillColor(normalColor);
    }
}

bool Button::handleClick(sf::Vector2i mousePos) {
    if (contains(mousePos) && clickCallback) {
        clickCallback();
        return true;
    }
    return false;
}

void Button::draw(sf::RenderWindow& window) const {
    window.draw(shape);
    drawSymbol(window);
}

void Button::setPosition(sf::Vector2f newPosition) {
    position = newPosition;
    updateShape();
}

void Button::setSize(sf::Vector2f newSize) {
    size = newSize;
    updateShape();
}

void Button::setLabel(const std::string& newLabel) {
    label = newLabel;
}

void Button::setColors(sf::Color normal, sf::Color hover) {
    normalColor = normal;
    hoverColor = hover;
    shape.setFillColor(hovered ? hoverColor : normalColor);
}

void Button::setClickCallback(std::function<void()> callback) {
    clickCallback = callback;
}

void Button::updateShape() {
    shape.setSize(size);
    shape.setPosition(position);
    shape.setFillColor(hovered ? hoverColor : normalColor);
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color(180, 180, 180));
}

void Button::drawSymbol(sf::RenderWindow& window) const {
    sf::Vector2f center = sf::Vector2f(position.x + size.x / 2.0f, position.y + size.y / 2.0f);

    if (label == "Pause") {
        drawPauseSymbol(window, center);
    } else if (label == "Resume" || label == "Play") {
        drawPlaySymbol(window, center);
    } else if (label == "Speed+" || label == "Speed Up") {
        drawSpeedUpSymbol(window, center);
    } else if (label == "Speed-" || label == "Speed Down") {
        drawSpeedDownSymbol(window, center);
    } else if (label == "Random") {
        drawRandomSymbol(window, center);
    }
}

void Button::drawPauseSymbol(sf::RenderWindow& window, sf::Vector2f center) const {
    sf::RectangleShape bar1(sf::Vector2f(6, 16));
    sf::RectangleShape bar2(sf::Vector2f(6, 16));

    bar1.setPosition(sf::Vector2f(center.x - 10, center.y - 8));
    bar2.setPosition(sf::Vector2f(center.x + 4, center.y - 8));

    bar1.setFillColor(sf::Color::Black);
    bar2.setFillColor(sf::Color::Black);

    window.draw(bar1);
    window.draw(bar2);
}

void Button::drawPlaySymbol(sf::RenderWindow& window, sf::Vector2f center) const {
    sf::CircleShape triangle(8, 3);
    triangle.setPosition(sf::Vector2f(center.x + 6, center.y - 8));
    triangle.setFillColor(sf::Color::Black);
    triangle.rotate(sf::degrees(90));
    window.draw(triangle);
}

void Button::drawSpeedUpSymbol(sf::RenderWindow& window, sf::Vector2f center) const {
    sf::CircleShape arrow(8, 3);
    arrow.setPosition(sf::Vector2f(center.x - 8, center.y - 4));
    arrow.setFillColor(sf::Color::Black);
    window.draw(arrow);
}

void Button::drawSpeedDownSymbol(sf::RenderWindow& window, sf::Vector2f center) const {
    sf::CircleShape arrow(8, 3);
    arrow.setPosition(sf::Vector2f(center.x + 8, center.y + 6));
    arrow.setFillColor(sf::Color::Black);
    arrow.rotate(sf::degrees(180));
    window.draw(arrow);
}

void Button::drawRandomSymbol(sf::RenderWindow& window, sf::Vector2f center) const {
    for (int i = 0; i < 9; ++i) {
        sf::CircleShape dot(2);
        float x = center.x - 12 + (i % 3) * 12.0f;
        float y = center.y - 8 + (i / 3.0f) * 8.0f;
        dot.setPosition(sf::Vector2f(x, y));
        dot.setFillColor(sf::Color::Black);
        window.draw(dot);
    }
}
