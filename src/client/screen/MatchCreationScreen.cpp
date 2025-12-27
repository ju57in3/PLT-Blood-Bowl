#include "MatchCreationScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include <iostream>
#include "../../shared/utility/Constants.h"

namespace screen {
    MatchCreationScreen::MatchCreationScreen() = default;

    MatchCreationScreen::~MatchCreationScreen() = default;

    void MatchCreationScreen::init(const std::shared_ptr<state::BloodBowlGame> &gamePtr, ResourceManager *resources) {
        this->resources = resources;
        try { font = resources->getFont("fonts/bloodbowl.ttf"); } catch (...) {
        }

        title.setFont(font);
        title.setString("Creation de match - Choisissez les equipes et options");
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

        // start button
        startButton.setSize({180, 50});
        startButton.setFillColor(sf::Color(80, 120, 80));
        startButton.setPosition(utility::Constants::WINDOW_WIDTH / 2 - 90, utility::Constants::WINDOW_HEIGHT - 120);
        startText.setFont(font);
        startText.setString("Demarrer le match");
        startText.setCharacterSize(18);
        startText.setFillColor(sf::Color::White);
        startText.setPosition(startButton.getPosition().x + 10, startButton.getPosition().y + 12);
    }

    void MatchCreationScreen::setManager(SceneManager *mgr) { manager = mgr; }

    void MatchCreationScreen::handleEvent(const sf::Event &event, sf::RenderWindow &window) {
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mpos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
            if (backButton.getGlobalBounds().contains(mpos)) {
                if (manager) manager->switchTo(render::SceneId::HOME);
            }
            if (startButton.getGlobalBounds().contains(mpos)) {
                if (manager) manager->switchTo(render::SceneId::GAME);
            }
        }
    }

    void MatchCreationScreen::update(float dt) {
    }

    void MatchCreationScreen::draw(sf::RenderWindow &window) {
        window.clear(sf::Color(25, 25, 40));
        window.draw(title);
        window.draw(backButton);
        window.draw(backText);
        window.draw(startButton);
        window.draw(startText);
    }

    render::SceneId MatchCreationScreen::getId() const {
        return render::SceneId::MATCH_CREATION;
    }
} // namespace screen
