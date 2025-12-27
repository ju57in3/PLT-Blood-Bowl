#include "HomeScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
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
        title.setFont(font);
        title.setString("BloodBowl");
        title.setCharacterSize(48);
        title.setFillColor(sf::Color::White);
        title.setPosition(utility::Constants::WINDOW_WIDTH * 0.5f - 120.0f, 30.0f);

        buttonTexts.clear();
        buttonRects.clear();

        std::vector<std::string> labels = {"Nouvelle equipe", "Creer match", "Quitter"};
        int y = 160;
        for (const auto &lbl: labels) {
            // Create button rectangle
            sf::RectangleShape rect({300, 50});
            rect.setFillColor(sf::Color(70, 70, 70));
            rect.setPosition(utility::Constants::WINDOW_WIDTH * 0.5f - 150.0f, static_cast<float>(y));
            buttonRects.push_back(rect);

            // Create button text
            sf::Text text;
            text.setFont(font);
            text.setString(lbl);
            text.setCharacterSize(20);
            text.setFillColor(sf::Color::White);
            text.setPosition(rect.getPosition().x + 20, rect.getPosition().y + 10);
            buttonTexts.push_back(text);

            y += 80;
        }
    }

    void HomeScreen::handleEvent(const sf::Event &event, sf::RenderWindow &window) {
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mpos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
            for (size_t i = 0; i < buttonRects.size(); ++i) {
                if (buttonRects[i].getGlobalBounds().contains(mpos)) {
                    switch (i) {
                        case 0: // Nouvelle équipe
                            if (manager) manager->switchTo(render::SceneId::TEAM_CREATION);
                            break;
                        case 1: // Créer match
                            if (manager) manager->switchTo(render::SceneId::MATCH_CREATION);
                            break;
                        case 2: // Quitter
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

    void HomeScreen::update(float dt) {
        // hover effect
        if (!resources) return;
    }

    void HomeScreen::draw(sf::RenderWindow &window) {
        // background
        window.clear(sf::Color(20, 20, 20));
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
