#include "EndGameScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "../../shared/utility/Constants.h"

namespace screen {
    EndGameScreen::EndGameScreen() = default;

    EndGameScreen::~EndGameScreen() = default;

    void EndGameScreen::init(const std::shared_ptr<state::BloodBowlGame> &gamePtr, ResourceManager *resourcesPtr) {
        this->resources = resourcesPtr;
        try { font = resources->getFont("fonts/bloodbowl.ttf"); } catch (...) {
        }
        title.setFont(font);
        title.setString("Fin de la partie — Merci d'avoir joue !");
        title.setCharacterSize(36);
        title.setFillColor(sf::Color::White);
        title.setPosition(static_cast<float>(utility::Constants::WINDOW_WIDTH) / 2.0f - 160.0f, 40.0f);

        backButton.setSize({140, 40});
        backButton.setFillColor(sf::Color(70, 70, 70));
        backButton.setPosition(20.0f, static_cast<float>(utility::Constants::WINDOW_HEIGHT) - 60.0f);
        backText.setFont(font);
        backText.setString("Menu");
        backText.setCharacterSize(18);
        backText.setFillColor(sf::Color::White);
        backText.setPosition(30.0f, static_cast<float>(utility::Constants::WINDOW_HEIGHT) - 55.0f);
    }

    void EndGameScreen::setManager(SceneManager *mgr) { manager = mgr; }

    void EndGameScreen::handleEvent(const sf::Event &event, sf::RenderWindow &window) {
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mpos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
            if (backButton.getGlobalBounds().contains(mpos)) {
                if (manager) manager->switchTo(render::SceneId::HOME);
            }
        }
    }

    void EndGameScreen::update(float dt) {
    }

    void EndGameScreen::draw(sf::RenderWindow &window) {
        window.clear(sf::Color(40, 20, 20));
        window.draw(title);
        window.draw(backButton);
        window.draw(backText);
    }

    render::SceneId EndGameScreen::getId() const {
        return render::SceneId::END_GAME;
    }
} // namespace screen
