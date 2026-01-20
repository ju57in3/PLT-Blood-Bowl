#include "LoadGameScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "LayoutHelper.h"
#include "../../shared/utility/Constants.h"
#include "../../shared/state/GameStateManager.h"
#include "../../shared/engine.h"
#include <iostream>

namespace screen {

    LoadGameScreen::LoadGameScreen() = default;

    LoadGameScreen::~LoadGameScreen() = default;

    void LoadGameScreen::init(const std::shared_ptr<state::BloodBowlGame>& gamePtr, ResourceManager* resourcesPtr) {
        this->resources = resourcesPtr;

        try { font = resources->getFont("fonts/bloodbowl.ttf"); } catch (...) {
        }

        // Titre
        title.setFont(font);
        title.setString("Charger une partie");
        title.setCharacterSize(36);
        title.setFillColor(sf::Color::White);
        LayoutHelper::setRelativeY(title, 0.05f);
        LayoutHelper::centerHorizontally(title);

        // Bouton Retour
        backButton.setSize({200, 50});
        backButton.setFillColor(sf::Color(70, 70, 70));
        backButton.setOrigin(100, 25);
        backButton.setPosition(150, utility::Constants::WINDOW_HEIGHT - 75);

        backText.setFont(font);
        backText.setString("Retour");
        backText.setCharacterSize(22);
        backText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(backText, backButton);
    }

    void LoadGameScreen::setManager(SceneManager* mgr) {
        manager = mgr;
    }

    void LoadGameScreen::onEnter() {
        selectedIndex = -1;
        loadSavedGamesList();
        createUI();
    }

    void LoadGameScreen::loadSavedGamesList() {
        auto& gameStateManager = state::GameStateManager::getInstance();
        savedGames = gameStateManager.listSavedGames();
        std::cout << "Found " << savedGames.size() << " saved games" << std::endl;
    }

    void LoadGameScreen::createUI() {
        saveButtons.clear();
        saveTexts.clear();
        saveInfoTexts.clear();
        deleteButtons.clear();
        deleteTexts.clear();

        const float windowWidth = utility::Constants::WINDOW_WIDTH;
        const float startY = 150.0f;
        const float spacing = 100.0f;
        const float buttonWidth = 600.0f;
        const float buttonHeight = 80.0f;

        for (size_t i = 0; i < savedGames.size(); ++i) {
            float y = startY + i * spacing;

            // Bouton principal
            sf::RectangleShape button;
            button.setSize({buttonWidth, buttonHeight});
            button.setFillColor(sf::Color(50, 50, 100));
            button.setOutlineColor(sf::Color::White);
            button.setOutlineThickness(2);
            button.setOrigin(buttonWidth / 2, buttonHeight / 2);
            button.setPosition(windowWidth / 2, y + buttonHeight / 2);
            saveButtons.push_back(button);

            // Texte principal (nom de la sauvegarde)
            sf::Text text;
            text.setFont(font);
            text.setString(savedGames[i].displayName);
            text.setCharacterSize(22);
            text.setFillColor(sf::Color::White);
            text.setStyle(sf::Text::Bold);
            text.setPosition(windowWidth / 2 - buttonWidth / 2 + 10, y + 5);
            saveTexts.push_back(text);

            // Infos (équipes, score, tour, date)
            std::string info = savedGames[i].teamAName + " " + std::to_string(savedGames[i].scoreA) +
                             " - " + std::to_string(savedGames[i].scoreB) + " " + savedGames[i].teamBName +
                             "\nTour: " + std::to_string(savedGames[i].turnCounter) +
                             " | Etat: " + savedGames[i].currentState +
                             "\n" + savedGames[i].timestamp;

            sf::Text infoText;
            infoText.setFont(font);
            infoText.setString(info);
            infoText.setCharacterSize(14);
            infoText.setFillColor(sf::Color(200, 200, 200));
            infoText.setPosition(windowWidth / 2 - buttonWidth / 2 + 10, y + 30);
            saveInfoTexts.push_back(infoText);

            // Bouton supprimer
            sf::RectangleShape deleteBtn;
            deleteBtn.setSize({80, 40});
            deleteBtn.setFillColor(sf::Color(150, 50, 50));
            deleteBtn.setOrigin(40, 20);
            deleteBtn.setPosition(windowWidth / 2 + buttonWidth / 2 - 50, y + buttonHeight / 2);
            deleteButtons.push_back(deleteBtn);

            sf::Text deleteText;
            deleteText.setFont(font);
            deleteText.setString("Suppr");
            deleteText.setCharacterSize(16);
            deleteText.setFillColor(sf::Color::White);
            LayoutHelper::centerTextInRect(deleteText, deleteBtn);
            deleteTexts.push_back(deleteText);
        }
    }

    void LoadGameScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

            // Bouton retour
            if (backButton.getGlobalBounds().contains(mousePos)) {
                if (manager) manager->switchTo(render::SceneId::HOME);
                return;
            }

            // Boutons de sauvegarde
            for (size_t i = 0; i < saveButtons.size(); ++i) {
                // Vérifier si on clique sur le bouton supprimer
                if (deleteButtons[i].getGlobalBounds().contains(mousePos)) {
                    deleteSelectedSave(i);
                    return;
                }

                // Vérifier si on clique sur le bouton de sauvegarde
                if (saveButtons[i].getGlobalBounds().contains(mousePos)) {
                    selectedIndex = i;
                    loadSelectedGame();
                    return;
                }
            }
        }

        // Hover effect
        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mousePos = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});

            for (size_t i = 0; i < saveButtons.size(); ++i) {
                if (saveButtons[i].getGlobalBounds().contains(mousePos)) {
                    saveButtons[i].setFillColor(sf::Color(70, 70, 120));
                } else {
                    saveButtons[i].setFillColor(sf::Color(50, 50, 100));
                }
            }

            if (backButton.getGlobalBounds().contains(mousePos)) {
                backButton.setFillColor(sf::Color(90, 90, 90));
            } else {
                backButton.setFillColor(sf::Color(70, 70, 70));
            }
        }
    }

    void LoadGameScreen::update() {
        // Rien à mettre à jour en continu
    }

    void LoadGameScreen::draw(sf::RenderWindow& window) {
        window.clear(sf::Color(50, 50, 60)); // Uniform gray background

        window.draw(title);
        window.draw(backButton);
        window.draw(backText);

        // Dessiner les sauvegardes
        for (size_t i = 0; i < saveButtons.size(); ++i) {
            window.draw(saveButtons[i]);
            window.draw(saveTexts[i]);
            window.draw(saveInfoTexts[i]);
            window.draw(deleteButtons[i]);
            window.draw(deleteTexts[i]);
        }

        // Message si aucune sauvegarde
        if (savedGames.empty()) {
            sf::Text noSavesText;
            noSavesText.setFont(font);
            noSavesText.setString("Aucune partie sauvegardee");
            noSavesText.setCharacterSize(24);
            noSavesText.setFillColor(sf::Color(150, 150, 150));
            LayoutHelper::setRelativePosition(noSavesText, 0.5f, 300.0f / utility::Constants::WINDOW_HEIGHT);
            window.draw(noSavesText);
        }
    }

    render::SceneId LoadGameScreen::getId() const {
        return render::SceneId::LOAD_GAME;
    }

    void LoadGameScreen::loadSelectedGame() {
        if (selectedIndex < 0 || selectedIndex >= static_cast<int>(savedGames.size())) {
            return;
        }

        auto& gameStateManager = state::GameStateManager::getInstance();
        auto loadedGame = gameStateManager.loadGame(savedGames[selectedIndex].filename);

        if (loadedGame && manager) {
            // Charger le jeu dans le SceneManager
            manager->setGame(loadedGame);

            // Créer un nouvel Engine avec le jeu chargé
            auto newEngine = std::make_unique<engine::Engine>(loadedGame);
            manager->setEngine(std::move(newEngine));

            std::cout << "Game loaded successfully!" << std::endl;
            // Aller à l'écran de jeu
            manager->switchTo(render::SceneId::GAME);
        } else {
            std::cerr << "Failed to load game!" << std::endl;
        }
    }

    void LoadGameScreen::deleteSelectedSave(int index) {
        if (index < 0 || index >= static_cast<int>(savedGames.size())) {
            return;
        }

        auto& gameStateManager = state::GameStateManager::getInstance();
        if (gameStateManager.deleteSave(savedGames[index].filename)) {
            std::cout << "Save deleted: " << savedGames[index].filename << std::endl;
            // Recharger la liste
            loadSavedGamesList();
            createUI();
        } else {
            std::cerr << "Failed to delete save!" << std::endl;
        }
    }

} // namespace screen

