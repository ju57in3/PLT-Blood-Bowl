#include "InputHandler.h"

#include "utility/Constants.h"
#include "engine/Engine.h"
#include "engine/Move.h"
#include "engine/Pass.h"
#include "engine/Block.h"
#include "state/BloodBowlGame.h"
#include "state/Character.h"
#include "state/PlayerTurn.h"
#include "state/Setup.h"
#include <iostream>
#include <utility>
#include <algorithm>
#include <unistd.h>
#include <vector>
#include <bits/fs_fwd.h>

#include "state/Kickoff.h"

namespace client {

    bool isKnockdown(const std::shared_ptr<state::Character> character) {
        if (character->getStatus() == state::CharacterStatus::knockedDown) {
            return true;
        }
        return false;
    }

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

    bool InputHandler::isCharacterPlayable(const std::shared_ptr<state::Character>& character) const {
        if (!character || !game) return false;
        if (!belongsToCurrentTeam(character, game)) return false;
        if (!game->getCurrentState()) return false;
        if (!dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) return false;
        return character->getStatus() == state::CharacterStatus::playable;
    }


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

    void InputHandler::resetSelection() {
        selectedCharacter = nullptr;
        currentMode = InputMode::Idle;
        currentMovePath.clear();
        std::cout << "Selection reset. Mode: IDLE\n";
    }

    void InputHandler::handleMouseClick(const sf::Event::MouseButtonEvent& mouseButton, sf::RenderWindow* window, const std::vector<sf::FloatRect>& diceBounds) {
        if (!window || !game || !engine) {
            return;
        }

        if (auto* setup = dynamic_cast<state::Setup*>(game->getCurrentState())) {
            sf::Vector2i mousePos(mouseButton.x, mouseButton.y);
            auto boardPos = screenToBoard(mousePos);
            if (mouseButton.button == sf::Mouse::Left) {
                if (utility::GameUtils::getCharacterAt(game, boardPos)) {
                    std::cout << "Tile occupied, cannot place character here\n";
                    return;
                }
                auto* team = game->getCurrentTeam();
                for (auto& cptr : team->getCharacters()) {
                    if (cptr && cptr->getStatus() == state::CharacterStatus::bench) {
                        cptr->setPosition(boardPos);
                        cptr->setStatus(state::CharacterStatus::playable);
                        std::cout << "Placed " << cptr->getName() << " at (" << boardPos.first << ", " << boardPos.second << ")\n";
                        return;
                    }
                }
                std::cout << "No bench character available to place for team " << team->getTeamId() << "\n";
                return;
            } if (mouseButton.button == sf::Mouse::Right) {
                auto ch = utility::GameUtils::getCharacterAt(game, boardPos);
                if (ch && ch->getStatus() != state::CharacterStatus::bench) {
                    if (game->getCurrentTeam() && belongsToCurrentTeam(ch, game)) {
                        ch->setPosition(std::make_pair(-1, -1));
                        ch->setStatus(state::CharacterStatus::bench);
                        std::cout << "Sent " << ch->getName() << " to bench\n";
                    } else {
                        std::cout << "Cannot remove opponent's character during setup\n";
                    }
                }
                return;
            }
        }

        if (auto* kickoff = dynamic_cast<state::Kickoff*>(game->getCurrentState())) {
            sf::Vector2i mousePos(mouseButton.x, mouseButton.y);
            auto boardPos = screenToBoard(mousePos);

            /*
            if (!kickoff->isValidKickoffTarget(boardPos, *game->getCurrentTeam())) {
                std::cout << "Invalid kickoff target at (" << boardPos.first << ", " << boardPos.second << ")\n";
                return;
            }
            */

            if (mouseButton.button == sf::Mouse::Left) {
                kickoff->setTarget(boardPos);
                std::cout << "Kickoff target set to (" << boardPos.first << ", " << boardPos.second << ")\n";
                kickoff->setTargetSelected(true);
                return;
            }

            if (auto targetPlayer = utility::GameUtils::getCharacterAt(game, game->getBallPosition())) {

            }
            else {

            }
        }

        if (mouseButton.button == sf::Mouse::Left)
        {
            if (pendingBlock)
            {
                if (!diceChosen && !diceBounds.empty()) {
                    auto mx = static_cast<float>(mouseButton.x);
                    auto my = static_cast<float>(mouseButton.y);
                    for (size_t i = 0; i < diceBounds.size(); ++i) {
                        if (diceBounds[i].contains(mx, my)) {
                            // apply choice (1-based index)
                            diceChosen = true;
                            applyPendingBlockChoice(static_cast<int>(i + 1));
                            return;
                        }
                    }
                    return;
                }

                if (diceChosen && pendingPush)
                {
                    sf::Vector2i mousePos(mouseButton.x, mouseButton.y);
                    std::pair<int,int> targetPos = screenToBoard(mousePos);

                    std::vector<std::pair<int,int>> positionOptions = pendingBlock->getPushedPositionOptions();
                    if (utility::GameUtils::getCharacterAt(game, targetPos))
                    {
                        std::cout << "You cannot pushed the enemy character in this squarre because it is occupied! \n";
                        return;
                    }

                    if (std::find(positionOptions.begin(), positionOptions.end(), targetPos) != positionOptions.end())
                    {
                        applyPendingPushChoice(targetPos);
                        diceChosen = false;
                        pendingPush = false;
                        return;
                    }
                    else
                    {
                        std::cout << "You cannot pushed the enemy character on this square because it is too far! \n";
                        return;
                    }
                }
            }


            sf::Vector2i mousePos(mouseButton.x, mouseButton.y);
            auto boardPos = screenToBoard(mousePos);

            auto character = utility::GameUtils::getCharacterAt(game, boardPos);
            if (character) {
                if (isCharacterPlayable(character) or isKnockdown(character)) {
                    selectedCharacter = character;
                    currentMode = InputMode::Selected_Character;
                }
                else {
                    std::cout << "Cannot select this character (not playable)\n";
                }
            } else {
                std::cout << "No character at this position\n";
            }

        } else if (mouseButton.button == sf::Mouse::Right) {
            sf::Vector2i mousePos(mouseButton.x, mouseButton.y);
            auto boardPos = screenToBoard(mousePos);

            auto targetCharacter = utility::GameUtils::getCharacterAt(game, boardPos);

            if (!selectedCharacter) {
                std::cout << "No character selected. Left click to select a character first.\n";
                return;
            }

            if (!targetCharacter) {
                // empty tile -> move
                if (!dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) {
                    std::cout << "Cannot move: not in PlayerTurn state\n";
                    return;
                }
                if (!selectedCharacter) {
                    std::cout << "No character selected for move path\n";
                    return;
                }

                if (currentMovePath.empty()) {
                    currentMovePath.push_back(selectedCharacter->getPosition());
                }

                auto last = currentMovePath.back();
                if (boardPos == last && currentMovePath.size() > 1) {
                    for (std::pair<int,int> pos : currentMovePath) {
                        auto moveStepCmd = std::make_unique<engine::Move>(selectedCharacter, pos);
                        engine->addCommand(std::move(moveStepCmd));
                        engine->executeCommand();

                        // Check if a turnover occurred after this step
                        if (auto* pt = dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) {
                            if (pt->getTurnOver()) {
                                std::cout << "Turnover occurred during movement - stopping at current position\n";
                                currentMovePath.clear();
                                resetSelection();
                                return;
                            }
                        }
                        sleep(0.5);
                    }

                    currentMovePath.clear();
                    resetSelection();
                    return;
                }

                int dx = std::abs(boardPos.first - last.first);
                int dy = std::abs(boardPos.second - last.second);
                if (isKnockdown(selectedCharacter)) {
                    std::cout << "This caracter is knockdown, click on him to get him up (cost 3 movements)\n";
                    return;
                }

                if (std::max(dx, dy) != 1) {
                    std::cout << "Invalid step: must be adjacent to previous step\n";
                    return;
                }

                if (utility::GameUtils::getCharacterAt(game, boardPos)) {
                    std::cout << "Cannot step onto occupied tile\n";
                    return;
                }

                int steps = static_cast<int>(currentMovePath.size());

                int range = selectedCharacter->getMovement();
                if (selectedCharacter->gotUp) {
                    range = range - 3;
                }

                if (steps >= range + 1) {
                    std::cout << "Movement limit reached\n";
                    return;
                }

                currentMovePath.push_back(boardPos);
                return;
            }

            if (targetCharacter == selectedCharacter) {
                if (isKnockdown(selectedCharacter)) {
                    selectedCharacter->setStatus(state::CharacterStatus::playable);
                    selectedCharacter->gotUp = true;
                    std::cout << "Got up !\n";
                    return;
                } else if (selectedCharacter->gotUp){
                    selectedCharacter->setStatus(state::CharacterStatus::played);
                    resetSelection();
                    return;
                }
                else {
                    std::cout << "Right-clicked the selected character; no action\n";
                    return;
                }
            }

            if (belongsToCurrentTeam(targetCharacter, game)) {
                // teammate -> pass
                if (!dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) {
                    std::cout << "Cannot pass: not in PlayerTurn state\n";
                    return;
                }
                std::cout << "Creating Pass command to " << targetCharacter->getName() << "\n";
                auto passCmd = std::make_unique<engine::Pass>(selectedCharacter, targetCharacter);
                engine->addCommand(std::move(passCmd));
                engine->executeCommand();
                std::cout << "Pass command executed\n";
                resetSelection();
                return;
            }
            //Test debug problème blocage
            int x_selected = targetCharacter->getPosition().first;
            int y_selected = targetCharacter->getPosition().second;
            std::cout << "On a sélectionné le joueur adverse situé à: (" << x_selected << ", " << y_selected << ")\n";
            bool test = isBlockLegal(targetCharacter);   // Le Problème vient d'ici
            std::cout << "Test Block: " << test;
            //
            if (isBlockLegal(targetCharacter))
            {
                pendingBlock = std::make_unique<engine::Block>(selectedCharacter, targetCharacter);
                auto options = pendingBlock->getDiceOptions();
                std::cout << "Block initiated against " << targetCharacter->getName() << ".\n";
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
        previewCharacter = utility::GameUtils::getCharacterAt(game, boardPos);
        previewIsLegal = false;

        if (!selectedCharacter) {
            previewIsLegal = false;
            return;
        }

        if (!previewCharacter) {
            previewIsLegal = isMoveLegal(boardPos);
            return;
        }

        if (belongsToCurrentTeam(previewCharacter, game)) {
            previewIsLegal = (previewCharacter != selectedCharacter);
            return;
        }

        previewIsLegal = isBlockLegal(previewCharacter);
    }

    void InputHandler::handleKeyPress(const sf::Event::KeyEvent& key) {
        switch (key.code) {
            case sf::Keyboard::Escape:
                pendingBlock.reset();
                resetSelection();
                break;
            case sf::Keyboard::Return:
                if (!game) break;
                if (auto* setup = dynamic_cast<state::Setup*>(game->getCurrentState())) {
                    auto* team = game->getCurrentTeam();
                    if (team && setup->isValidSetup(*team)) {
                        setup->endSetup();
                        std::cout << "Setup ended for team " << team->getTeamId() << "; proceeding to Kickoff if both teams done.\n";
                    } else {
                        std::cout << "Setup invalid for current team; cannot end setup yet.\n";
                    }
                    break;
                }

                if (auto* pt = dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) {
                    pt->setEndTurn(true);
                    pt->update();
                    pendingBlock.reset();
                    resetSelection();
                    std::cout << "End of turn " << game->getTurnCounter() << "\n";
                } else {
                    std::cout << "Return key pressed: no action in current state (" << (game->getCurrentState()? game->getCurrentState()->getName() : "none") << ")\n";
                }
                break;

        case sf::Keyboard::Y:
            if (pendingFollow)
            {
                pendingFollow =false;
                auto followPath = std::make_unique<engine::Move>(selectedCharacter, pendingBlock->getHoldDefenderPosition());
                engine->addCommand(std::move(followPath));
                engine->executeCommand();
                engine->addCommand(std::move(pendingBlock));
                engine->executeCommand();
                resetSelection();
            }
            break;

        case sf::Keyboard::N:
            if (pendingFollow)
            {
                pendingFollow = false;

                engine->addCommand(std::move(pendingBlock));
                engine->executeCommand();
                resetSelection();
            }
            break;

        default:
            break;
        }
    }

    void InputHandler::applyPendingBlockChoice(int chosenIndex) {
        if (!pendingBlock || !engine) return;
        pendingBlock->applyDiceChoice(chosenIndex);
        pendingPush = pendingBlock->getEnemyPushed();
        if (!pendingPush)
        {
            engine->addCommand(std::move(pendingBlock));
            engine->executeCommand();
            std::cout << "Block executed with chosen dice index " << chosenIndex << "\n";
            resetSelection();
        }
        else
        {
            diceChosen = true;
            std::cout << "Pushed Position Choice required \n";
        }
    }

    void InputHandler::applyPendingPushChoice(std::pair<int,int> targetPos)
    {
        if (!pendingPush || !engine) return;
        pendingBlock->applyPushedPositionChoice(targetPos);
        std::cout << "Push executed \n";
        pendingFollow = true;
        std::cout << "Following Choice Required: tap Y to Follow or N to not Follow.\n";
    }

    InputMode InputHandler::getCurrentMode() const {
        return currentMode;
    }

    std::shared_ptr<state::Character> InputHandler::getSelectedCharacter() const {
        return selectedCharacter;
    }

    std::vector<std::pair<int,int>> InputHandler::getMovePath() const {
        return currentMovePath;
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

        auto start = selectedCharacter->getPosition();
        int dx = std::abs(dest.first - start.first);
        int dy = std::abs(dest.second - start.second);
        int maxDelta = std::max(dx, dy);
        if (maxDelta > selectedCharacter->getMovement()) return false;

        // Check board bounds
        if (dest.first < 0 || dest.first >= utility::Constants::BOARD_WIDTH) return false;
        if (dest.second < 0 || dest.second >= utility::Constants::BOARD_HEIGHT) return false;

        auto occ = utility::GameUtils::getCharacterAt(game, dest);
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

        if (target->getStatus() != state::CharacterStatus::playable && target->getStatus() != state::CharacterStatus::played) // Problème avec le status des joueurs. Mattéo? Relevé des joueurs?
        {
            return false;
        }

        auto a = selectedCharacter->getPosition();
        auto b = target->getPosition();
        int dx = std::abs(a.first - b.first);
        int dy = std::abs(a.second - b.second);
        if (dx <= 1 && dy <= 1)
        {
            return true;
        }

        return false;
    }

} // namespace client
