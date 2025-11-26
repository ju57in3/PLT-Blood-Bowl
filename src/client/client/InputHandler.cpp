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
#include <algorithm>
#include <vector>

namespace client {

    bool belongsToCurrentTeam(const std::shared_ptr<state::Character>& ch, const std::shared_ptr<state::BloodBowlGame>& game) {
        if (!ch || !game) return false;
        auto* currentTeam = game->getCurrentTeam();
        if (!currentTeam) return false;
        for (const auto& c : currentTeam->getCharacters()) {
            if (c == ch) return true;
        }
        return false;
    }

    InputHandler::InputHandler(std::shared_ptr<state::BloodBowlGame> game, engine::Engine* engine)
        : currentMode(Idle), selectedCharacter(nullptr), game(std::move(game)), engine(engine), pendingBlock(nullptr), previewBoardPos({0,0}), previewCharacter(nullptr) {
    }

    InputHandler::~InputHandler() = default;


    std::pair<int, int> InputHandler::screenToBoard(const sf::Vector2i& screenPos) const {

        const int stride = utility::Constants::BOARD_TILE_PIXEL_SIZE + utility::Constants::BOARD_TILE_SPACING;
        int boardX = (screenPos.x - utility::Constants::BOARD_OFFSET_X) / stride;
        int boardY = (screenPos.y - utility::Constants::BOARD_OFFSET_Y) / stride;

        if (boardX < 0) boardX = 0;
        if (boardX >= utility::Constants::BOARD_WIDTH) boardX = utility::Constants::BOARD_WIDTH - 1;
        if (boardY < 0) boardY = 0;
        if (boardY >= utility::Constants::BOARD_HEIGHT) boardY = utility::Constants::BOARD_HEIGHT - 1;

        return {boardX, boardY};
    }

    std::shared_ptr<state::Character> InputHandler::getCharacterAt(const std::pair<int, int>& boardPos) const {
        if (!game) return nullptr;

        auto* currentTeam = game->getCurrentTeam();
        if (!currentTeam) {
            return nullptr;
        }

        for (const auto& character : currentTeam->getCharacters()) {
            if (character->getPosition() == boardPos) {
                return character;
            }
        }

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

    void InputHandler::handleMouseClick(const sf::Event::MouseButtonEvent& mouseButton, sf::RenderWindow* window, const std::vector<sf::FloatRect>& diceBounds) {
        if (!window || !game || !engine) {
            return;
        }

        if (mouseButton.button == sf::Mouse::Left) {
            // If a pending block exists, a left-click may be a choice on the dice overlay
            if (pendingBlock && !diceBounds.empty()) {
                float mx = static_cast<float>(mouseButton.x);
                float my = static_cast<float>(mouseButton.y);
                for (size_t i = 0; i < diceBounds.size(); ++i) {
                    if (diceBounds[i].contains(mx, my)) {
                        // apply choice (1-based index)
                        applyPendingBlockChoice(static_cast<int>(i + 1));
                        return;
                    }
                }
                // click not on dice overlay -> ignore while pendingBlock is active
                return;
            }

            sf::Vector2i mousePos(mouseButton.x, mouseButton.y);
            auto boardPos = screenToBoard(mousePos);

            std::cout << "Click at screen (" << mouseButton.x << ", " << mouseButton.y
                      << ") -> board (" << boardPos.first << ", " << boardPos.second << ")\n";

            // Selection logic (context sensitive: left click selects characters)
            auto character = getCharacterAt(boardPos);
            if (character) {
                if (belongsToCurrentTeam(character, game)) {
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

        } else if (mouseButton.button == sf::Mouse::Right) {
            // Right click: context-sensitive action (move / pass / block)
            sf::Vector2i mousePos(mouseButton.x, mouseButton.y);
            auto boardPos = screenToBoard(mousePos);

            auto targetCharacter = getCharacterAt(boardPos);

            if (!selectedCharacter) {
                std::cout << "No character selected. Left click to select a character first.\n";
                return;
            }

            if (!targetCharacter) {
                // empty tile -> move
                std::cout << "Creating Move command to (" << boardPos.first << ", " << boardPos.second << ")\n";
                auto moveCmd = std::make_unique<engine::Move>(selectedCharacter, boardPos);
                engine->addCommand(std::move(moveCmd));
                engine->executeCommand();
                std::cout << "Move command executed\n";
                resetSelection();
                return;
            }

            if (targetCharacter == selectedCharacter) {
                std::cout << "Right-clicked the selected character; no action\n";
                return;
            }

            if (belongsToCurrentTeam(targetCharacter, game)) {
                // teammate -> pass
                std::cout << "Creating Pass command to " << targetCharacter->getName() << "\n";
                auto passCmd = std::make_unique<engine::Pass>(selectedCharacter, targetCharacter);
                engine->addCommand(std::move(passCmd));
                engine->executeCommand();
                std::cout << "Pass command executed\n";
                resetSelection();
                return;
            }

            // enemy -> block
            pendingBlock = std::make_unique<engine::Block>(selectedCharacter, targetCharacter);
            auto options = pendingBlock->getDiceOptions();
            std::cout << "Block initiated against " << targetCharacter->getName() << ". ";
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

        if (!selectedCharacter) {
            previewIsLegal = false;
            return;
        }

        // If there is no character under cursor, show move legality
        if (!previewCharacter) {
            previewIsLegal = isMoveLegal(boardPos);
            return;
        }

        // If it's a teammate (not self) -> pass is possible
        if (belongsToCurrentTeam(previewCharacter, game)) {
            previewIsLegal = (previewCharacter != selectedCharacter);
            return;
        }

        // Otherwise it's an opponent -> show block legality
        previewIsLegal = isBlockLegal(previewCharacter);
    }

    void InputHandler::handleKeyPress(const sf::Event::KeyEvent& key) {
        switch (key.code) {
            case sf::Keyboard::Escape:
                pendingBlock.reset();
                resetSelection();
                break;

            default:
                // Numeric key selection for pending block removed: dice must be chosen by clicking
                // on the dice overlay (main loop) or via applyPendingBlockChoice().
                break;
        }
    }

    void InputHandler::applyPendingBlockChoice(int chosenIndex) {
        if (!pendingBlock || !engine) return;
        pendingBlock->applyDiceChoice(chosenIndex);
        engine->addCommand(std::move(pendingBlock));
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

    void InputHandler::handleEvent(const sf::Event& event, sf::RenderWindow* window, const std::vector<sf::FloatRect>& diceBounds) {
        if (event.type == sf::Event::MouseButtonPressed) {
            this->handleMouseClick(event.mouseButton, window, diceBounds);
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

        auto start = selectedCharacter->getPosition();
        int dx = std::abs(dest.first - start.first);
        int dy = std::abs(dest.second - start.second);
        int maxDelta = std::max(dx, dy);
        if (maxDelta > selectedCharacter->getMovement()) return false;

        // Check board bounds
        if (dest.first < 0 || dest.first >= utility::Constants::BOARD_WIDTH) return false;
        if (dest.second < 0 || dest.second >= utility::Constants::BOARD_HEIGHT) return false;

        auto occ = getCharacterAt(dest);
        if (occ) return false;

        return true;
    }

    bool InputHandler::isBlockLegal (const std::shared_ptr<state::Character>& target) const{
        if (!selectedCharacter || !target || !game) return false;

        if (target == selectedCharacter) return false;

        auto* currentTeam = game->getCurrentTeam();
        if (!currentTeam) return false;
        for (const auto& c : currentTeam->getCharacters()) {
            if (c == target) return false;
        }

        auto a = selectedCharacter->getPosition();
        auto b = target->getPosition();
        int dx = std::abs(a.first - b.first);
        int dy = std::abs(a.second - b.second);
        if (dx <= 1 && dy <= 1) return true;

        return false;
    }

} // namespace client