#include "NetworkScreen.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "state/TeamManager.h"
#include "network/NetworkClient.h"
#include "LayoutHelper.h"
#include <iostream>

namespace screen {

    NetworkScreen::NetworkScreen()
        : manager(nullptr), resources(nullptr),
          mode(NetworkMode::MENU),
          serverHost("localhost"), serverPort(8080),
          selectedTeamA(-1), selectedTeamB(-1),
          selectedButton(0) {
    }

    NetworkScreen::~NetworkScreen() = default;

    void NetworkScreen::init(const std::shared_ptr<state::BloodBowlGame>& game, ResourceManager* res) {
        this->resources = res;
        networkClient = std::make_unique<network::NetworkClient>(serverHost, serverPort);

        try { font = resources->getFont("fonts/bloodbowl.ttf"); } catch (...) {
        }

        initMenu();
    }

    void NetworkScreen::setManager(SceneManager* mgr) {
        this->manager = mgr;
    }

    void NetworkScreen::onEnter() {
        // Charger les équipes depuis le disque
        auto& teamManager = state::TeamManager::getInstance();
        teamManager.loadFromDisk("teams.json");

        // Initialiser la sélection avec la première équipe si disponible
        int teamCount = teamManager.getTeamCount();
        if (teamCount > 0) {
            selectedTeamA = 0;
            selectedTeamB = teamCount > 1 ? 1 : 0;
        } else {
            selectedTeamA = -1;
            selectedTeamB = -1;
        }

        mode = NetworkMode::MENU;
        initMenu();
    }

    void NetworkScreen::initMenu() {
        menuTexts.clear();
        buttons.clear();

        sf::Text title("Network Game", font, 40);
        title.setPosition(400, 100);
        title.setFillColor(sf::Color::White);
        menuTexts.push_back(title);

        // Bouton Créer une partie
        sf::Text createText("Create Game", font, 30);
        createText.setPosition(400, 250);
        menuTexts.push_back(createText);

        sf::RectangleShape createButton(sf::Vector2f(300, 50));
        createButton.setPosition(380, 240);
        createButton.setFillColor(sf::Color(100, 100, 100));
        buttons.push_back(createButton);

        // Bouton Rejoindre une partie
        sf::Text joinText("Join Game", font, 30);
        joinText.setPosition(400, 350);
        menuTexts.push_back(joinText);

        sf::RectangleShape joinButton(sf::Vector2f(300, 50));
        joinButton.setPosition(380, 340);
        joinButton.setFillColor(sf::Color(100, 100, 100));
        buttons.push_back(joinButton);

        // Bouton Retour
        sf::Text backText("Back", font, 30);
        backText.setPosition(400, 450);
        menuTexts.push_back(backText);

        sf::RectangleShape backButton(sf::Vector2f(300, 50));
        backButton.setPosition(380, 440);
        backButton.setFillColor(sf::Color(100, 100, 100));
        buttons.push_back(backButton);
    }

    void NetworkScreen::initCreateGame() {
        menuTexts.clear();
        buttons.clear();

        auto& teamManager = state::TeamManager::getInstance();
        int teamCount = teamManager.getTeamCount();

        sf::Text title("Create Network Game", font, 40);
        title.setPosition(300, 50);
        title.setFillColor(sf::Color::White);
        menuTexts.push_back(title);

        // Afficher la sélection d'équipe
        sf::Text teamLabel("Your Team:", font, 25);
        teamLabel.setPosition(300, 150);
        teamLabel.setFillColor(sf::Color::White);
        menuTexts.push_back(teamLabel);

        if (teamCount > 0) {
            // Bouton < pour équipe
            sf::RectangleShape prevBtn(sf::Vector2f(40, 40));
            prevBtn.setPosition(300, 200);
            prevBtn.setFillColor(sf::Color(100, 100, 100));
            buttons.push_back(prevBtn);

            sf::Text prevText("<", font, 25);
            prevText.setPosition(315, 205);
            prevText.setFillColor(sf::Color::White);
            menuTexts.push_back(prevText);

            // Nom de l'équipe sélectionnée
            const auto& teams = teamManager.getSavedTeams();
            if (selectedTeamA >= 0 && selectedTeamA < teamCount) {
                sf::Text teamName(teams[selectedTeamA]->getName(), font, 25);
                teamName.setPosition(350, 205);
                teamName.setFillColor(sf::Color::Yellow);
                menuTexts.push_back(teamName);
            }

            // Bouton > pour équipe
            sf::RectangleShape nextBtn(sf::Vector2f(40, 40));
            nextBtn.setPosition(600, 200);
            nextBtn.setFillColor(sf::Color(100, 100, 100));
            buttons.push_back(nextBtn);

            sf::Text nextText(">", font, 25);
            nextText.setPosition(615, 205);
            nextText.setFillColor(sf::Color::White);
            menuTexts.push_back(nextText);
        } else {
            sf::Text noTeams("No teams available! Create a team first.", font, 20);
            noTeams.setPosition(250, 200);
            noTeams.setFillColor(sf::Color::Red);
            menuTexts.push_back(noTeams);
        }

        // Bouton Créer
        sf::Text createText(teamCount > 0 ? "Create" : "Create (No Team!)", font, 30);
        createText.setPosition(400, 400);
        menuTexts.push_back(createText);

        sf::RectangleShape createBtn(sf::Vector2f(200, 50));
        createBtn.setPosition(380, 390);
        createBtn.setFillColor(teamCount > 0 ? sf::Color::Green : sf::Color(50, 50, 50));
        buttons.push_back(createBtn);

        // Bouton Retour
        sf::Text backText("Back", font, 30);
        backText.setPosition(400, 500);
        menuTexts.push_back(backText);

        sf::RectangleShape backBtn(sf::Vector2f(200, 50));
        backBtn.setPosition(380, 490);
        backBtn.setFillColor(sf::Color::Red);
        buttons.push_back(backBtn);
    }

    void NetworkScreen::initJoinGame() {
        menuTexts.clear();
        buttons.clear();

        auto& teamManager = state::TeamManager::getInstance();
        int teamCount = teamManager.getTeamCount();

        sf::Text title("Join Network Game", font, 40);
        title.setPosition(300, 50);
        title.setFillColor(sf::Color::White);
        menuTexts.push_back(title);

        sf::Text info("Enter Game ID:", font, 25);
        info.setPosition(300, 120);
        info.setFillColor(sf::Color::White);
        menuTexts.push_back(info);

        sf::Text gameIdText(gameIdInput.empty() ? "_" : gameIdInput, font, 30);
        gameIdText.setPosition(300, 160);
        gameIdText.setFillColor(sf::Color::Yellow);
        menuTexts.push_back(gameIdText);

        // Afficher la sélection d'équipe
        sf::Text teamLabel("Your Team:", font, 25);
        teamLabel.setPosition(300, 240);
        teamLabel.setFillColor(sf::Color::White);
        menuTexts.push_back(teamLabel);

        if (teamCount > 0) {
            // Bouton < pour équipe
            sf::RectangleShape prevBtn(sf::Vector2f(40, 40));
            prevBtn.setPosition(300, 290);
            prevBtn.setFillColor(sf::Color(100, 100, 100));
            buttons.push_back(prevBtn);

            sf::Text prevText("<", font, 25);
            prevText.setPosition(315, 295);
            prevText.setFillColor(sf::Color::White);
            menuTexts.push_back(prevText);

            // Nom de l'équipe sélectionnée
            const auto& teams = teamManager.getSavedTeams();
            if (selectedTeamB >= 0 && selectedTeamB < teamCount) {
                sf::Text teamName(teams[selectedTeamB]->getName(), font, 25);
                teamName.setPosition(350, 295);
                teamName.setFillColor(sf::Color::Yellow);
                menuTexts.push_back(teamName);
            }

            // Bouton > pour équipe
            sf::RectangleShape nextBtn(sf::Vector2f(40, 40));
            nextBtn.setPosition(600, 290);
            nextBtn.setFillColor(sf::Color(100, 100, 100));
            buttons.push_back(nextBtn);

            sf::Text nextText(">", font, 25);
            nextText.setPosition(615, 295);
            nextText.setFillColor(sf::Color::White);
            menuTexts.push_back(nextText);
        } else {
            sf::Text noTeams("No teams available! Create a team first.", font, 20);
            noTeams.setPosition(250, 290);
            noTeams.setFillColor(sf::Color::Red);
            menuTexts.push_back(noTeams);
        }

        // Bouton Rejoindre
        sf::Text joinText(teamCount > 0 ? "Join" : "Join (No Team!)", font, 30);
        joinText.setPosition(400, 400);
        menuTexts.push_back(joinText);

        sf::RectangleShape joinBtn(sf::Vector2f(200, 50));
        joinBtn.setPosition(380, 390);
        joinBtn.setFillColor(teamCount > 0 && !gameIdInput.empty() ? sf::Color::Green : sf::Color(50, 50, 50));
        buttons.push_back(joinBtn);

        // Bouton Retour
        sf::Text backText("Back", font, 30);
        backText.setPosition(400, 500);
        menuTexts.push_back(backText);

        sf::RectangleShape backBtn(sf::Vector2f(200, 50));
        backBtn.setPosition(380, 490);
        backBtn.setFillColor(sf::Color::Red);
        buttons.push_back(backBtn);
    }

    void NetworkScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
        // Gestion des clics de souris
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

            if (mode == NetworkMode::MENU) {
                // Vérifier quel bouton a été cliqué
                for (size_t i = 0; i < buttons.size(); ++i) {
                    if (buttons[i].getGlobalBounds().contains(mousePos)) {
                        selectedButton = static_cast<int>(i);
                        if (selectedButton == 0) {
                            mode = NetworkMode::CREATE_GAME;
                            selectedTeamA = 0;
                            selectedTeamB = 1;
                            selectedButton = 0;
                            initCreateGame();
                        } else if (selectedButton == 1) {
                            mode = NetworkMode::JOIN_GAME;
                            gameIdInput = "";
                            selectedButton = 0;
                            initJoinGame();
                        } else if (selectedButton == 2) {
                            if (manager) {
                                manager->switchTo(render::SceneId::HOME);
                            }
                        }
                        break;
                    }
                }
            } else if (mode == NetworkMode::CREATE_GAME) {
                auto& teamManager = state::TeamManager::getInstance();
                int teamCount = teamManager.getTeamCount();

                // Navigation entre équipes avec les boutons < et >
                if (teamCount > 0) {
                    // Bouton < (index 0)
                    if (buttons.size() > 0 && buttons[0].getGlobalBounds().contains(mousePos)) {
                        selectedTeamA = (selectedTeamA - 1 + teamCount) % teamCount;
                        initCreateGame();
                        return;
                    }
                    // Bouton > (index 1)
                    if (buttons.size() > 1 && buttons[1].getGlobalBounds().contains(mousePos)) {
                        selectedTeamA = (selectedTeamA + 1) % teamCount;
                        initCreateGame();
                        return;
                    }
                }

                // Bouton Créer (index 2 si équipes disponibles)
                int createBtnIndex = teamCount > 0 ? 2 : 0;
                if (buttons.size() > createBtnIndex && buttons[createBtnIndex].getGlobalBounds().contains(mousePos)) {
                    // Créer la partie avec les IDs réels des équipes
                    if (teamCount > 0 && networkClient) {
                        const auto& teams = teamManager.getSavedTeams();
                        int teamAId = teams[selectedTeamA]->getTeamId();
                        int teamBId = teams[selectedTeamB]->getTeamId();

                        if (networkClient->createGame(teamAId, teamBId)) {
                            std::cout << "Game ID: " << networkClient->getGameId() << std::endl;
                            auto game = networkClient->getGameState();
                            if (game && manager) {
                                manager->setGame(game);
                                manager->switchTo(render::SceneId::GAME);
                            }
                        }
                    }
                }
                // Bouton Retour
                int backBtnIndex = teamCount > 0 ? 3 : 1;
                if (buttons.size() > backBtnIndex && buttons[backBtnIndex].getGlobalBounds().contains(mousePos)) {
                    // Retour
                    mode = NetworkMode::MENU;
                    selectedButton = 0;
                    initMenu();
                }
            } else if (mode == NetworkMode::JOIN_GAME) {
                auto& teamManager = state::TeamManager::getInstance();
                int teamCount = teamManager.getTeamCount();

                // Navigation entre équipes avec les boutons < et >
                if (teamCount > 0) {
                    // Bouton < (index 0)
                    if (buttons.size() > 0 && buttons[0].getGlobalBounds().contains(mousePos)) {
                        selectedTeamB = (selectedTeamB - 1 + teamCount) % teamCount;
                        initJoinGame();
                        return;
                    }
                    // Bouton > (index 1)
                    if (buttons.size() > 1 && buttons[1].getGlobalBounds().contains(mousePos)) {
                        selectedTeamB = (selectedTeamB + 1) % teamCount;
                        initJoinGame();
                        return;
                    }
                }

                // Bouton Rejoindre (index 2 si équipes disponibles)
                int joinBtnIndex = teamCount > 0 ? 2 : 0;
                if (buttons.size() > joinBtnIndex && buttons[joinBtnIndex].getGlobalBounds().contains(mousePos)) {
                    // Rejoindre la partie avec l'ID réel de l'équipe
                    if (teamCount > 0 && !gameIdInput.empty() && networkClient) {
                        const auto& teams = teamManager.getSavedTeams();
                        int teamId = teams[selectedTeamB]->getTeamId();

                        if (networkClient->joinGame(gameIdInput, teamId)) {
                            auto game = networkClient->getGameState();
                            if (game && manager) {
                                manager->setGame(game);
                                manager->switchTo(render::SceneId::GAME);
                            }
                        }
                    }
                }
                // Bouton Retour
                int backBtnIndex = teamCount > 0 ? 3 : 1;
                if (buttons.size() > backBtnIndex && buttons[backBtnIndex].getGlobalBounds().contains(mousePos)) {
                    // Retour
                    mode = NetworkMode::MENU;
                    selectedButton = 0;
                    initMenu();
                }
            }
        }

        // Gestion du clavier
        if (event.type == sf::Event::KeyPressed) {
            if (mode == NetworkMode::MENU) {
                if (event.key.code == sf::Keyboard::Up) {
                    selectedButton = (selectedButton - 1 + 3) % 3;
                } else if (event.key.code == sf::Keyboard::Down) {
                    selectedButton = (selectedButton + 1) % 3;
                } else if (event.key.code == sf::Keyboard::Return) {
                    if (selectedButton == 0) {
                        mode = NetworkMode::CREATE_GAME;
                        selectedTeamA = 0;
                        selectedTeamB = 1;
                        selectedButton = 0;
                        initCreateGame();
                    } else if (selectedButton == 1) {
                        mode = NetworkMode::JOIN_GAME;
                        gameIdInput = "";
                        selectedButton = 0;
                        initJoinGame();
                    } else if (selectedButton == 2) {
                        if (manager) {
                            manager->switchTo(render::SceneId::HOME);
                        }
                    }
                }
            } else if (mode == NetworkMode::CREATE_GAME) {
                if (event.key.code == sf::Keyboard::Up) {
                    selectedButton = (selectedButton - 1 + 2) % 2;
                } else if (event.key.code == sf::Keyboard::Down) {
                    selectedButton = (selectedButton + 1) % 2;
                } else if (event.key.code == sf::Keyboard::Return) {
                    if (selectedButton == 0) {
                        // Créer la partie avec les IDs réels des équipes
                        auto& teamManager = state::TeamManager::getInstance();
                        int teamCount = teamManager.getTeamCount();

                        if (teamCount > 0 && networkClient) {
                            const auto& teams = teamManager.getSavedTeams();
                            int teamAId = teams[selectedTeamA]->getTeamId();
                            int teamBId = teams[selectedTeamB]->getTeamId();

                            if (networkClient->createGame(teamAId, teamBId)) {
                                std::cout << "Game ID: " << networkClient->getGameId() << std::endl;

                                // Récupérer l'état initial du jeu
                                auto game = networkClient->getGameState();
                                if (game && manager) {
                                    manager->setGame(game);
                                    manager->switchTo(render::SceneId::GAME);
                                }
                            }
                        }
                    } else if (selectedButton == 1) {
                        // Retour
                        mode = NetworkMode::MENU;
                        selectedButton = 0;
                        initMenu();
                    }
                } else if (event.key.code == sf::Keyboard::Escape) {
                    mode = NetworkMode::MENU;
                    selectedButton = 0;
                    initMenu();
                }
            } else if (mode == NetworkMode::JOIN_GAME) {
                if (event.key.code == sf::Keyboard::Up) {
                    selectedButton = (selectedButton - 1 + 2) % 2;
                } else if (event.key.code == sf::Keyboard::Down) {
                    selectedButton = (selectedButton + 1) % 2;
                } else if (event.key.code == sf::Keyboard::Return) {
                    if (selectedButton == 0 && !gameIdInput.empty()) {
                        // Rejoindre la partie avec l'ID réel de l'équipe
                        auto& teamManager = state::TeamManager::getInstance();
                        int teamCount = teamManager.getTeamCount();

                        if (teamCount > 0 && networkClient) {
                            const auto& teams = teamManager.getSavedTeams();
                            int teamId = teams[selectedTeamB]->getTeamId();

                            if (networkClient->joinGame(gameIdInput, teamId)) {
                                auto game = networkClient->getGameState();
                                if (game && manager) {
                                    manager->setGame(game);
                                    // Passer le NetworkClient au SceneManager pour la synchronisation
                                    manager->setNetworkClient(networkClient.get());
                                    manager->switchTo(render::SceneId::GAME);
                                }
                            }
                        }
                    } else if (selectedButton == 1) {
                        // Retour
                        mode = NetworkMode::MENU;
                        selectedButton = 0;
                        initMenu();
                    }
                } else if (event.key.code == sf::Keyboard::Escape) {
                    mode = NetworkMode::MENU;
                    selectedButton = 0;
                    initMenu();
                } else if (event.key.code == sf::Keyboard::BackSpace && !gameIdInput.empty()) {
                    gameIdInput.pop_back();
                    initJoinGame();
                }
            }
        } else if (event.type == sf::Event::TextEntered && mode == NetworkMode::JOIN_GAME) {
            if (event.text.unicode >= 32 && event.text.unicode < 128) {
                gameIdInput += static_cast<char>(event.text.unicode);
                initJoinGame();
            }
        }
    }

    void NetworkScreen::update() {
        auto& teamManager = state::TeamManager::getInstance();
        int teamCount = teamManager.getTeamCount();

        // Mettre à jour les couleurs des boutons selon la sélection
        for (size_t i = 0; i < buttons.size(); ++i) {
            if (mode == NetworkMode::MENU) {
                if (static_cast<int>(i) == selectedButton) {
                    buttons[i].setFillColor(sf::Color(150, 150, 200));
                } else {
                    buttons[i].setFillColor(sf::Color(100, 100, 100));
                }
            } else if (mode == NetworkMode::CREATE_GAME) {
                // Boutons de navigation d'équipe (0 et 1)
                if (teamCount > 0 && (i == 0 || i == 1)) {
                    buttons[i].setFillColor(sf::Color(100, 100, 100));
                }
                // Bouton Créer
                else if ((teamCount > 0 && i == 2) || (teamCount == 0 && i == 0)) {
                    buttons[i].setFillColor(teamCount > 0 ? sf::Color::Green : sf::Color(50, 50, 50));
                }
                // Bouton Retour
                else if ((teamCount > 0 && i == 3) || (teamCount == 0 && i == 1)) {
                    buttons[i].setFillColor(sf::Color::Red);
                }
            } else if (mode == NetworkMode::JOIN_GAME) {
                // Boutons de navigation d'équipe (0 et 1)
                if (teamCount > 0 && (i == 0 || i == 1)) {
                    buttons[i].setFillColor(sf::Color(100, 100, 100));
                }
                // Bouton Rejoindre
                else if ((teamCount > 0 && i == 2) || (teamCount == 0 && i == 0)) {
                    buttons[i].setFillColor(teamCount > 0 && !gameIdInput.empty() ? sf::Color::Green : sf::Color(50, 50, 50));
                }
                // Bouton Retour
                else if ((teamCount > 0 && i == 3) || (teamCount == 0 && i == 1)) {
                    buttons[i].setFillColor(sf::Color::Red);
                }
            }
        }
    }

    void NetworkScreen::draw(sf::RenderWindow& window) {
        window.clear(sf::Color(30, 30, 50));

        switch (mode) {
            case NetworkMode::MENU:
                drawMenu(window);
                break;
            case NetworkMode::CREATE_GAME:
                drawCreateGame(window);
                break;
            case NetworkMode::JOIN_GAME:
                drawJoinGame(window);
                break;
            case NetworkMode::WAITING:
                drawWaiting(window);
                break;
        }
    }

    void NetworkScreen::drawMenu(sf::RenderWindow& window) {
        for (const auto& button : buttons) {
            window.draw(button);
        }
        for (const auto& text : menuTexts) {
            window.draw(text);
        }
    }

    void NetworkScreen::drawCreateGame(sf::RenderWindow& window) {
        for (const auto& button : buttons) {
            window.draw(button);
        }
        for (const auto& text : menuTexts) {
            window.draw(text);
        }
    }

    void NetworkScreen::drawJoinGame(sf::RenderWindow& window) {
        for (const auto& button : buttons) {
            window.draw(button);
        }
        for (const auto& text : menuTexts) {
            window.draw(text);
        }
    }

    void NetworkScreen::drawWaiting(sf::RenderWindow& window) {
        sf::Text waitText("Waiting for opponent...", font, 40);
        waitText.setPosition(300, 300);
        waitText.setFillColor(sf::Color::White);
        window.draw(waitText);
    }

    render::SceneId NetworkScreen::getId() const {
        return render::SceneId::NETWORK;
    }

} // namespace screen

