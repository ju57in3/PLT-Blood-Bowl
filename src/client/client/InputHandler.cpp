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
        : currentMode(Idle), selectedCharacter(nullptr), game(std::move(game)), engine(engine) {
    }

    InputHandler::~InputHandler() = default;


    std::pair<int, int> InputHandler::screenToBoard(const sf::Vector2i& screenPos) const {
        // Convert screen coordinates to board coordinates
        int boardX = (screenPos.x - utility::Constants::BOARD_OFFSET_X) / (utility::Constants::BOARD_TILE_PIXEL_SIZE + 1);
        int boardY = (screenPos.y - utility::Constants::BOARD_OFFSET_Y) / (utility::Constants::BOARD_TILE_PIXEL_SIZE + 1);

        // Clamp to board bounds
        if (boardX < 0) boardX = 0;
        if (boardX >= utility::Constants::BOARD_WIDTH) boardX = utility::Constants::BOARD_WIDTH - 1;
        if (boardY < 0) boardY = 0;
        if (boardY >= utility::Constants::BOARD_HEIGHT) boardY = utility::Constants::BOARD_HEIGHT - 1;

        return {boardX, boardY};
    }

    std::shared_ptr<state::Character> InputHandler::getCharacterAt(const std::pair<int, int>& boardPos) {
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
                case InputMode::Idle:
                case InputMode::Selected_Character: {
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
                            std::cout << "Press M for Move, P for Pass, B for Block\n";
                        } else {
                            std::cout << "Cannot select opponent's character\n";
                        }
                    } else {
                        std::cout << "No character at this position\n";
                    }
                    break;
                }

                case InputMode::Select_Move_Target: {
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

                case InputMode::Select_Pass_Target: {
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

                case InputMode::Select_Block_Target: {
                    if (selectedCharacter) {
                        auto targetCharacter = getCharacterAt(boardPos);
                        if (targetCharacter && targetCharacter != selectedCharacter) {
                            std::cout << "Creating Block command against " << targetCharacter->getName() << "\n";
                            auto blockCmd = std::make_unique<engine::Block>(selectedCharacter, targetCharacter);
                            engine->addCommand(std::move(blockCmd));
                            engine->executeCommand();
                            std::cout << "Block command executed\n";
                        } else {
                            std::cout << "Invalid block target\n";
                        }
                    }
                    resetSelection();
                    break;
                }
            }
        } else if (mouseButton.button == sf::Mouse::Right) {
            // Right click cancels current action
            resetSelection();
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
                resetSelection();
                break;

            default:
                break;
        }
    }

    InputMode InputHandler::getCurrentMode() const {
        return currentMode;
    }

    std::shared_ptr<state::Character> InputHandler::getSelectedCharacter() const {
        return selectedCharacter;
    }

    void InputHandler::handleEvent(const sf::Event& event, sf::RenderWindow* window) {
        // New implementation: call member functions directly and forward window pointer
        if (event.type == sf::Event::MouseButtonPressed) {
            this->handleMouseClick(event.mouseButton, window);
        } else if (event.type == sf::Event::KeyPressed) {
            this->handleKeyPress(event.key);
        }
    }

}

/*


*/
