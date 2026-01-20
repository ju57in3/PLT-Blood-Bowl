#include "PauseScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "LayoutHelper.h"
#include "../../shared/utility/Constants.h"
#include "../../shared/state/GameStateManager.h"
#include "../../shared/state/TeamManager.h"
#include <iostream>

namespace screen {

    PauseScreen::PauseScreen() = default;

    PauseScreen::~PauseScreen() = default;

    void PauseScreen::init(const std::shared_ptr<state::BloodBowlGame>& gamePtr, ResourceManager* resourcesPtr) {
        this->resources = resourcesPtr;

        try {
            font = resources->getFont("fonts/bloodbowl.ttf");
        } catch (...) {
            std::cerr << "Failed to load font" << std::endl;
        }

        // Titre
        title.setFont(font);
        title.setString("PAUSE");
        title.setCharacterSize(48);
        title.setFillColor(sf::Color::White);
        title.setStyle(sf::Text::Bold);
        LayoutHelper::setRelativeY(title, 0.15f);
        LayoutHelper::centerHorizontally(title);

        // Bouton Reprendre
        const float buttonWidth = 375.0f;
        const float buttonHeight = 50.0f;

        resumeButton.setSize({buttonWidth, buttonHeight});
        resumeButton.setFillColor(sf::Color(50, 100, 200));
        resumeButton.setOutlineColor(sf::Color::White);
        resumeButton.setOutlineThickness(2);
        LayoutHelper::setRelativeY(resumeButton, 0.35f);
        LayoutHelper::centerHorizontally(resumeButton);

        resumeText.setFont(font);
        resumeText.setString("Reprendre la partie");
        resumeText.setCharacterSize(24);
        resumeText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(resumeText, resumeButton);

        // Bouton Sauvegarder
        saveButton.setSize({buttonWidth, buttonHeight});
        saveButton.setFillColor(sf::Color(50, 150, 50));
        saveButton.setOutlineColor(sf::Color::White);
        saveButton.setOutlineThickness(2);
        LayoutHelper::setRelativeY(saveButton, 0.50f);
        LayoutHelper::centerHorizontally(saveButton);

        saveText.setFont(font);
        saveText.setString("Sauvegarder et quitter");
        saveText.setCharacterSize(24);
        saveText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(saveText, saveButton);

        // Bouton Quitter sans sauvegarder
        quitButton.setSize({buttonWidth, buttonHeight});
        quitButton.setFillColor(sf::Color(150, 50, 50));
        quitButton.setOutlineColor(sf::Color::White);
        quitButton.setOutlineThickness(2);
        LayoutHelper::setRelativeY(quitButton, 0.65f);
        LayoutHelper::centerHorizontally(quitButton);

        quitText.setFont(font);
        quitText.setString("Quitter sans sauvegarder");
        quitText.setCharacterSize(24);
        quitText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(quitText, quitButton);
    }

    void PauseScreen::setManager(SceneManager* mgr) {
        manager = mgr;
    }

    void PauseScreen::onEnter() {
        // Rien de spécial à faire
    }

    void PauseScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

            if (resumeButton.getGlobalBounds().contains(mousePos)) {
                resumeGame();
            } else if (saveButton.getGlobalBounds().contains(mousePos)) {
                saveAndQuit();
            } else if (quitButton.getGlobalBounds().contains(mousePos)) {
                quitWithoutSaving();
            }
        }

        // Échap pour reprendre la partie
        if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape) {
            resumeGame();
        }
    }

    void PauseScreen::update() {
        // Rien à mettre à jour
    }

    void PauseScreen::draw(sf::RenderWindow& window) {
        window.clear(sf::Color(50, 50, 60)); // Uniform gray background

        window.draw(title);
        window.draw(resumeButton);
        window.draw(resumeText);
        window.draw(saveButton);
        window.draw(saveText);
        window.draw(quitButton);
        window.draw(quitText);
    }

    render::SceneId PauseScreen::getId() const {
        return render::SceneId::PAUSE;
    }

    void PauseScreen::resumeGame() {
        std::cout << "[PAUSE] Reprendre la partie" << std::endl;
        if (manager) {
            manager->pop(); // Retourne au GameScreen
        }
    }

    void PauseScreen::saveAndQuit() {
        std::cout << "[PAUSE] Sauvegarder et quitter" << std::endl;

        // Récupérer le jeu depuis le SceneManager
        auto game = manager->getGame();
        if (game) {
            auto& gameStateManager = state::GameStateManager::getInstance();
            std::string saveName = "QuickSave_" + game->getTeamA().getName() + "_vs_" + game->getTeamB().getName();
            gameStateManager.saveGame(*game, saveName);
            std::cout << "Game saved as: " << saveName << std::endl;
        }

        // Sauvegarder les équipes
        auto& teamManager = state::TeamManager::getInstance();
        teamManager.saveToDisk("teams.json");

        // Retourner à l'écran de fin
        if (manager) {
            manager->switchTo(render::SceneId::END_GAME);
        }
    }

    void PauseScreen::quitWithoutSaving() {
        std::cout << "[PAUSE] Quitter sans sauvegarder" << std::endl;

        // Sauvegarder quand même les équipes pour les changements persistants
        auto& teamManager = state::TeamManager::getInstance();
        teamManager.saveToDisk("teams.json");

        // Retourner à l'écran de fin
        if (manager) {
            manager->switchTo(render::SceneId::END_GAME);
        }
    }
} // namespace screen

