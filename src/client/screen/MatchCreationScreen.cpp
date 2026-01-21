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
        title.setString("Create a Match");
        title.setCharacterSize(48);
        title.setFillColor(sf::Color::White);
        LayoutHelper::setRelativePosition(title, 0.5f, 0.08f);

        // === ÉQUIPE 1 ===
        team1Label.setFont(font);
        team1Label.setString("Equipe 1:");
        team1Label.setCharacterSize(22);
        team1Label.setFillColor(sf::Color::White);
        team1Label.setPosition(60, 120);

        team1SelectBox.setSize({420, 45});
        team1SelectBox.setFillColor(sf::Color(70, 80, 100));
        team1SelectBox.setOutlineColor(sf::Color(150, 150, 170));
        team1SelectBox.setOutlineThickness(2);
        team1SelectBox.setPosition(60, 150);

        team1NameText.setFont(font);
        team1NameText.setCharacterSize(19);
        team1NameText.setFillColor(sf::Color::White);
        team1NameText.setPosition(70, 160);

        team1PrevButton.setSize({38, 38});
        team1PrevButton.setFillColor(sf::Color(80, 80, 100));
        team1PrevButton.setOutlineColor(sf::Color(150, 150, 170));
        team1PrevButton.setOutlineThickness(1);
        team1PrevButton.setOrigin(19.f, 19.f);
        team1PrevButton.setPosition(508.f, 172.5f);

        team1PrevText.setFont(font);
        team1PrevText.setString("<");
        team1PrevText.setCharacterSize(22);
        team1PrevText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(team1PrevText, team1PrevButton);

        team1NextButton.setSize({38, 38});
        team1NextButton.setFillColor(sf::Color(80, 80, 100));
        team1NextButton.setOutlineColor(sf::Color(150, 150, 170));
        team1NextButton.setOutlineThickness(1);
        team1NextButton.setOrigin(19.f, 19.f);
        team1NextButton.setPosition(551.f, 172.5f);

        team1NextText.setFont(font);
        team1NextText.setString(">");
        team1NextText.setCharacterSize(22);
        team1NextText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(team1NextText, team1NextButton);

        // === ÉQUIPE 2 ===
        team2Label.setFont(font);
        team2Label.setString("Equipe 2:");
        team2Label.setCharacterSize(22);
        team2Label.setFillColor(sf::Color::White);
        team2Label.setPosition(60, 210);

        team2SelectBox.setSize({420, 45});
        team2SelectBox.setFillColor(sf::Color(70, 80, 100));
        team2SelectBox.setOutlineColor(sf::Color(150, 150, 170));
        team2SelectBox.setOutlineThickness(2);
        team2SelectBox.setPosition(60, 240);

        team2NameText.setFont(font);
        team2NameText.setCharacterSize(19);
        team2NameText.setFillColor(sf::Color::White);
        team2NameText.setPosition(70, 250);

        team2PrevButton.setSize({38, 38});
        team2PrevButton.setFillColor(sf::Color(80, 80, 100));
        team2PrevButton.setOutlineColor(sf::Color(150, 150, 170));
        team2PrevButton.setOutlineThickness(1);
        team2PrevButton.setOrigin(19.f, 19.f);
        team2PrevButton.setPosition(508.f, 262.5f);

        team2PrevText.setFont(font);
        team2PrevText.setString("<");
        team2PrevText.setCharacterSize(22);
        team2PrevText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(team2PrevText, team2PrevButton);

        team2NextButton.setSize({38, 38});
        team2NextButton.setFillColor(sf::Color(80, 80, 100));
        team2NextButton.setOutlineColor(sf::Color(150, 150, 170));
        team2NextButton.setOutlineThickness(1);
        team2NextButton.setOrigin(19.f, 19.f);
        team2NextButton.setPosition(551.f, 262.5f);

        team2NextText.setFont(font);
        team2NextText.setString(">");
        team2NextText.setCharacterSize(22);
        team2NextText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(team2NextText, team2NextButton);

        // === MODE DE JEU ===
        modeLabel.setFont(font);
        modeLabel.setString("Mode de jeu:");
        modeLabel.setCharacterSize(22);
        modeLabel.setFillColor(sf::Color::White);
        modeLabel.setPosition(60, 300);

        pvpButton.setSize({155, 42});
        pvpButton.setFillColor(sf::Color(70, 120, 90));
        pvpButton.setOutlineColor(sf::Color::Yellow);
        pvpButton.setOutlineThickness(3);
        pvpButton.setOrigin(77.5f, 21.f);
        pvpButton.setPosition(137.5f, 351.f);

        pvpText.setFont(font);
        pvpText.setString("Humain vs Humain");
        pvpText.setCharacterSize(15);
        pvpText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(pvpText, pvpButton);

        pveButton.setSize({155, 42});
        pveButton.setFillColor(sf::Color(120, 90, 70));
        pveButton.setOrigin(77.5f, 21.f);
        pveButton.setPosition(302.5f, 351.f);

        pveText.setFont(font);
        pveText.setString("Humain vs IA");
        pveText.setCharacterSize(15);
        pveText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(pveText, pveButton);

        aivsaiButton.setSize({155, 42});
        aivsaiButton.setFillColor(sf::Color(70, 90, 120));
        aivsaiButton.setOrigin(77.5f, 21.f);
        aivsaiButton.setPosition(467.5f, 351.f);

        aivsaiText.setFont(font);
        aivsaiText.setString("IA vs IA");
        aivsaiText.setCharacterSize(15);
        aivsaiText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(aivsaiText, aivsaiButton);

        // === TYPE D'IA EQUIPE 1 (AIvAI mode only) ===
        ai1TypeLabel.setFont(font);
        ai1TypeLabel.setString("IA Equipe 1:");
        ai1TypeLabel.setCharacterSize(22);
        ai1TypeLabel.setFillColor(sf::Color::White);
        ai1TypeLabel.setPosition(60, 395);

        randomAI1Button.setSize({155, 38});
        randomAI1Button.setFillColor(sf::Color(80, 80, 100));
        randomAI1Button.setOutlineColor(sf::Color::Yellow);
        randomAI1Button.setOutlineThickness(2);
        randomAI1Button.setOrigin(77.5f, 19.f);
        randomAI1Button.setPosition(137.5f, 444.f);

        randomAI1Text.setFont(font);
        randomAI1Text.setString("Random");
        randomAI1Text.setCharacterSize(17);
        randomAI1Text.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(randomAI1Text, randomAI1Button);

        heuristicAI1Button.setSize({155, 38});
        heuristicAI1Button.setFillColor(sf::Color(80, 80, 100));
        heuristicAI1Button.setOrigin(77.5f, 19.f);
        heuristicAI1Button.setPosition(302.5f, 444.f);

        heuristicAI1Text.setFont(font);
        heuristicAI1Text.setString("Heuristique");
        heuristicAI1Text.setCharacterSize(17);
        heuristicAI1Text.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(heuristicAI1Text, heuristicAI1Button);

        advancedAI1Button.setSize({155, 38});
        advancedAI1Button.setFillColor(sf::Color(80, 80, 100));
        advancedAI1Button.setOrigin(77.5f, 19.f);
        advancedAI1Button.setPosition(467.5f, 444.f);

        advancedAI1Text.setFont(font);
        advancedAI1Text.setString("Avancee");
        advancedAI1Text.setCharacterSize(17);
        advancedAI1Text.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(advancedAI1Text, advancedAI1Button);

        // === TYPE D'IA EQUIPE 2 (PvE and AIvAI modes) ===
        aiTypeLabel.setFont(font);
        aiTypeLabel.setString("IA Equipe 2:");
        aiTypeLabel.setCharacterSize(22);
        aiTypeLabel.setFillColor(sf::Color::White);
        aiTypeLabel.setPosition(60, 490);

        randomAIButton.setSize({155, 38});
        randomAIButton.setFillColor(sf::Color(80, 80, 100));
        randomAIButton.setOutlineColor(sf::Color::Yellow);
        randomAIButton.setOutlineThickness(2);
        randomAIButton.setOrigin(77.5f, 19.f);
        randomAIButton.setPosition(137.5f, 539.f);

        randomAIText.setFont(font);
        randomAIText.setString("Random");
        randomAIText.setCharacterSize(17);
        randomAIText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(randomAIText, randomAIButton);

        heuristicAIButton.setSize({155, 38});
        heuristicAIButton.setFillColor(sf::Color(80, 80, 100));
        heuristicAIButton.setOrigin(77.5f, 19.f);
        heuristicAIButton.setPosition(302.5f, 539.f);

        heuristicAIText.setFont(font);
        heuristicAIText.setString("Heuristique");
        heuristicAIText.setCharacterSize(17);
        heuristicAIText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(heuristicAIText, heuristicAIButton);

        advancedAIButton.setSize({155, 38});
        advancedAIButton.setFillColor(sf::Color(80, 80, 100));
        advancedAIButton.setOrigin(77.5f, 19.f);
        advancedAIButton.setPosition(467.5f, 539.f);

        advancedAIText.setFont(font);
        advancedAIText.setString("Avancee");
        advancedAIText.setCharacterSize(17);
        advancedAIText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(advancedAIText, advancedAIButton);

        // === STEP BY STEP MODE ===
        aiStepModeButton.setSize({485, 38});
        aiStepModeButton.setFillColor(sf::Color(90, 90, 105));
        aiStepModeButton.setOutlineColor(sf::Color(150, 150, 170));
        aiStepModeButton.setOutlineThickness(2);
        aiStepModeButton.setPosition(60, 585);

        aiStepModeText.setFont(font);
        aiStepModeText.setString("IA Pas a pas : Non");
        aiStepModeText.setCharacterSize(17);
        aiStepModeText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(aiStepModeText, aiStepModeButton);


        // === MESSAGE D'ERREUR ===
        errorText.setFont(font);
        errorText.setCharacterSize(18);
        errorText.setFillColor(sf::Color::Red);
        errorText.setPosition(60, 640);

        // === BOUTONS D'ACTION ===
        backButton.setSize({160, 50});
        backButton.setFillColor(sf::Color(80, 80, 100));
        backButton.setOutlineColor(sf::Color(150, 150, 170));
        backButton.setOutlineThickness(2);
        LayoutHelper::setRelativePosition(backButton, 0.1f, 0.92f);

        backText.setFont(font);
        backText.setString("Back");
        backText.setCharacterSize(22);
        backText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(backText, backButton);

        startButton.setSize({220, 55});
        startButton.setFillColor(sf::Color(70, 150, 70));
        startButton.setOutlineColor(sf::Color(150, 150, 170));
        startButton.setOutlineThickness(2);
        LayoutHelper::setRelativePosition(startButton, 0.9f, 0.92f);

        startText.setFont(font);
        startText.setString("Start");
        startText.setCharacterSize(22);
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
            team1NameText.setString("No Team available");
            team2NameText.setString("No Team available");
            return;
        }

        // Équipe 1
        if (selectedTeam1Index >= 0 && selectedTeam1Index < static_cast<int>(teams.size())) {
            team1NameText.setString(teams[selectedTeam1Index]->getName() +
                                   " (" + std::to_string(teams[selectedTeam1Index]->getCharacters().size()) + " players)");
        }

        // Équipe 2
        if (selectedTeam2Index >= 0 && selectedTeam2Index < static_cast<int>(teams.size())) {
            team2NameText.setString(teams[selectedTeam2Index]->getName() +
                                   " (" + std::to_string(teams[selectedTeam2Index]->getCharacters().size()) + " players)");
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
            return "Please create at least two teams";
        }
        if (teamManager.getTeamCount() == 1) {
            return "Please create a second team";
        }
        if (selectedTeam1Index == selectedTeam2Index) {
            return "Please select two different teams";
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
        window.clear(sf::Color(50, 50, 60)); // Fond gris uniforme

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
        aiStepModeText.setString(aiStepModeLocal ? "AI Step by Step: Yes" : "AI Step by Step: No");
        LayoutHelper::centerTextInRect(aiStepModeText, aiStepModeButton);
    }
} // namespace screen
