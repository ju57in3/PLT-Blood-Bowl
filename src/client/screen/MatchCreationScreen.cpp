#include "MatchCreationScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "LayoutHelper.h"
#include "../../shared/utility/Constants.h"

namespace screen {
    MatchCreationScreen::MatchCreationScreen() = default;

    MatchCreationScreen::~MatchCreationScreen() = default;

    void MatchCreationScreen::init(const std::shared_ptr<state::BloodBowlGame> &gamePtr, ResourceManager *resources) {
        this->resources = resources;
        const float windowWidth = utility::Constants::WINDOW_WIDTH;
        const float windowHeight = utility::Constants::WINDOW_HEIGHT;

        try { font = resources->getFont("fonts/bloodbowl.ttf"); } catch (...) {
        }

        // Titre principal - centré
        title.setFont(font);
        title.setString("Creation de match");
        title.setCharacterSize(36);
        title.setFillColor(sf::Color::White);
        LayoutHelper::setRelativeY(title, windowHeight, 0.05f);
        LayoutHelper::centerHorizontally(title, windowWidth);

        // Sous-titre - centré
        subtitle.setFont(font);
        subtitle.setString("Choisissez les equipes et options");
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

        // Bouton démarrer - centré en bas
        startButton.setSize({200, 50});
        startButton.setFillColor(sf::Color(80, 120, 80));
        LayoutHelper::setRelativeY(startButton, windowHeight, 0.85f);
        LayoutHelper::centerHorizontally(startButton, windowWidth);

        startText.setFont(font);
        startText.setString("Demarrer le match");
        startText.setCharacterSize(18);
        startText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(startText, startButton);
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
        window.draw(subtitle);
        window.draw(backButton);
        window.draw(backText);
        window.draw(startButton);
        window.draw(startText);
    }

    render::SceneId MatchCreationScreen::getId() const {
        return render::SceneId::MATCH_CREATION;
    }
} // namespace screen
