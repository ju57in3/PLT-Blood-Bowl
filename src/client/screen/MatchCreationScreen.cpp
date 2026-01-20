#include "MatchCreationScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "LayoutHelper.h"
#include "../../shared/utility/Constants.h"
#include "../../shared/state/TeamManager.h"
#include "../../shared/state/Team.h"
#include "../../shared/state/BloodBowlGame.h"
#include "../../shared/engine/Engine.h"
#include "../../shared/ai/RandomAI.h"
#include "../../shared/ai/HeuristicAI.h"
#include <iostream>

#include "ai/AdvancedAI.h"

namespace screen {
    MatchCreationScreen::MatchCreationScreen()
        : selectedTeam1Index(-1), selectedTeam2Index(-1),
          gameMode(0), selectedAIType(0), selectedAI1Type(0) {
    }

    MatchCreationScreen::~MatchCreationScreen() = default;

    void MatchCreationScreen::init(const std::shared_ptr<state::BloodBowlGame> &gamePtr, ResourceManager *resources) {
        this->resources = resources;
        const float windowWidth = utility::Constants::WINDOW_WIDTH;
        const float windowHeight = utility::Constants::WINDOW_HEIGHT;

        try { font = resources->getFont("fonts/bloodbowl.ttf"); } catch (...) {
        }

        // Titre principal
        title.setFont(font);
        title.setString("Creation de match");
        title.setCharacterSize(32);
        title.setFillColor(sf::Color::White);
        title.setPosition(windowWidth / 2 - 150, 20);

        // === ÉQUIPE 1 ===
        team1Label.setFont(font);
        team1Label.setString("Equipe 1:");
        team1Label.setCharacterSize(20);
        team1Label.setFillColor(sf::Color::White);
        team1Label.setPosition(50, 80);

        team1SelectBox.setSize({400, 40});
        team1SelectBox.setFillColor(sf::Color(50, 50, 80));
        team1SelectBox.setOutlineColor(sf::Color::White);
        team1SelectBox.setOutlineThickness(2);
        team1SelectBox.setPosition(50, 110);

        team1NameText.setFont(font);
        team1NameText.setCharacterSize(18);
        team1NameText.setFillColor(sf::Color::White);
        team1NameText.setPosition(60, 118);

        team1PrevButton.setSize({35, 35});
        team1PrevButton.setFillColor(sf::Color(100, 100, 100));
        team1PrevButton.setPosition(460, 112);

        team1PrevText.setFont(font);
        team1PrevText.setString("<");
        team1PrevText.setCharacterSize(20);
        team1PrevText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(team1PrevText, team1PrevButton);

        team1NextButton.setSize({35, 35});
        team1NextButton.setFillColor(sf::Color(100, 100, 100));
        team1NextButton.setPosition(500, 112);

        team1NextText.setFont(font);
        team1NextText.setString(">");
        team1NextText.setCharacterSize(20);
        team1NextText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(team1NextText, team1NextButton);

        // === ÉQUIPE 2 ===
        team2Label.setFont(font);
        team2Label.setString("Equipe 2:");
        team2Label.setCharacterSize(20);
        team2Label.setFillColor(sf::Color::White);
        team2Label.setPosition(50, 170);

        team2SelectBox.setSize({400, 40});
        team2SelectBox.setFillColor(sf::Color(80, 50, 50));
        team2SelectBox.setOutlineColor(sf::Color::White);
        team2SelectBox.setOutlineThickness(2);
        team2SelectBox.setPosition(50, 200);

        team2NameText.setFont(font);
        team2NameText.setCharacterSize(18);
        team2NameText.setFillColor(sf::Color::White);
        team2NameText.setPosition(60, 208);

        team2PrevButton.setSize({35, 35});
        team2PrevButton.setFillColor(sf::Color(100, 100, 100));
        team2PrevButton.setPosition(460, 202);

        team2PrevText.setFont(font);
        team2PrevText.setString("<");
        team2PrevText.setCharacterSize(20);
        team2PrevText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(team2PrevText, team2PrevButton);

        team2NextButton.setSize({35, 35});
        team2NextButton.setFillColor(sf::Color(100, 100, 100));
        team2NextButton.setPosition(500, 202);

        team2NextText.setFont(font);
        team2NextText.setString(">");
        team2NextText.setCharacterSize(20);
        team2NextText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(team2NextText, team2NextButton);

        // === MODE DE JEU ===
        modeLabel.setFont(font);
        modeLabel.setString("Mode de jeu:");
        modeLabel.setCharacterSize(20);
        modeLabel.setFillColor(sf::Color::White);
        modeLabel.setPosition(50, 260);

        pvpButton.setSize({150, 40});
        pvpButton.setFillColor(sf::Color(70, 120, 70));
        pvpButton.setOutlineColor(sf::Color::Yellow);
        pvpButton.setOutlineThickness(3);
        pvpButton.setPosition(50, 290);

        pvpText.setFont(font);
        pvpText.setString("Humain vs Humain");
        pvpText.setCharacterSize(14);
        pvpText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(pvpText, pvpButton);

        pveButton.setSize({150, 40});
        pveButton.setFillColor(sf::Color(120, 70, 70));
        pveButton.setPosition(210, 290);

        pveText.setFont(font);
        pveText.setString("Humain vs IA");
        pveText.setCharacterSize(14);
        pveText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(pveText, pveButton);

        aivsaiButton.setSize({150, 40});
        aivsaiButton.setFillColor(sf::Color(70, 70, 120));
        aivsaiButton.setPosition(370, 290);

        aivsaiText.setFont(font);
        aivsaiText.setString("IA vs IA");
        aivsaiText.setCharacterSize(14);
        aivsaiText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(aivsaiText, aivsaiButton);

        // === TYPE D'IA EQUIPE 1 (AIvAI mode only) ===
        ai1TypeLabel.setFont(font);
        ai1TypeLabel.setString("IA Equipe 1:");
        ai1TypeLabel.setCharacterSize(20);
        ai1TypeLabel.setFillColor(sf::Color::White);
        ai1TypeLabel.setPosition(50, 350);

        randomAI1Button.setSize({150, 35});
        randomAI1Button.setFillColor(sf::Color(80, 80, 120));
        randomAI1Button.setOutlineColor(sf::Color::Yellow);
        randomAI1Button.setOutlineThickness(2);
        randomAI1Button.setPosition(50, 380);

        randomAI1Text.setFont(font);
        randomAI1Text.setString("Random");
        randomAI1Text.setCharacterSize(16);
        randomAI1Text.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(randomAI1Text, randomAI1Button);

        heuristicAI1Button.setSize({150, 35});
        heuristicAI1Button.setFillColor(sf::Color(80, 80, 120));
        heuristicAI1Button.setPosition(210, 380);

        heuristicAI1Text.setFont(font);
        heuristicAI1Text.setString("Heuristique");
        heuristicAI1Text.setCharacterSize(16);
        heuristicAI1Text.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(heuristicAI1Text, heuristicAI1Button);

        advancedAI1Button.setSize({150, 35});
        advancedAI1Button.setFillColor(sf::Color(80, 80, 120));
        advancedAI1Button.setPosition(370, 380);

        advancedAI1Text.setFont(font);
        advancedAI1Text.setString("Avancee");
        advancedAI1Text.setCharacterSize(16);
        advancedAI1Text.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(advancedAI1Text, advancedAI1Button);

        // === TYPE D'IA EQUIPE 2 (PvE and AIvAI modes) ===
        aiTypeLabel.setFont(font);
        aiTypeLabel.setString("IA Equipe 2:");
        aiTypeLabel.setCharacterSize(20);
        aiTypeLabel.setFillColor(sf::Color::White);
        aiTypeLabel.setPosition(50, 430);

        randomAIButton.setSize({150, 35});
        randomAIButton.setFillColor(sf::Color(80, 80, 120));
        randomAIButton.setOutlineColor(sf::Color::Yellow);
        randomAIButton.setOutlineThickness(2);
        randomAIButton.setPosition(50, 460);

        randomAIText.setFont(font);
        randomAIText.setString("Random");
        randomAIText.setCharacterSize(16);
        randomAIText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(randomAIText, randomAIButton);

        heuristicAIButton.setSize({150, 35});
        heuristicAIButton.setFillColor(sf::Color(80, 80, 120));
        heuristicAIButton.setPosition(210, 460);

        heuristicAIText.setFont(font);
        heuristicAIText.setString("Heuristic");
        heuristicAIText.setCharacterSize(16);
        heuristicAIText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(heuristicAIText, heuristicAIButton);

        advancedAIButton.setSize({150, 35});
        advancedAIButton.setFillColor(sf::Color(80, 80, 120));
        advancedAIButton.setPosition(370, 460);

        advancedAIText.setFont(font);
        advancedAIText.setString("Avdanced");
        advancedAIText.setCharacterSize(16);
        advancedAIText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(advancedAIText, advancedAIButton);

        // === STEP BY STEP MODE ===
        aiStepModeButton.setSize({150, 35});
        aiStepModeButton.setFillColor(sf::Color(80,80,120));
        aiStepModeButton.setPosition(520, 460);

        aiStepModeText.setFont(font);
        aiStepModeText.setString("Step by Step : Off");
        aiStepModeText.setCharacterSize(16);
        aiStepModeText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(aiStepModeText, aiStepModeButton);


        // === MESSAGE D'ERREUR ===
        errorText.setFont(font);
        errorText.setCharacterSize(16);
        errorText.setFillColor(sf::Color::Red);
        errorText.setPosition(50, 520);

        // === BOUTONS D'ACTION ===
        backButton.setSize({140, 40});
        backButton.setFillColor(sf::Color(70, 70, 70));
        backButton.setPosition(20, windowHeight - 60);

        backText.setFont(font);
        backText.setString("Back");
        backText.setCharacterSize(18);
        backText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(backText, backButton);

        startButton.setSize({200, 50});
        startButton.setFillColor(sf::Color(50, 150, 50));
        startButton.setPosition(windowWidth - 220, windowHeight - 65);

        startText.setFont(font);
        startText.setString("Demarrer le match");
        startText.setCharacterSize(18);
        startText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(startText, startButton);

        // Initialiser avec la première équipe si disponible
        auto& teamManager = state::TeamManager::getInstance();
        if (teamManager.getTeamCount() > 0) {
            selectedTeam1Index = 0;
            if (teamManager.getTeamCount() > 1) {
                selectedTeam2Index = 1;
            }
        }
    }

    void MatchCreationScreen::setManager(SceneManager *mgr) { manager = mgr; }

    void MatchCreationScreen::onEnter() {
        // Reload teams from disk when entering the screen
        auto& teamManager = state::TeamManager::getInstance();
        teamManager.loadFromDisk("teams.json");

        // Reset selections based on available teams
        selectedTeam1Index = -1;
        selectedTeam2Index = -1;

        if (teamManager.getTeamCount() > 0) {
            selectedTeam1Index = 0;
            if (teamManager.getTeamCount() > 1) {
                selectedTeam2Index = 1;
            }
        }

        if (manager) {
            aiStepModeLocal = manager->isAIStepModeEnabled();
            updateAIStepButtonText();
        }
    }

    void MatchCreationScreen::handleEvent(const sf::Event &event, sf::RenderWindow &window) {
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mpos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

            auto& teamManager = state::TeamManager::getInstance();
            int teamCount = teamManager.getTeamCount();

            // Bouton retour
            if (backButton.getGlobalBounds().contains(mpos)) {
                if (manager) manager->switchTo(render::SceneId::HOME);
                return;
            }

            // Bouton démarrer
            if (startButton.getGlobalBounds().contains(mpos) && canStartMatch()) {
                createAndStartMatch();
                return;
            }

            // Navigation équipe 1
            if (team1PrevButton.getGlobalBounds().contains(mpos) && teamCount > 0) {
                selectedTeam1Index = (selectedTeam1Index - 1 + teamCount) % teamCount;
            }
            if (team1NextButton.getGlobalBounds().contains(mpos) && teamCount > 0) {
                selectedTeam1Index = (selectedTeam1Index + 1) % teamCount;
            }

            // Navigation équipe 2
            if (team2PrevButton.getGlobalBounds().contains(mpos) && teamCount > 0) {
                selectedTeam2Index = (selectedTeam2Index - 1 + teamCount) % teamCount;
            }
            if (team2NextButton.getGlobalBounds().contains(mpos) && teamCount > 0) {
                selectedTeam2Index = (selectedTeam2Index + 1) % teamCount;
            }

            // Sélection mode de jeu
            if (pvpButton.getGlobalBounds().contains(mpos)) {
                gameMode = 0; // PvP
                pvpButton.setOutlineThickness(3);
                pvpButton.setOutlineColor(sf::Color::Yellow);
                pveButton.setOutlineThickness(0);
                aivsaiButton.setOutlineThickness(0);
            }
            if (pveButton.getGlobalBounds().contains(mpos)) {
                gameMode = 1; // PvE
                pveButton.setOutlineThickness(3);
                pveButton.setOutlineColor(sf::Color::Yellow);
                pvpButton.setOutlineThickness(0);
                aivsaiButton.setOutlineThickness(0);
            }
            if (aivsaiButton.getGlobalBounds().contains(mpos)) {
                gameMode = 2; // AIvAI
                aivsaiButton.setOutlineThickness(3);
                aivsaiButton.setOutlineColor(sf::Color::Yellow);
                pvpButton.setOutlineThickness(0);
                pveButton.setOutlineThickness(0);
            }

            // Sélection type d'IA Team 1 (seulement si AIvAI)
            if (gameMode == 2) {
                if (randomAI1Button.getGlobalBounds().contains(mpos)) {
                    selectedAI1Type = 0;
                    randomAI1Button.setOutlineThickness(2);
                    randomAI1Button.setOutlineColor(sf::Color::Yellow);
                    heuristicAI1Button.setOutlineThickness(0);
                    advancedAI1Button.setOutlineThickness(0);
                }
                if (heuristicAI1Button.getGlobalBounds().contains(mpos)) {
                    selectedAI1Type = 1;
                    heuristicAI1Button.setOutlineThickness(2);
                    heuristicAI1Button.setOutlineColor(sf::Color::Yellow);
                    randomAI1Button.setOutlineThickness(0);
                    advancedAI1Button.setOutlineThickness(0);
                }
                if (advancedAI1Button.getGlobalBounds().contains(mpos)) {
                    selectedAI1Type = 2;
                    advancedAI1Button.setOutlineThickness(2);
                    advancedAI1Button.setOutlineColor(sf::Color::Yellow);
                    randomAI1Button.setOutlineThickness(0);
                    heuristicAI1Button.setOutlineThickness(0);
                }
            }

            // Sélection type d'IA Team 2 (seulement si PvE ou AIvAI)
            if (gameMode == 1 || gameMode == 2) {
                if (randomAIButton.getGlobalBounds().contains(mpos)) {
                    selectedAIType = 0;
                    randomAIButton.setOutlineThickness(2);
                    randomAIButton.setOutlineColor(sf::Color::Yellow);
                    heuristicAIButton.setOutlineThickness(0);
                    advancedAIButton.setOutlineThickness(0);
                }
                if (heuristicAIButton.getGlobalBounds().contains(mpos)) {
                    selectedAIType = 1;
                    heuristicAIButton.setOutlineThickness(2);
                    heuristicAIButton.setOutlineColor(sf::Color::Yellow);
                    randomAIButton.setOutlineThickness(0);
                    advancedAIButton.setOutlineThickness(0);
                }
                if (advancedAIButton.getGlobalBounds().contains(mpos)) {
                    selectedAIType = 2;
                    advancedAIButton.setOutlineThickness(2);
                    advancedAIButton.setOutlineColor(sf::Color::Yellow);
                    randomAIButton.setOutlineThickness(0);
                    heuristicAIButton.setOutlineThickness(0);
                }
            }

            // Toggle AI Step by Step
            if (aiStepModeButton.getGlobalBounds().contains(mpos)) {
                aiStepModeLocal = !aiStepModeLocal;
                if (manager) manager->setAIStepModeEnabled(aiStepModeLocal);
                updateAIStepButtonText();
                return;
            }
        }
    }

    void MatchCreationScreen::update() {
        updateTeamDisplay();

        // Mettre à jour l'état du bouton démarrer
        if (canStartMatch()) {
            startButton.setFillColor(sf::Color(50, 150, 50));
        } else {
            startButton.setFillColor(sf::Color(100, 100, 100));
        }

        // Mettre à jour le message d'erreur
        errorText.setString(getErrorMessage());
    }

    void MatchCreationScreen::updateTeamDisplay() {
        auto& teamManager = state::TeamManager::getInstance();
        const auto& teams = teamManager.getSavedTeams();

        if (teams.empty()) {
            team1NameText.setString("Aucune equipe disponible");
            team2NameText.setString("Aucune equipe disponible");
            return;
        }

        // Équipe 1
        if (selectedTeam1Index >= 0 && selectedTeam1Index < static_cast<int>(teams.size())) {
            team1NameText.setString(teams[selectedTeam1Index]->getName() +
                                   " (" + std::to_string(teams[selectedTeam1Index]->getCharacters().size()) + " joueurs)");
        }

        // Équipe 2
        if (selectedTeam2Index >= 0 && selectedTeam2Index < static_cast<int>(teams.size())) {
            team2NameText.setString(teams[selectedTeam2Index]->getName() +
                                   " (" + std::to_string(teams[selectedTeam2Index]->getCharacters().size()) + " joueurs)");
        }
    }

    bool MatchCreationScreen::canStartMatch() const {
        auto& teamManager = state::TeamManager::getInstance();

        // Au moins 2 équipes
        if (teamManager.getTeamCount() < 2) return false;

        // Indices valides
        if (selectedTeam1Index < 0 || selectedTeam2Index < 0) return false;

        // Équipes différentes
        if (selectedTeam1Index == selectedTeam2Index) return false;

        return true;
    }

    std::string MatchCreationScreen::getErrorMessage() const {
        auto& teamManager = state::TeamManager::getInstance();

        if (teamManager.getTeamCount() == 0) {
            return "Veuillez creer au moins 2 equipes";
        }
        if (teamManager.getTeamCount() == 1) {
            return "Veuillez creer une deuxieme equipe";
        }
        if (selectedTeam1Index == selectedTeam2Index) {
            return "Veuillez selectionner deux equipes differentes";
        }
        return "";
    }

    void MatchCreationScreen::createAndStartMatch() {
        auto& teamManager = state::TeamManager::getInstance();
        const auto& teams = teamManager.getSavedTeams();

        if (!canStartMatch()) return;

        // Récupérer les équipes sélectionnées
        state::Team* team1 = teams[selectedTeam1Index].get();
        state::Team* team2 = teams[selectedTeam2Index].get();

        // Réinitialiser les données temporaires de match (position, status, score)
        // Les données persistantes (XP, blessures permanentes, compétences) sont conservées
        team1->resetMatchData();
        team2->resetMatchData();

        // Créer le nouveau jeu avec les équipes réinitialisées
        auto newGame = std::make_shared<state::BloodBowlGame>(*team1, *team2);

        // Créer le nouveau moteur de jeu avec smart pointer
        auto newEngine = std::make_unique<engine::Engine>(newGame);

        // Mettre à jour le jeu et l'engine dans le SceneManager
        if (manager) {
            manager->setGame(newGame);

            // Récupérer un pointeur brut pour configurer l'IA avant de transférer la propriété
            engine::Engine* enginePtr = newEngine.get();

            // Créer et configurer l'IA selon le mode de jeu
            if (gameMode == 1) {
                // Mode PvE : Humain vs IA (team 2 est l'IA)
                int aiTeamId = team2->getTeamId();
                std::unique_ptr<ai::AI> aiInstance = nullptr;

                switch (selectedAIType) {
                    case 0: // Random AI
                        std::cout << "[MATCH CREATION] Creating RandomAI for team " << aiTeamId << "\n";
                        aiInstance = std::make_unique<ai::RandomAI>(
                            *enginePtr,
                            newGame,
                            aiTeamId
                        );
                        break;

                    case 1: // Heuristic AI
                        std::cout << "[MATCH CREATION] Creating HeuristicAI for team " << aiTeamId << "\n";
                        aiInstance = std::make_unique<ai::HeuristicAI>(
                            *enginePtr,
                            newGame,
                            aiTeamId
                        );
                        break;

                    case 2: // Advanced AI
                        std::cout << "[MATCH CREATION] Creating AdvancedAI for team " << aiTeamId << "\n";
                        aiInstance = std::make_unique<ai::AdvancedAI>(
                            *enginePtr,
                            newGame,
                            aiTeamId
                        );
                        break;

                    default:
                        std::cout << "[MATCH CREATION] Invalid AI type, defaulting to RandomAI\n";
                        aiInstance = std::make_unique<ai::RandomAI>(
                            *enginePtr,
                            newGame,
                            aiTeamId
                        );
                        break;
                }

                if (aiInstance) {
                    enginePtr->setAI(std::move(aiInstance));
                }
            } else if (gameMode == 2) {
                // Mode AIvAI : deux IAs jouent l'une contre l'autre

                int ai1TeamId = team1->getTeamId();
                int ai2TeamId = team2->getTeamId();

                std::unique_ptr<ai::AI> ai1Instance = nullptr;
                std::unique_ptr<ai::AI> ai2Instance = nullptr;

                // Créer l'IA pour l'équipe 1
                switch (selectedAI1Type) {
                    case 0:
                        std::cout << "[MATCH CREATION] Creating RandomAI for team " << ai1TeamId << "\n";
                        ai1Instance = std::make_unique<ai::RandomAI>(*enginePtr, newGame, ai1TeamId);
                        break;
                    case 1:
                        std::cout << "[MATCH CREATION] Creating HeuristicAI for team " << ai1TeamId << "\n";
                        ai1Instance = std::make_unique<ai::HeuristicAI>(*enginePtr, newGame, ai1TeamId);
                        break;
                    case 2:
                        std::cout << "[MATCH CREATION] Creating AdvancedAI for team " << ai1TeamId << "\n";
                        ai1Instance = std::make_unique<ai::AdvancedAI>(*enginePtr, newGame, ai1TeamId);
                        break;
                    default:
                        ai1Instance = std::make_unique<ai::RandomAI>(*enginePtr, newGame, ai1TeamId);
                        break;
                }

                // Créer l'IA pour l'équipe 2
                switch (selectedAIType) {
                    case 0:
                        std::cout << "[MATCH CREATION] Creating RandomAI for team " << ai2TeamId << "\n";
                        ai2Instance = std::make_unique<ai::RandomAI>(*enginePtr, newGame, ai2TeamId);
                        break;
                    case 1:
                        std::cout << "[MATCH CREATION] Creating HeuristicAI for team " << ai2TeamId << "\n";
                        ai2Instance = std::make_unique<ai::HeuristicAI>(*enginePtr, newGame, ai2TeamId);
                        break;
                    case 2:
                        std::cout << "[MATCH CREATION] Creating AdvancedAI for team " << ai2TeamId << "\n";
                        ai2Instance = std::make_unique<ai::AdvancedAI>(*enginePtr, newGame, ai2TeamId);
                        break;
                    default:
                        ai2Instance = std::make_unique<ai::RandomAI>(*enginePtr, newGame, ai2TeamId);
                        break;
                }

                // Configurer les deux IAs dans l'Engine
                if (ai1Instance) {
                    enginePtr->setAI(std::move(ai1Instance));
                }
                if (ai2Instance) {
                    enginePtr->setSecondAI(std::move(ai2Instance));
                }

            } else if (gameMode == 0) {
                std::cout << "[MATCH CREATION] PvP mode - No AI\n";
            }

            // Transférer la propriété de l'engine au SceneManager
            manager->setEngine(std::move(newEngine));

            // Lancer le match
            manager->switchTo(render::SceneId::GAME);
        }
    }

    void MatchCreationScreen::draw(sf::RenderWindow &window) {
        window.clear(sf::Color(30, 30, 30));

        window.draw(title);

        // Équipes
        window.draw(team1Label);
        window.draw(team1SelectBox);
        window.draw(team1NameText);
        window.draw(team1PrevButton);
        window.draw(team1PrevText);
        window.draw(team1NextButton);
        window.draw(team1NextText);

        window.draw(team2Label);
        window.draw(team2SelectBox);
        window.draw(team2NameText);
        window.draw(team2PrevButton);
        window.draw(team2PrevText);
        window.draw(team2NextButton);
        window.draw(team2NextText);

        // Mode de jeu
        window.draw(modeLabel);
        window.draw(pvpButton);
        window.draw(pvpText);
        window.draw(pveButton);
        window.draw(pveText);
        window.draw(aivsaiButton);
        window.draw(aivsaiText);

        // Type d'IA Team 1 (seulement si AIvAI sélectionné)
        if (gameMode == 2) {
            window.draw(ai1TypeLabel);
            window.draw(randomAI1Button);
            window.draw(randomAI1Text);
            window.draw(heuristicAI1Button);
            window.draw(heuristicAI1Text);
            window.draw(advancedAI1Button);
            window.draw(advancedAI1Text);
        }

        // Type d'IA Team 2 (seulement si PvE ou AIvAI sélectionné)
        if (gameMode == 1 || gameMode == 2) {
            window.draw(aiTypeLabel);
            window.draw(randomAIButton);
            window.draw(randomAIText);
            window.draw(heuristicAIButton);
            window.draw(heuristicAIText);
            window.draw(advancedAIButton);
            window.draw(advancedAIText);
        }

        // Message d'erreur
        window.draw(errorText);

        // Boutons d'action
        window.draw(backButton);
        window.draw(backText);
        window.draw(startButton);
        window.draw(startText);

        // AI Step by Step Button
        if (gameMode == 1 || gameMode == 2) {
            window.draw(aiStepModeButton);
            window.draw(aiStepModeText);
        }
    }

    render::SceneId MatchCreationScreen::getId() const {
        return render::SceneId::MATCH_CREATION;
    }

    void MatchCreationScreen::updateAIStepButtonText() {
        aiStepModeText.setString(aiStepModeLocal ? "AI Step by Step : On" : "AI Step by Step : Off");
        LayoutHelper::centerTextInRect(aiStepModeText, aiStepModeButton);
    }
} // namespace screen
