#include "TeamCreationScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "LayoutHelper.h"
#include "../../shared/utility/Constants.h"

namespace screen {
    TeamCreationScreen::TeamCreationScreen() = default;

    TeamCreationScreen::~TeamCreationScreen() = default;

    void TeamCreationScreen::init(const std::shared_ptr<state::BloodBowlGame> &gamePtr, ResourceManager *resources) {
        this->resources = resources;
        const float windowWidth = utility::Constants::WINDOW_WIDTH;
        const float windowHeight = utility::Constants::WINDOW_HEIGHT;

        try { font = resources->getFont("fonts/bloodbowl.ttf"); } catch (...) {
        }

        // Titre principal - centré
        title.setFont(font);
        title.setString("Creation d'equipe");
        title.setCharacterSize(36);
        title.setFillColor(sf::Color::White);
        LayoutHelper::setRelativeY(title, windowHeight, 0.05f);
        LayoutHelper::centerHorizontally(title, windowWidth);

        // Sous-titre - centré
        subtitle.setFont(font);
        subtitle.setString("Construisez votre equipe");
        subtitle.setCharacterSize(36);
        subtitle.setFillColor(sf::Color::White);
        LayoutHelper::setRelativeY(subtitle, windowHeight, 0.12f);
        LayoutHelper::centerHorizontally(subtitle, windowWidth);

        // Bouton retour - en bas à gauche
        backButton.setSize({140, 40});
        backButton.setFillColor(sf::Color(70, 70, 70));
        backButton.setPosition(20, 0);
        LayoutHelper::positionAtBottom(backButton, windowHeight, 20);

        backText.setFont(font);
        backText.setString("Retour");
        backText.setCharacterSize(18);
        backText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(backText, backButton);
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
        window.draw(subtitle);
        window.draw(backButton);
        window.draw(backText);
    }

    render::SceneId TeamCreationScreen::getId() const {
        return render::SceneId::TEAM_CREATION;
    }
} // namespace screen
