#include "HomeScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "LayoutHelper.h"
#include "../../shared/utility/Constants.h"

namespace screen {
    HomeScreen::HomeScreen() = default;

    HomeScreen::~HomeScreen() = default;

    void HomeScreen::init(const std::shared_ptr<state::BloodBowlGame> &gamePtr, ResourceManager *res) {
        this->resources = res;
        // try to load default font from resources; fall back to system font if not found
        try {
            const sf::Font &f = resources->getFont("fonts/bloodbowl.ttf");
            font = f;
        } catch (...) {
            // leave font default if load fails
        }

        // Load background image
        if (!backgroundTexture.loadFromFile("res/home_display.jpg")) {
            // If loading fails, we'll just use a solid color background
        } else {
            backgroundSprite.setTexture(backgroundTexture);
            // Scale the sprite to fill the window
            float scaleX = static_cast<float>(utility::Constants::WINDOW_WIDTH) / backgroundTexture.getSize().x;
            float scaleY = static_cast<float>(utility::Constants::WINDOW_HEIGHT) / backgroundTexture.getSize().y;
            backgroundSprite.setScale(scaleX, scaleY);
        }

        setupUI();
    }

    void HomeScreen::setManager(SceneManager *mgr) { this->manager = mgr; }

    void HomeScreen::setupUI() {

        // Titre principal - centré en haut avec marge
        title.setFont(font);
        title.setString("BloodBowl");
        title.setCharacterSize(56);
        title.setFillColor(sf::Color::White);
        LayoutHelper::setRelativePosition(title, 0.5f, 0.12f);

        buttonTexts.clear();
        buttonRects.clear();

        std::vector<std::string> labels = {"Manage Teams", "Create Match", "Load Game", "Quit"};

        // Positions relatives pour les boutons (centrés verticalement)
        const float buttonWidth = 380.f;
        const float buttonHeight = 65.f;
        const float buttonSpacing = 0.11f; // 11% de la hauteur d'écran entre chaque bouton
        const float startY = 0.35f; // Commence à 35% de la hauteur

        for (size_t i = 0; i < labels.size(); ++i) {
            // Create button rectangle
            sf::RectangleShape rect({buttonWidth, buttonHeight});
            rect.setFillColor(sf::Color(80, 80, 100));
            rect.setOutlineColor(sf::Color(150, 150, 170));
            rect.setOutlineThickness(2);
            LayoutHelper::setRelativePosition(rect, 0.5f, startY + static_cast<float>(i) * buttonSpacing);
            buttonRects.push_back(rect);

            // Create button text - centré dans le bouton
            sf::Text text;
            text.setFont(font);
            text.setString(labels[i]);
            text.setCharacterSize(26);
            text.setFillColor(sf::Color::White);
            LayoutHelper::centerTextInRect(text, rect);
            buttonTexts.push_back(text);
        }
    }

    void HomeScreen::handleEvent(const sf::Event &event, sf::RenderWindow &window) {
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mpos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
            for (size_t i = 0; i < buttonRects.size(); ++i) {
                if (buttonRects[i].getGlobalBounds().contains(mpos)) {
                    switch (i) {
                        case 0: // Gérer équipes
                            if (manager) manager->switchTo(render::SceneId::TEAM_MANAGEMENT);
                            break;
                        case 1: // Créer match
                            if (manager) manager->switchTo(render::SceneId::MATCH_CREATION);
                            break;
                        case 2: // Charger partie
                            if (manager) manager->switchTo(render::SceneId::LOAD_GAME);
                            break;
                        case 3: // Quitter
                            window.close();
                            break;
                        default:
                            // no-op for safety
                            break;
                    }
                }
            }
        }
    }

    void HomeScreen::update() {
        // hover effect
        if (!resources) return;
    }

    void HomeScreen::draw(sf::RenderWindow &window) {
        // Draw background image if loaded, otherwise use gray
        if (backgroundTexture.getSize().x > 0) {
            window.draw(backgroundSprite);
        } else {
            window.clear(sf::Color(50, 50, 60));
        }

        window.draw(title);
        for (size_t i = 0; i < buttonRects.size(); ++i) {
            window.draw(buttonRects[i]);
            window.draw(buttonTexts[i]);
        }
    }

    render::SceneId HomeScreen::getId() const {
        return render::SceneId::HOME;
    }
} // namespace screen
