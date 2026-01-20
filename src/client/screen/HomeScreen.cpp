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
        setupUI();
    }

    void HomeScreen::setManager(SceneManager *mgr) { this->manager = mgr; }

    void HomeScreen::setupUI() {
        const float windowWidth = utility::Constants::WINDOW_WIDTH;
        const float windowHeight = utility::Constants::WINDOW_HEIGHT;

        // Titre principal - centré en haut
        title.setFont(font);
        title.setString("BloodBowl");
        title.setCharacterSize(48);
        title.setFillColor(sf::Color::White);
        LayoutHelper::setRelativeY(title, windowHeight, 0.05f);
        LayoutHelper::centerHorizontally(title, windowWidth);

        buttonTexts.clear();
        buttonRects.clear();

        std::vector<std::string> labels = {"Gerer equipes", "Creer match", "Charger partie", "Jeu en reseau", "Quitter"};

        // Positions relatives pour les boutons (centrés verticalement)
        const float buttonWidth = 300.f;
        const float buttonHeight = 50.f;
        const float buttonSpacing = 0.1f; // 10% de la hauteur d'écran entre chaque bouton
        const float startY = 0.25f; // Commence à 25% de la hauteur

        for (size_t i = 0; i < labels.size(); ++i) {
            // Create button rectangle
            sf::RectangleShape rect({buttonWidth, buttonHeight});
            rect.setFillColor(sf::Color(70, 70, 70));
            LayoutHelper::setRelativeY(rect, windowHeight, startY + i * buttonSpacing);
            LayoutHelper::centerHorizontally(rect, windowWidth);
            buttonRects.push_back(rect);

            // Create button text - centré dans le bouton
            sf::Text text;
            text.setFont(font);
            text.setString(labels[i]);
            text.setCharacterSize(20);
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
                        case 3: // Jeu en réseau
                            if (manager) manager->switchTo(render::SceneId::NETWORK);
                            break;
                        case 4: // Quitter
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
        // background
        window.clear(sf::Color(30, 30, 30));
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
