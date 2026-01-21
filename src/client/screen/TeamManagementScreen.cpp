#include "TeamManagementScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "LayoutHelper.h"
#include "../../shared/utility/Constants.h"
#include "../../shared/state/Team.h"
#include "../../shared/state/TeamManager.h"
#include <sstream>

namespace screen {

    TeamManagementScreen::TeamManagementScreen()
        : editMode(false), editingTeamId(-1), editingName(false),
          editRerollsCount(0), scrollOffset(0.0f) {
    }

    TeamManagementScreen::~TeamManagementScreen() = default;

    void TeamManagementScreen::init(const std::shared_ptr<state::BloodBowlGame>& gamePtr, ResourceManager* resources) {
        this->resources = resources;
        const float windowWidth = utility::Constants::WINDOW_WIDTH;
        const float windowHeight = utility::Constants::WINDOW_HEIGHT;

        // Get font from ResourceManager
        const sf::Font& font = resources->getFont("fonts/bloodbowl.ttf");

        // Title
        title.setFont(font);
        title.setString("Gestion des Equipes");
        title.setCharacterSize(36);
        title.setFillColor(sf::Color::White);
        LayoutHelper::setRelativePosition(title, 0.5f, 0.05f);

        // Back button
        backButton.setSize({140, 40});
        backButton.setFillColor(sf::Color(80, 80, 100));
        backButton.setOutlineColor(sf::Color(150, 150, 170));
        backButton.setOutlineThickness(2);
        LayoutHelper::setRelativePosition(backButton, 0.08f, 0.92f);  // Raised from bottom

        backText.setFont(font);
        backText.setString("Retour");
        backText.setCharacterSize(18);
        backText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(backText, backButton);

        // Create new team button
        createNewButton.setSize({200, 40});
        createNewButton.setFillColor(sf::Color(70, 100, 150));
        createNewButton.setOutlineColor(sf::Color(150, 150, 170));
        createNewButton.setOutlineThickness(2);
        LayoutHelper::setRelativePosition(createNewButton, 0.5f, 0.92f);  // Raised from bottom

        createNewText.setFont(font);
        createNewText.setString("New Team");
        createNewText.setCharacterSize(18);
        createNewText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(createNewText, createNewButton);

        // Edit mode UI elements
        editModeLabel.setFont(font);
        editModeLabel.setString("Team Editing");
        editModeLabel.setCharacterSize(24);
        editModeLabel.setFillColor(sf::Color::Yellow);
        editModeLabel.setPosition(60, 100);  // More margin from top

        editNameBox.setSize({300, 35});
        editNameBox.setFillColor(sf::Color(60, 60, 70));
        editNameBox.setOutlineColor(sf::Color(150, 150, 170));
        editNameBox.setOutlineThickness(2);
        editNameBox.setOrigin(150, 17.5f);
        editNameBox.setPosition(210, 157.5f);  // Adjusted

        editNameText.setFont(font);
        editNameText.setCharacterSize(18);
        editNameText.setFillColor(sf::Color::White);
        editNameText.setPosition(65, 145);  // Adjusted

        rerollsLabel.setFont(font);
        rerollsLabel.setString("Relances:");
        rerollsLabel.setCharacterSize(20);
        rerollsLabel.setFillColor(sf::Color::White);
        rerollsLabel.setPosition(60, 200);  // Adjusted

        rerollsMinusButton.setSize({35, 35});
        rerollsMinusButton.setFillColor(sf::Color(150, 70, 70));
        rerollsMinusButton.setOrigin(17.5f, 17.5f);
        rerollsMinusButton.setPosition(77.5f, 247.5f);  // Adjusted

        rerollsMinusText.setFont(font);
        rerollsMinusText.setString("-");
        rerollsMinusText.setCharacterSize(24);
        rerollsMinusText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(rerollsMinusText, rerollsMinusButton);

        rerollsCountText.setFont(font);
        rerollsCountText.setCharacterSize(20);
        rerollsCountText.setFillColor(sf::Color::Yellow);
        rerollsCountText.setPosition(110, 235);  // Adjusted

        rerollsPlusButton.setSize({35, 35});
        rerollsPlusButton.setFillColor(sf::Color(70, 150, 70));
        rerollsPlusButton.setOrigin(17.5f, 17.5f);
        rerollsPlusButton.setPosition(167.5f, 247.5f);  // Adjusted

        rerollsPlusText.setFont(font);
        rerollsPlusText.setString("+");
        rerollsPlusText.setCharacterSize(24);
        rerollsPlusText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(rerollsPlusText, rerollsPlusButton);

        saveEditButton.setSize({140, 40});
        saveEditButton.setFillColor(sf::Color(70, 150, 70));
        saveEditButton.setOrigin(70, 20);
        saveEditButton.setPosition(130, 310);  // Adjusted

        saveEditText.setFont(font);
        saveEditText.setString("Sauvegarder");
        saveEditText.setCharacterSize(18);
        saveEditText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(saveEditText, saveEditButton);

        cancelEditButton.setSize({120, 40});
        cancelEditButton.setFillColor(sf::Color(150, 70, 70));
        cancelEditButton.setOrigin(60, 20);
        cancelEditButton.setPosition(280, 310);  // Adjusted

        cancelEditText.setFont(font);
        cancelEditText.setString("Annuler");
        cancelEditText.setCharacterSize(18);
        cancelEditText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(cancelEditText, cancelEditButton);
    }

    void TeamManagementScreen::setManager(SceneManager* mgr) {
        manager = mgr;
    }

    void TeamManagementScreen::onEnter() {
        // Reload teams from disk and refresh the list when entering the screen
        loadTeamsFromFile();
        refreshTeamList();
    }

    void TeamManagementScreen::refreshTeamList() {
        teamBoxes.clear();
        teamNameTexts.clear();
        editButtons.clear();
        editTexts.clear();
        deleteButtons.clear();
        deleteTexts.clear();
        teamInfoTexts.clear();

        auto& teamManager = state::TeamManager::getInstance();
        const auto& teams = teamManager.getSavedTeams();

        // Get font from ResourceManager
        const sf::Font& font = resources->getFont("fonts/bloodbowl.ttf");

        const float windowWidth = utility::Constants::WINDOW_WIDTH;
        const float startY = 170.0f;
        const float itemHeight = 105.0f;
        const float boxWidth = 800.0f;  // Réduit pour éviter le débordement
        const float boxHeight = 90.0f;

        for (size_t i = 0; i < teams.size(); ++i) {
            const auto& team = teams[i];
            float yPos = startY + static_cast<float>(i) * itemHeight + scrollOffset;

            // Team box - centré
            sf::RectangleShape box;
            box.setSize({boxWidth, boxHeight});
            box.setFillColor(sf::Color(70, 80, 100));
            box.setOutlineColor(sf::Color(150, 150, 170));
            box.setOutlineThickness(2);
            box.setOrigin(boxWidth / 2, boxHeight / 2);
            box.setPosition(windowWidth / 2, yPos + boxHeight / 2);
            teamBoxes.push_back(box);

            // Team name
            sf::Text nameText;
            nameText.setFont(font);
            nameText.setString(team->getName());
            nameText.setCharacterSize(24);
            nameText.setFillColor(sf::Color::White);
            nameText.setStyle(sf::Text::Bold);
            nameText.setPosition(windowWidth / 2 - boxWidth / 2 + 15, yPos + 10);
            teamNameTexts.push_back(nameText);

            // Team info
            sf::Text infoText;
            infoText.setFont(font);
            std::stringstream ss;
            ss << "Players: " << team->getCharacters().size()
               << " | Rerolls: " << team->getRerolls()
               << " | Score: " << team->getScore();
            infoText.setString(ss.str());
            infoText.setCharacterSize(17);
            infoText.setFillColor(sf::Color(200, 200, 200));
            infoText.setPosition(windowWidth / 2 - boxWidth / 2 + 15, yPos + 45);
            teamInfoTexts.push_back(infoText);

            // Edit button - décalé vers la gauche pour rester dans le cadre
            sf::RectangleShape editBtn;
            editBtn.setSize({95, 35});
            editBtn.setFillColor(sf::Color(70, 100, 150));
            editBtn.setOutlineColor(sf::Color(150, 150, 170));
            editBtn.setOutlineThickness(1);
            editBtn.setOrigin(47.5f, 17.5f);
            editBtn.setPosition(windowWidth / 2 + boxWidth / 2 - 180, yPos + boxHeight / 2);
            editButtons.push_back(editBtn);

            sf::Text editTxt;
            editTxt.setFont(font);
            editTxt.setString("Edit");
            editTxt.setCharacterSize(17);
            editTxt.setFillColor(sf::Color::White);
            LayoutHelper::centerTextInRect(editTxt, editBtn);
            editTexts.push_back(editTxt);

            // Delete button - décalé vers la gauche pour rester dans le cadre
            sf::RectangleShape delBtn;
            delBtn.setSize({105, 35});
            delBtn.setFillColor(sf::Color(150, 70, 70));
            delBtn.setOutlineColor(sf::Color(150, 150, 170));
            delBtn.setOutlineThickness(1);
            delBtn.setOrigin(52.5f, 17.5f);
            delBtn.setPosition(windowWidth / 2 + boxWidth / 2 - 70, yPos + boxHeight / 2);
            deleteButtons.push_back(delBtn);

            sf::Text delTxt;
            delTxt.setFont(font);
            delTxt.setString("Delete");
            delTxt.setCharacterSize(17);
            delTxt.setFillColor(sf::Color::White);
            LayoutHelper::centerTextInRect(delTxt, delBtn);
            deleteTexts.push_back(delTxt);
        }
    }

    void TeamManagementScreen::enterEditMode(int teamId) {
        auto& teamManager = state::TeamManager::getInstance();
        state::Team* team = teamManager.getTeamById(teamId);

        if (team) {
            editMode = true;
            editingTeamId = teamId;
            editNameInput = team->getName();
            editNameText.setString(editNameInput);
            editRerollsCount = team->getRerolls();
            rerollsCountText.setString(std::to_string(editRerollsCount));
            editingName = false;
            editNameBox.setOutlineColor(sf::Color::White);
        }
    }

    void TeamManagementScreen::exitEditMode() {
        editMode = false;
        editingTeamId = -1;
        editingName = false;
        editNameBox.setOutlineColor(sf::Color::White);
    }

    void TeamManagementScreen::saveEdit() {
        auto& teamManager = state::TeamManager::getInstance();
        if (teamManager.updateTeam(editingTeamId, editNameInput, editRerollsCount)) {
            saveTeamsToFile(); // Sauvegarder sur le disque
            refreshTeamList();
            exitEditMode();
        }
    }

    void TeamManagementScreen::deleteTeam(int teamId) {
        auto& teamManager = state::TeamManager::getInstance();
        if (teamManager.deleteTeam(teamId)) {
            refreshTeamList();
            if (editingTeamId == teamId) {
                exitEditMode();
            }
        }
    }

    void TeamManagementScreen::saveTeamsToFile() {
        auto& teamManager = state::TeamManager::getInstance();
        teamManager.saveToDisk("teams.json");
    }

    void TeamManagementScreen::loadTeamsFromFile() {
        auto& teamManager = state::TeamManager::getInstance();
        teamManager.loadFromDisk("teams.json");
    }

    void TeamManagementScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mpos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

            // Back button
            if (backButton.getGlobalBounds().contains(mpos)) {
                if (editMode) {
                    // En mode édition, retour au panneau de gestion
                    exitEditMode();
                } else {
                    // Sinon, retour au menu principal
                    saveTeamsToFile(); // Auto-save when leaving
                    if (manager) manager->switchTo(render::SceneId::HOME);
                }
                return;
            }

            // Create new team button (seulement visible en mode liste)
            if (!editMode && createNewButton.getGlobalBounds().contains(mpos)) {
                saveTeamsToFile(); // Auto-save before creating new team
                if (manager) manager->switchTo(render::SceneId::TEAM_CREATION);
                return;
            }

            if (editMode) {
                // Edit mode buttons
                if (editNameBox.getGlobalBounds().contains(mpos)) {
                    editingName = true;
                    editNameBox.setOutlineColor(sf::Color::Yellow);
                } else {
                    editingName = false;
                    editNameBox.setOutlineColor(sf::Color::White);
                }

                if (rerollsMinusButton.getGlobalBounds().contains(mpos)) {
                    if (editRerollsCount > 0) {
                        editRerollsCount--;
                        rerollsCountText.setString(std::to_string(editRerollsCount));
                    }
                }

                if (rerollsPlusButton.getGlobalBounds().contains(mpos)) {
                    if (editRerollsCount < 10) {
                        editRerollsCount++;
                        rerollsCountText.setString(std::to_string(editRerollsCount));
                    }
                }

                if (saveEditButton.getGlobalBounds().contains(mpos)) {
                    saveEdit();
                }

                if (cancelEditButton.getGlobalBounds().contains(mpos)) {
                    exitEditMode();
                }
            } else {
                // Team list buttons
                auto& teamManager = state::TeamManager::getInstance();
                const auto& teams = teamManager.getSavedTeams();

                for (size_t i = 0; i < teams.size() && i < editButtons.size(); ++i) {
                    // Edit button
                    if (editButtons[i].getGlobalBounds().contains(mpos)) {
                        enterEditMode(teams[i]->getTeamId());
                        return;
                    }

                    // Delete button
                    if (deleteButtons[i].getGlobalBounds().contains(mpos)) {
                        deleteTeam(teams[i]->getTeamId());
                        return;
                    }
                }
            }
        }

        // Text input for team name in edit mode
        if (editMode && editingName && event.type == sf::Event::TextEntered) {
            if (event.text.unicode == 8) { // Backspace
                if (!editNameInput.empty()) {
                    editNameInput.pop_back();
                }
            } else if (event.text.unicode < 128 && event.text.unicode >= 32) {
                if (editNameInput.length() < 30) {
                    editNameInput += static_cast<char>(event.text.unicode);
                }
            }
            // Update the text content (font is set in draw())
            editNameText.setString(editNameInput);
        }

        // Mouse wheel scrolling
        if (event.type == sf::Event::MouseWheelScrolled) {
            scrollOffset += event.mouseWheelScroll.delta * 20.0f;
            // Limit scrolling
            if (scrollOffset > 0) scrollOffset = 0;
            refreshTeamList();
        }
    }

    void TeamManagementScreen::update() {
        // Nothing to update per frame
    }

    void TeamManagementScreen::draw(sf::RenderWindow& window) {
        window.clear(sf::Color(50, 50, 60)); // Uniform gray background

        // Get font from ResourceManager for drawing
        const sf::Font& font = resources->getFont("fonts/bloodbowl.ttf");

        if (editMode) {
            // Draw edit mode UI
            window.draw(title);
            window.draw(editModeLabel);
            window.draw(editNameBox);

            // Update font before drawing to avoid dangling reference
            editNameText.setFont(font);
            window.draw(editNameText);
            window.draw(rerollsLabel);
            window.draw(rerollsMinusButton);
            window.draw(rerollsMinusText);
            window.draw(rerollsCountText);
            window.draw(rerollsPlusButton);
            window.draw(rerollsPlusText);
            window.draw(saveEditButton);
            window.draw(saveEditText);
            window.draw(cancelEditButton);
            window.draw(cancelEditText);
        } else {
            // Draw team list
            window.draw(title);

            for (size_t i = 0; i < teamBoxes.size(); ++i) {
                window.draw(teamBoxes[i]);
                window.draw(teamNameTexts[i]);
                window.draw(teamInfoTexts[i]);
                window.draw(editButtons[i]);
                window.draw(editTexts[i]);
                window.draw(deleteButtons[i]);
                window.draw(deleteTexts[i]);
            }

            window.draw(createNewButton);
            window.draw(createNewText);
        }

        window.draw(backButton);
        window.draw(backText);
    }

    render::SceneId TeamManagementScreen::getId() const {
        return render::SceneId::TEAM_MANAGEMENT;
    }

} // namespace screen

