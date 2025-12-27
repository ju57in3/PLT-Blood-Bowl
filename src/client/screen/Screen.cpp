#include "Screen.h"

namespace screen {
    Screen::~Screen() = default;

    void Screen::draw(sf::RenderWindow &window) {
    }

    render::SceneId Screen::getId() const {
        return render::SceneId::HOME; // Default implementation
    }

    void Screen::init(const std::shared_ptr<state::BloodBowlGame> &game, ResourceManager *resources) {
    }


    void Screen::update(float dt) {
    }

    void Screen::setManager(SceneManager *mgr) {
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
