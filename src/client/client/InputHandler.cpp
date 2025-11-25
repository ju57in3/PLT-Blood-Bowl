#include "InputHandler.h"

#include "utility/Constants.h"
#include "engine/Engine.h"
#include "engine/Move.h"
#include "engine/Pass.h"
#include "engine/Block.h"
#include "state/BloodBowlGame.h"
#include "state/Character.h"
#include <iostream>
#include <utility>

namespace client {

    InputHandler::InputHandler(std::shared_ptr<state::BloodBowlGame> game, engine::Engine* engine)
        : currentMode(Idle), selectedCharacter(nullptr), game(std::move(game)), engine(engine), pendingBlock(nullptr), previewBoardPos({0,0}), previewExists(false), previewCharacter(nullptr), previewIsLegal(false) {
    }

    InputHandler::~InputHandler() = default;


    std::pair<int, int> InputHandler::screenToBoard(const sf::Vector2i& screenPos) const {
        // Convert screen coordinates to board coordinates
        const int stride = utility::Constants::BOARD_TILE_PIXEL_SIZE + utility::Constants::BOARD_TILE_SPACING;
        int boardX = (screenPos.x - utility::Constants::BOARD_OFFSET_X) / stride;
        int boardY = (screenPos.y - utility::Constants::BOARD_OFFSET_Y) / stride;

        // Clamp to board bounds
        if (boardX < 0) boardX = 0;
        if (boardX >= utility::Constants::BOARD_WIDTH) boardX = utility::Constants::BOARD_WIDTH - 1;
        if (boardY < 0) boardY = 0;
        if (boardY >= utility::Constants::BOARD_HEIGHT) boardY = utility::Constants::BOARD_HEIGHT - 1;

        return {boardX, boardY};
    }

    std::shared_ptr<state::Character> InputHandler::getCharacterAt(const std::pair<int, int>& boardPos) const {
        if (!game) return nullptr;

        // Check current team first (only current team can be selected)
        auto* currentTeam = game->getCurrentTeam();
        if (!currentTeam) return nullptr;

        for (const auto& character : currentTeam->getCharacters()) {
            if (character->getPosition() == boardPos) {
                return character;
            }
        }

        // If not found in current team, check opponent team (for blocking/passing targets)
        auto& opponentTeam = (currentTeam == &game->getTeamA()) ? game->getTeamB() : game->getTeamA();
        for (const auto& character : opponentTeam.getCharacters()) {
            if (character->getPosition() == boardPos) {
                return character;
            }
        }

        return nullptr;
    }

    void InputHandler::resetSelection() {
        selectedCharacter = nullptr;
        currentMode = InputMode::Idle;
        std::cout << "Selection reset. Mode: IDLE\n";
    }

    void InputHandler::handleMouseClick(const sf::Event::MouseButtonEvent& mouseButton, sf::RenderWindow* window) {
        if (!window || !game || !engine) return;

        if (mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mousePos(mouseButton.x, mouseButton.y);
            auto boardPos = screenToBoard(mousePos);

            std::cout << "Click at screen (" << mouseButton.x << ", " << mouseButton.y
                      << ") -> board (" << boardPos.first << ", " << boardPos.second << ")\n";

            switch (currentMode) {
                case Idle:
                case Selected_Character: {
                    // Try to select a character from current team
                    auto character = getCharacterAt(boardPos);
                    if (character) {
                        auto* currentTeam = game->getCurrentTeam();
                        // Check if character belongs to current team
                        bool belongsToCurrentTeam = false;
                        for (const auto& c : currentTeam->getCharacters()) {
                            if (c == character) {
                                belongsToCurrentTeam = true;
                                break;
                            }
                        }

                        if (belongsToCurrentTeam) {
                            selectedCharacter = character;
                            currentMode = InputMode::Selected_Character;
                            std::cout << "Character selected: " << character->getName()
                                      << " at (" << character->getPosition().first
                                      << ", " << character->getPosition().second << ")\n";
                        } else {
                            std::cout << "Cannot select opponent's character\n";
                        }
                    } else {
                        std::cout << "No character at this position\n";
                    }
                    break;
                }

                case Select_Move_Target: {
                    if (selectedCharacter) {
                        std::cout << "Creating Move command to (" << boardPos.first << ", " << boardPos.second << ")\n";
                        auto moveCmd = std::make_unique<engine::Move>(selectedCharacter, boardPos);
                        engine->addCommand(std::move(moveCmd));
                        engine->executeCommand();
                        std::cout << "Move command executed\n";
                    }
                    resetSelection();
                    break;
                }

                case Select_Pass_Target: {
                    if (selectedCharacter) {
                        auto targetCharacter = getCharacterAt(boardPos);
                        if (targetCharacter) {
                            std::cout << "Creating Pass command to " << targetCharacter->getName() << "\n";
                            auto passCmd = std::make_unique<engine::Pass>(selectedCharacter, targetCharacter);
                            engine->addCommand(std::move(passCmd));
                            engine->executeCommand();
                            std::cout << "Pass command executed\n";
                        } else {
                            std::cout << "No character at target position for pass\n";
                        }
                    }
                    resetSelection();
                    break;
                }

                case Select_Block_Target: {
                    if (selectedCharacter) {
                        auto targetCharacter = getCharacterAt(boardPos);
                        if (targetCharacter && targetCharacter != selectedCharacter) {
                            // Create a pending block and expose dice options to the UI
                            pendingBlock = std::make_unique<engine::Block>(selectedCharacter, targetCharacter);
                            auto options = pendingBlock->getDiceOptions();
                            std::cout << "Block initiated against " << targetCharacter->getName() << ". Dice options: ";
                            for (size_t i = 0; i < options.size(); ++i) {
                                std::cout << "[" << (i+1) << ":" << options[i] << "] ";
                            }
                            std::cout << "\nPress the number key corresponding to the die to choose (or ESC to cancel)." << std::endl;
                        } else {
                            std::cout << "Invalid block target\n";
                        }
                    }
                    resetSelection();
                    break;
                }
            }
        } else if (mouseButton.button == sf::Mouse::Right) {
            // Right click: if a character is selected
            sf::Vector2i mousePos(mouseButton.x, mouseButton.y);
            auto boardPos = screenToBoard(mousePos);

            auto targetCharacter = getCharacterAt(boardPos);

            if (selectedCharacter) {
                // determine whether targetCharacter belongs to current team
                bool targetBelongsToCurrent = false;
                auto* currentTeam = game->getCurrentTeam();
                if (currentTeam) {
                    for (const auto& c : currentTeam->getCharacters()) {
                        if (c == targetCharacter) { targetBelongsToCurrent = true; break; }
                    }
                }

                if (targetCharacter && !targetBelongsToCurrent && targetCharacter != selectedCharacter) {
                    // Right-click on opponent -> start pending Block (wait for dice choice)
                    pendingBlock = std::make_unique<engine::Block>(selectedCharacter, targetCharacter);
                    auto options = pendingBlock->getDiceOptions();
                    std::cout << "Block initiated against " << targetCharacter->getName() << ". Dice options: ";
                    for (size_t i = 0; i < options.size(); ++i) {
                        std::cout << "[" << (i+1) << ":" << options[i] << "] ";
                    }
                    std::cout << "\nPress the number key corresponding to the die to choose (or ESC to cancel)." << std::endl;
                    // keep selection active until player chooses
                    return;
                }

                // Right-click on empty tile -> Move
                if (!targetCharacter) {
                    std::cout << "Creating Move command to (" << boardPos.first << ", " << boardPos.second << ")\n";
                    auto moveCmd = std::make_unique<engine::Move>(selectedCharacter, boardPos);
                    engine->addCommand(std::move(moveCmd));
                    engine->executeCommand();
                    std::cout << "Move command executed\n";
                    resetSelection();
                    return;
                }
            } else {
                std::cout << "No character selected. Left click to select a character first.\n";
            }
        }
    }

    std::vector<int> InputHandler::getPendingBlockDiceOptions() const {
        if (pendingBlock) return pendingBlock->getDiceOptions();
        return {};
    }

    bool InputHandler::hasPendingBlock() const {
        return pendingBlock != nullptr;
    }

    std::pair<int,int> InputHandler::getPreviewPosition() const {
        return previewBoardPos;
    }

    bool InputHandler::hasPreviewPosition() const {
        return previewExists;
    }

    std::shared_ptr<state::Character> InputHandler::getPreviewCharacter() const {
        return previewCharacter;
    }

    bool InputHandler::isPreviewLegal() const {
        return previewIsLegal;
    }

    void InputHandler::updatePreviewFromMouse(const sf::Vector2i& mousePos) {
        if (!game) { previewExists = false; previewIsLegal = false; previewCharacter = nullptr; return; }
        auto boardPos = screenToBoard(mousePos);
        previewBoardPos = boardPos;
        previewExists = true;
        previewCharacter = getCharacterAt(boardPos);
        previewIsLegal = false;

        switch (currentMode) {
            case InputMode::Idle:
            case InputMode::Selected_Character:
                previewIsLegal = false;
                break;
            case InputMode::Select_Move_Target:
                if (selectedCharacter) {
                    previewIsLegal = isMoveLegal(boardPos);
                }
                break;
            case InputMode::Select_Pass_Target:
                if (selectedCharacter && previewCharacter) {
                    previewIsLegal = (previewCharacter != selectedCharacter);
                }
                break;
            case InputMode::Select_Block_Target:
                if (selectedCharacter && previewCharacter) {
                    previewIsLegal = isBlockLegal(previewCharacter);
                }
                break;
        }
    }

    void InputHandler::handleKeyPress(const sf::Event::KeyEvent& key) {
        switch (key.code) {
            case sf::Keyboard::M:
                if (currentMode == InputMode::Selected_Character && selectedCharacter) {
                    currentMode = InputMode::Select_Move_Target;
                    std::cout << "Move mode: Click on target position\n";
                }
                break;

            case sf::Keyboard::P:
                if (currentMode == InputMode::Selected_Character && selectedCharacter) {
                    currentMode = InputMode::Select_Pass_Target;
                    std::cout << "Pass mode: Click on target character\n";
                }
                break;

            case sf::Keyboard::B:
                if (currentMode == InputMode::Selected_Character && selectedCharacter) {
                    currentMode = InputMode::Select_Block_Target;
                    std::cout << "Block mode: Click on target character\n";
                }
                break;

            case sf::Keyboard::Escape:
                // cancel pending block if any
                pendingBlock.reset();
                resetSelection();
                break;

            default:
                // handle numeric keys for block dice choice (1..6)
                if (pendingBlock) {
                    int index = -1;
                    if (key.code >= sf::Keyboard::Num0 && key.code <= sf::Keyboard::Num9) {
                        index = key.code - sf::Keyboard::Num0; // 0..9
                    } else if (key.code >= sf::Keyboard::Numpad0 && key.code <= sf::Keyboard::Numpad9) {
                        index = key.code - sf::Keyboard::Numpad0; // 0..9
                    }
                    if (index > 0) {
                        // apply choice and execute
                        pendingBlock->applyDiceChoice(index);
                        engine->addCommand(std::unique_ptr<engine::Command>(pendingBlock.release()));
                        engine->executeCommand();
                        std::cout << "Block executed with chosen die index " << index << "\n";
                        resetSelection();
                    }
                }
                break;
        }
    }

    void InputHandler::applyPendingBlockChoice(int chosenIndex) {
        if (!pendingBlock || !engine) return;
        pendingBlock->applyDiceChoice(chosenIndex);
        engine->addCommand(std::unique_ptr<engine::Command>(pendingBlock.release()));
        engine->executeCommand();
        std::cout << "Block executed with chosen die index " << chosenIndex << "\n";
        resetSelection();
    }

    InputMode InputHandler::getCurrentMode() const {
        return currentMode;
    }

    std::shared_ptr<state::Character> InputHandler::getSelectedCharacter() const {
        return selectedCharacter;
    }

    void InputHandler::handleEvent(const sf::Event& event, sf::RenderWindow* window) {
        if (event.type == sf::Event::MouseButtonPressed) {
            this->handleMouseClick(event.mouseButton, window);
        } else if (event.type == sf::Event::MouseMoved) {
            if (window) {
                sf::Vector2i mousePos(event.mouseMove.x, event.mouseMove.y);
                updatePreviewFromMouse(mousePos);
            }
        } else if (event.type == sf::Event::KeyPressed) {
            this->handleKeyPress(event.key);
        }
    }

    bool InputHandler::isMoveLegal(const std::pair<int, int> &dest) const {
        if (!selectedCharacter || !game) return false;

        // Movement: allow positions where max(|dx|,|dy|) <= movement (Chebyshev / square range)
        auto start = selectedCharacter->getPosition();
        int dx = std::abs(dest.first - start.first);
        int dy = std::abs(dest.second - start.second);
        int maxDelta = std::max(dx, dy);
        if (maxDelta > selectedCharacter->getMovement()) return false;

        // Check board bounds
        if (dest.first < 0 || dest.first >= utility::Constants::BOARD_WIDTH) return false;
        if (dest.second < 0 || dest.second >= utility::Constants::BOARD_HEIGHT) return false;

        // Destination must not be occupied by any character
        auto occ = getCharacterAt(dest);
        if (occ) return false;

        // Further rules (tackle zones, terrain, stamina, etc.) can be added here
        return true;
    }

    bool InputHandler::isBlockLegal (const std::shared_ptr<state::Character>& target) const{
        if (!selectedCharacter || !target || !game) return false;

        if (target == selectedCharacter) return false;

        // Target must not belong to current team
        auto* currentTeam = game->getCurrentTeam();
        if (!currentTeam) return false;
        for (const auto& c : currentTeam->getCharacters()) {
            if (c == target) return false; // target is a teammate -> not legal
        }

        // Must be adjacent (8-neighbour)
        auto a = selectedCharacter->getPosition();
        auto b = target->getPosition();
        int dx = std::abs(a.first - b.first);
        int dy = std::abs(a.second - b.second);
        if (dx <= 1 && dy <= 1) return true;

        return false;
    }

} // namespace client
