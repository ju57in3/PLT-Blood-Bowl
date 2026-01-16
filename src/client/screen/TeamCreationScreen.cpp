#include "TeamCreationScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "LayoutHelper.h"
#include "../../shared/utility/Constants.h"
#include "../../shared/state/Team.h"
#include "../../shared/state/Character.h"
#include "../../shared/state/TeamManager.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace screen {
    TeamCreationScreen::TeamCreationScreen()
        : editingName(false), selectedRace(""), totalPlayers(0) {
    }

    TeamCreationScreen::~TeamCreationScreen() = default;

    void TeamCreationScreen::init(const std::shared_ptr<state::BloodBowlGame> &gamePtr, ResourceManager *resources) {
        this->resources = resources;
        const float windowWidth = utility::Constants::WINDOW_WIDTH;
        const float windowHeight = utility::Constants::WINDOW_HEIGHT;

        try { font = resources->getFont("fonts/bloodbowl.ttf"); } catch (...) {
        }

        // Titre principal
        title.setFont(font);
        title.setString("Creation d'equipe");
        title.setCharacterSize(32);
        title.setFillColor(sf::Color::White);
        title.setPosition(windowWidth / 2 - 150, 20);

        // === NOM DE L'ÉQUIPE ===
        teamNameLabel.setFont(font);
        teamNameLabel.setString("Nom de l'equipe:");
        teamNameLabel.setCharacterSize(20);
        teamNameLabel.setFillColor(sf::Color::White);
        teamNameLabel.setPosition(50, 80);

        teamNameBox.setSize({300, 35});
        teamNameBox.setFillColor(sf::Color(50, 50, 50));
        teamNameBox.setOutlineColor(sf::Color::White);
        teamNameBox.setOutlineThickness(2);
        teamNameBox.setPosition(50, 110);

        teamNameText.setFont(font);
        teamNameText.setString("Mon Equipe");
        teamNameText.setCharacterSize(18);
        teamNameText.setFillColor(sf::Color::White);
        teamNameText.setPosition(55, 115);
        teamName = "Mon Equipe";

        // === SÉLECTION DE RACE ===
        raceLabel.setFont(font);
        raceLabel.setString("Race:");
        raceLabel.setCharacterSize(20);
        raceLabel.setFillColor(sf::Color::White);
        raceLabel.setPosition(50, 160);

        humanRaceButton.setSize({120, 40});
        humanRaceButton.setFillColor(sf::Color(70, 100, 150));
        humanRaceButton.setPosition(50, 190);

        humanRaceText.setFont(font);
        humanRaceText.setString("Humains");
        humanRaceText.setCharacterSize(18);
        humanRaceText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(humanRaceText, humanRaceButton);

        orcRaceButton.setSize({120, 40});
        orcRaceButton.setFillColor(sf::Color(100, 150, 70));
        orcRaceButton.setPosition(180, 190);

        orcRaceText.setFont(font);
        orcRaceText.setString("Orcs");
        orcRaceText.setCharacterSize(18);
        orcRaceText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(orcRaceText, orcRaceButton);

        // === JOUEURS ===
        playersLabel.setFont(font);
        playersLabel.setString("Joueurs (11-16):");
        playersLabel.setCharacterSize(20);
        playersLabel.setFillColor(sf::Color::White);
        playersLabel.setPosition(50, 250);

        playerCountText.setFont(font);
        playerCountText.setCharacterSize(18);
        playerCountText.setFillColor(sf::Color::Yellow);
        playerCountText.setPosition(50, 280);

        // === RÉSUMÉ ===
        summaryText.setFont(font);
        summaryText.setCharacterSize(16);
        summaryText.setFillColor(sf::Color(200, 200, 200));
        summaryText.setPosition(windowWidth - 350, 100);

        // === BOUTONS D'ACTION ===
        backButton.setSize({140, 40});
        backButton.setFillColor(sf::Color(70, 70, 70));
        backButton.setPosition(20, windowHeight - 60);

        backText.setFont(font);
        backText.setString("Retour");
        backText.setCharacterSize(18);
        backText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(backText, backButton);

        validateButton.setSize({160, 40});
        validateButton.setFillColor(sf::Color(50, 150, 50));
        validateButton.setPosition(windowWidth - 180, windowHeight - 60);

        validateText.setFont(font);
        validateText.setString("Valider");
        validateText.setCharacterSize(18);
        validateText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(validateText, validateButton);
    }

    void TeamCreationScreen::setManager(SceneManager *mgr) { manager = mgr; }

    void TeamCreationScreen::initPlayerButtons() {
        playerButtons.clear();
        playerButtonTexts.clear();
        playerTypes.clear();
        playerCounts.clear();

        if (selectedRace == "human") {
            playerTypes = {"Blitzer", "Catcher", "Thrower", "Lineman"};
        } else if (selectedRace == "orc") {
            playerTypes = {"Blitzer", "Black Orc", "Thrower", "Lineman"};
        }

        float startY = 310;
        for (size_t i = 0; i < playerTypes.size(); ++i) {
            playerCounts.push_back(0);

            // Bouton -
            sf::RectangleShape minusBtn;
            minusBtn.setSize({35, 35});
            minusBtn.setFillColor(sf::Color(150, 50, 50));
            minusBtn.setPosition(50, startY + i * 45);
            playerButtons.push_back(minusBtn);

            sf::Text minusText;
            minusText.setFont(font);
            minusText.setString("-");
            minusText.setCharacterSize(24);
            minusText.setFillColor(sf::Color::White);
            LayoutHelper::centerTextInRect(minusText, minusBtn);
            playerButtonTexts.push_back(minusText);

            // Label du type
            sf::Text typeText;
            typeText.setFont(font);
            typeText.setString(playerTypes[i]);
            typeText.setCharacterSize(18);
            typeText.setFillColor(sf::Color::White);
            typeText.setPosition(95, startY + i * 45 + 8);
            playerButtonTexts.push_back(typeText);

            // Compteur
            sf::Text countText;
            countText.setFont(font);
            countText.setString("0");
            countText.setCharacterSize(18);
            countText.setFillColor(sf::Color::Yellow);
            countText.setPosition(220, startY + i * 45 + 8);
            playerButtonTexts.push_back(countText);

            // Bouton +
            sf::RectangleShape plusBtn;
            plusBtn.setSize({35, 35});
            plusBtn.setFillColor(sf::Color(50, 150, 50));
            plusBtn.setPosition(260, startY + i * 45);
            playerButtons.push_back(plusBtn);

            sf::Text plusText;
            plusText.setFont(font);
            plusText.setString("+");
            plusText.setCharacterSize(24);
            plusText.setFillColor(sf::Color::White);
            LayoutHelper::centerTextInRect(plusText, plusBtn);
            playerButtonTexts.push_back(plusText);
        }
    }

    void TeamCreationScreen::handleEvent(const sf::Event &event, sf::RenderWindow &window) {
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mpos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

            // Bouton retour
            if (backButton.getGlobalBounds().contains(mpos)) {
                if (manager) manager->switchTo(render::SceneId::HOME);
                return;
            }

            // Bouton valider
            if (validateButton.getGlobalBounds().contains(mpos) && canValidate()) {
                createTeam();
                if (manager) manager->switchTo(render::SceneId::TEAM_MANAGEMENT);
                return;
            }

            // Champ nom
            if (teamNameBox.getGlobalBounds().contains(mpos)) {
                editingName = true;
                teamNameBox.setOutlineColor(sf::Color::Yellow);
                return;
            } else {
                editingName = false;
                teamNameBox.setOutlineColor(sf::Color::White);
            }

            // Sélection de race
            if (humanRaceButton.getGlobalBounds().contains(mpos)) {
                selectedRace = "human";
                humanRaceButton.setOutlineThickness(3);
                humanRaceButton.setOutlineColor(sf::Color::Yellow);
                orcRaceButton.setOutlineThickness(0);
                initPlayerButtons();
                return;
            }

            if (orcRaceButton.getGlobalBounds().contains(mpos)) {
                selectedRace = "orc";
                orcRaceButton.setOutlineThickness(3);
                orcRaceButton.setOutlineColor(sf::Color::Yellow);
                humanRaceButton.setOutlineThickness(0);
                initPlayerButtons();
                return;
            }

            // Boutons +/-
            if (!selectedRace.empty()) {
                for (size_t i = 0; i < playerTypes.size(); ++i) {
                    // Bouton - (index pair dans playerButtons)
                    if (playerButtons[i * 2].getGlobalBounds().contains(mpos)) {
                        if (playerCounts[i] > 0) {
                            playerCounts[i]--;
                            totalPlayers--;
                        }
                    }
                    // Bouton + (index impair dans playerButtons)
                    if (playerButtons[i * 2 + 1].getGlobalBounds().contains(mpos)) {
                        if (totalPlayers < 16) {
                            playerCounts[i]++;
                            totalPlayers++;
                        }
                    }
                }
            }
        }

        // Saisie du nom d'équipe
        if (editingName && event.type == sf::Event::TextEntered) {
            if (event.text.unicode == 8) { // Backspace
                if (!teamName.empty()) {
                    teamName.pop_back();
                }
            } else if (event.text.unicode < 128 && event.text.unicode >= 32) {
                if (teamName.length() < 20) {
                    teamName += static_cast<char>(event.text.unicode);
                }
            }
            teamNameText.setString(teamName);
        }
    }

    void TeamCreationScreen::update() {
        updateSummary();

        // Mettre à jour le compteur de joueurs
        totalPlayers = 0;
        for (int count : playerCounts) {
            totalPlayers += count;
        }

        std::string countStr = "Total: " + std::to_string(totalPlayers) + " / 16";
        if (totalPlayers < 11) {
            countStr += " (min 11)";
            playerCountText.setFillColor(sf::Color::Red);
        } else {
            playerCountText.setFillColor(sf::Color::Green);
        }
        playerCountText.setString(countStr);

        // Mise à jour des compteurs individuels
        for (size_t i = 0; i < playerCounts.size(); ++i) {
            playerButtonTexts[i * 4 + 2].setString(std::to_string(playerCounts[i]));
        }

        // État du bouton valider
        if (canValidate()) {
            validateButton.setFillColor(sf::Color(50, 150, 50));
        } else {
            validateButton.setFillColor(sf::Color(100, 100, 100));
        }
    }

    void TeamCreationScreen::updateSummary() {
        std::stringstream ss;
        ss << "=== Resume ===\n\n";
        ss << "Nom: " << teamName << "\n\n";

        if (!selectedRace.empty()) {
            ss << "Race: " << (selectedRace == "human" ? "Humains" : "Orcs") << "\n\n";
            ss << "Composition:\n";
            for (size_t i = 0; i < playerTypes.size(); ++i) {
                if (playerCounts[i] > 0) {
                    ss << "  " << playerTypes[i] << ": " << playerCounts[i] << "\n";
                }
            }
            ss << "\nTotal: " << totalPlayers << " joueurs";
        } else {
            ss << "Choisissez une race";
        }

        summaryText.setString(ss.str());
    }

    bool TeamCreationScreen::canValidate() const {
        return !teamName.empty() &&
               !selectedRace.empty() &&
               totalPlayers >= 11 &&
               totalPlayers <= 16;
    }

    void TeamCreationScreen::createTeam() {
        // Créer l'équipe
        auto& teamManager = state::TeamManager::getInstance();
        int teamId = teamManager.getNextTeamId();

        auto newTeam = std::make_unique<state::Team>(teamId, teamName, 3);

        // Ajouter les personnages
        int characterId = 1;
        std::string racePrefix = selectedRace == "human" ? "human" : "orc";

        for (size_t i = 0; i < playerTypes.size(); ++i) {
            for (int j = 0; j < playerCounts[i]; ++j) {
                std::string typeName = playerTypes[i];
                std::transform(typeName.begin(), typeName.end(), typeName.begin(), ::tolower);

                // Remplacer les espaces
                std::replace(typeName.begin(), typeName.end(), ' ', '_');

                std::string fullType = racePrefix + "_" + typeName;
                std::string charName = playerTypes[i] + std::to_string(j + 1);

                // Statistiques par défaut (peuvent être personnalisées selon le type)
                int movement = 6;
                int strength = 3;
                int agility = 3;
                int armor = 8;

                // Ajuster selon le type
                if (typeName == "blitzer") {
                    strength = 4;
                } else if (typeName == "black_orc") {
                    strength = 4;
                    movement = 4;
                }

                auto character = std::make_shared<state::Character>(
                    characterId++, charName, fullType, movement, strength, agility, armor
                );
                character->setStatus(state::bench);
                newTeam->addCharacter(character);
            }
        }

        teamManager.saveTeam(std::move(newTeam));

        // Auto-save to disk
        teamManager.saveToDisk("teams.json");
    }

    void TeamCreationScreen::draw(sf::RenderWindow &window) {
        window.clear(sf::Color(30, 30, 30));

        window.draw(title);

        // Nom de l'équipe
        window.draw(teamNameLabel);
        window.draw(teamNameBox);
        window.draw(teamNameText);

        // Sélection de race
        window.draw(raceLabel);
        window.draw(humanRaceButton);
        window.draw(humanRaceText);
        window.draw(orcRaceButton);
        window.draw(orcRaceText);

        // Joueurs
        if (!selectedRace.empty()) {
            window.draw(playersLabel);
            window.draw(playerCountText);

            for (auto& btn : playerButtons) {
                window.draw(btn);
            }
            for (auto& txt : playerButtonTexts) {
                window.draw(txt);
            }
        }

        // Résumé
        window.draw(summaryText);

        // Boutons d'action
        window.draw(backButton);
        window.draw(backText);
        window.draw(validateButton);
        window.draw(validateText);
    }

    render::SceneId TeamCreationScreen::getId() const {
        return render::SceneId::TEAM_CREATION;
    }
} // namespace screen
