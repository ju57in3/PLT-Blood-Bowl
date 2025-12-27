#include "Screen.h"
#include <iostream>

namespace screen {
    Screen::~Screen() = default;

    void Screen::draw(sf::RenderWindow &window) {
        // Default empty implementation - should be overridden by subclasses
    }

    render::SceneId Screen::getId() const {
        // Returning HOME as fallback, but this indicates a bug if called.
        std::cerr << "WARNING: Screen::getId() not overridden in subclass!\n";
        return render::SceneId::HOME;
    }

    void Screen::init(const std::shared_ptr<state::BloodBowlGame> &game, ResourceManager *resources) {
        // Default empty implementation - can be overridden by subclasses if needed
    }


    void Screen::update(float dt) {
        // Default empty implementation - can be overridden by subclasses if needed
    }

    void Screen::setManager(SceneManager *mgr) {
        // Default empty implementation - subclasses that need manager access should override this
        // and store the pointer in their own member variable
    }


    void Screen::handleEvent(const sf::Event &event, sf::RenderWindow &window) {
    }

    void Screen::onEnter() {
        // Default implementation - can be overridden
    }

    void Screen::onExit() {
        // Default implementation - can be overridden
    }
} // namespace screen
