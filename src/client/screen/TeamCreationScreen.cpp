#include "TeamCreationScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include <iostream>
#include "../../shared/utility/Constants.h"

namespace screen {
    TeamCreationScreen::TeamCreationScreen() = default;

    TeamCreationScreen::~TeamCreationScreen() = default;

    void TeamCreationScreen::init(const std::shared_ptr<state::BloodBowlGame> &gamePtr, ResourceManager *resources) {
        this->resources = resources;
        try { font = resources->getFont("fonts/bloodbowl.ttf"); } catch (...) {
        }
        title.setFont(font);
        title.setString("Creation d'equipe - Construisez votre equipe");
        title.setCharacterSize(36);
        title.setFillColor(sf::Color::White);
        title.setPosition(utility::Constants::WINDOW_WIDTH / 2 - 160, 40);

        backButton.setSize({140, 40});
        backButton.setFillColor(sf::Color(70, 70, 70));
        backButton.setPosition(20, utility::Constants::WINDOW_HEIGHT - 60);
        backText.setFont(font);
        backText.setString("Retour");
        backText.setCharacterSize(18);
        backText.setFillColor(sf::Color::White);
        backText.setPosition(30, utility::Constants::WINDOW_HEIGHT - 55);
    }

    void TeamCreationScreen::setManager(SceneManager *mgr) { manager = mgr; }

    void TeamCreationScreen::handleEvent(const sf::Event &event, sf::RenderWindow &window) {
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mpos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
            if (backButton.getGlobalBounds().contains(mpos)) {
                if (manager) manager->switchTo(render::SceneId::HOME);
            }
        }
    }

    void TeamCreationScreen::update(float dt) {
    }

    void TeamCreationScreen::draw(sf::RenderWindow &window) {
        window.clear(sf::Color(30, 30, 30));
        window.draw(title);
        window.draw(backButton);
        window.draw(backText);
    }

    render::SceneId TeamCreationScreen::getId() const {
        return render::SceneId::TEAM_CREATION;
    }
} // namespace screen
