#include "LayoutHelper.h"
#include "SFML/Graphics.hpp"

namespace screen {

    void LayoutHelper::centerHorizontally(sf::Text& text, float windowWidth) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2.f, 0);
        text.setPosition(windowWidth / 2.f, text.getPosition().y);
    }

    void LayoutHelper::centerHorizontally(sf::RectangleShape& rect, float windowWidth) {
        sf::FloatRect bounds = rect.getLocalBounds();
        rect.setPosition((windowWidth - bounds.width) / 2.f, rect.getPosition().y);
    }

    void LayoutHelper::setRelativeY(sf::Text &text, float windowHeight, float relativeY) {
        text.setPosition(text.getPosition().x, windowHeight * relativeY);
    }

    void LayoutHelper::setRelativeY(sf::RectangleShape& rect, float windowHeight, float relativeY) {
        rect.setPosition(rect.getPosition().x, windowHeight * relativeY);
    }

    void LayoutHelper::positionAtBottom(sf::RectangleShape& rect, float windowHeight, float margin) {
        rect.setPosition(rect.getPosition().x, windowHeight - rect.getSize().y - margin);
    }

    void LayoutHelper::centerTextInRect(sf::Text &text, const sf::RectangleShape &rect) {
        sf::FloatRect textBounds = text.getLocalBounds();
        sf::FloatRect rectBounds = rect.getGlobalBounds();

        // Centrer horizontalement et verticalement
        text.setOrigin(textBounds.left + textBounds.width / 2.f,
                       textBounds.top + textBounds.height / 2.f);
        text.setPosition(
            rectBounds.left + rectBounds.width / 2.f,
            rectBounds.top + rectBounds.height / 2.f
        );
    }

    void LayoutHelper::positionTextRelativeToRect(sf::Text& text, sf::RectangleShape &rect,
                                           float offsetX, float offsetY) {
        sf::FloatRect rectBounds = rect.getGlobalBounds();
        text.setPosition(rectBounds.left + offsetX, rectBounds.top + offsetY);
    }

    void LayoutHelper::setRelativePosition(sf::Text& text, float windowWidth, float windowHeight,
                                    float relativeX, float relativeY) {
        text.setPosition(windowWidth * relativeX, windowHeight * relativeY);
    }

    void LayoutHelper::setRelativePosition(sf::RectangleShape& rect, float windowWidth, float windowHeight,
                                    float relativeX, float relativeY) {
        rect.setPosition(windowWidth * relativeX, windowHeight * relativeY);
    }
} // namespace screen
