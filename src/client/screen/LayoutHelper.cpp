#include "LayoutHelper.h"
#include "SFML/Graphics.hpp"
#include "utility/Constants.h"

namespace screen {

    void LayoutHelper::centerHorizontally(sf::Text& text) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        text.setPosition(utility::Constants::WINDOW_WIDTH / 2.f, text.getPosition().y);
    }

    void LayoutHelper::centerHorizontally(sf::RectangleShape& rect) {
        sf::FloatRect bounds = rect.getLocalBounds();
        rect.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        rect.setPosition(utility::Constants::WINDOW_WIDTH / 2.f, rect.getPosition().y);
    }

    void LayoutHelper::setRelativeY(sf::Text &text, float relativeY) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        text.setPosition(text.getPosition().x, utility::Constants::WINDOW_HEIGHT * relativeY);
    }

    void LayoutHelper::setRelativeY(sf::RectangleShape& rect, float relativeY) {
        sf::FloatRect bounds = rect.getLocalBounds();
        rect.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        rect.setPosition(rect.getPosition().x, utility::Constants::WINDOW_HEIGHT * relativeY);
    }

    void LayoutHelper::positionAtBottom(sf::RectangleShape& rect, float margin) {
        sf::FloatRect bounds = rect.getLocalBounds();
        rect.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        rect.setPosition(rect.getPosition().x, utility::Constants::WINDOW_HEIGHT - margin - bounds.height / 2.f);
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
        sf::FloatRect textBounds = text.getLocalBounds();
        sf::FloatRect rectBounds = rect.getGlobalBounds();
        text.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
        text.setPosition(rectBounds.left + offsetX + textBounds.width / 2.f,
                        rectBounds.top + offsetY + textBounds.height / 2.f);
    }

    void LayoutHelper::setRelativePosition(sf::Text& text, float relativeX, float relativeY) {
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        text.setPosition(utility::Constants::WINDOW_WIDTH * relativeX, utility::Constants::WINDOW_HEIGHT * relativeY);
    }

    void LayoutHelper::setRelativePosition(sf::RectangleShape& rect, float relativeX, float relativeY) {
        sf::FloatRect bounds = rect.getLocalBounds();
        rect.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        rect.setPosition(utility::Constants::WINDOW_WIDTH * relativeX, utility::Constants::WINDOW_HEIGHT * relativeY);
    }
} // namespace screen
