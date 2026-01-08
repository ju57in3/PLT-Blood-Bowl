#include "EndGameScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "LayoutHelper.h"
#include "../../shared/utility/Constants.h"

namespace screen {
    EndGameScreen::EndGameScreen() = default;

    EndGameScreen::~EndGameScreen() = default;

    void EndGameScreen::init(const std::shared_ptr<state::BloodBowlGame> &gamePtr, ResourceManager *resourcesPtr) {
        this->resources = resourcesPtr;
        const float windowWidth = utility::Constants::WINDOW_WIDTH;
        const float windowHeight = utility::Constants::WINDOW_HEIGHT;

        try { font = resources->getFont("fonts/bloodbowl.ttf"); } catch (...) {
        }

        // Titre principal - centré
        title.setFont(font);
        title.setString("Fin de la partie — Merci d'avoir joue !");
        title.setCharacterSize(36);
        title.setFillColor(sf::Color::White);
        LayoutHelper::setRelativeY(title, windowHeight, 0.05f);
        LayoutHelper::centerHorizontally(title, windowWidth);

        // Bouton retour au menu - en bas à gauche
        backButton.setSize({140, 40});
        backButton.setFillColor(sf::Color(70, 70, 70));
        backButton.setPosition(20, 0);
        LayoutHelper::positionAtBottom(backButton, windowHeight, 20);

        backText.setFont(font);
        backText.setString("Menu");
        backText.setCharacterSize(18);
        backText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(backText, backButton);
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
