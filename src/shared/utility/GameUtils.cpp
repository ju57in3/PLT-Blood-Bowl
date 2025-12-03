#include "GameUtils.h"
#include "state/Character.h"
#include "state/Team.h"
#include "state/BloodBowlGame.h"
#include "state/CharacterStatus.h"
#include "utility/Constants.h"
#include <cmath>
#include <algorithm>

namespace utility {

    std::mt19937& GameUtils::getRNG() {
        static std::mt19937 rng(std::random_device{}());
        return rng;
    }

    std::pair<int,int> GameUtils::scatterOnce(std::pair<int,int> from) {
        std::uniform_int_distribution<int> d8(1, 8);
        int dir = d8(getRNG());
        int dx = 0, dy = 0;

        switch(dir) {
            case 1: dy = 1; break;         // N
            case 2: dx = 1; dy = 1; break; // NE
            case 3: dx = 1; break;         // E
            case 4: dx = 1; dy = -1; break;// SE
            case 5: dy = -1; break;        // S
            case 6: dx = -1; dy = -1; break;// SW
            case 7: dx = -1; break;        // W
            case 8: dx = -1; dy = 1; break;// NW
            default: break;
        }

        int nx = from.first + dx;
        int ny = from.second + dy;

        return {nx, ny};
    }

    int GameUtils::agilityTarget(int agility) {
        if (agility <= 1) return 6;
        if (agility == 2) return 5;
        if (agility == 3) return 4;
        if (agility == 4) return 3;
        if (agility >= 5) return 2;
        return 6; // fallback
    }

    bool GameUtils::agilityTest(int agility, int modifiers) {
        std::uniform_int_distribution<int> d6(1, 6);
        int roll = d6(getRNG());

        // Natural 1 is always a failure
        if (roll == 1) return false;

        // Natural 6 is always a success
        if (roll == 6) return true;

        int total = roll + modifiers;
        int need = agilityTarget(agility);
        return total >= need;
    }

    int GameUtils::countTackleZones(const state::Character& character, const state::Team& opponentTeam) {
        auto pos = character.getPosition();
        int count = 0;

        for (const auto& opponent : opponentTeam.getCharacters()) {
            if (!opponent) continue;

            // Only standing players exert tackle zones
            if (opponent->getStatus() != state::CharacterStatus::playable &&
                opponent->getStatus() != state::CharacterStatus::played) {
                continue;
            }

            auto oPos = opponent->getPosition();
            int dx = std::abs(oPos.first - pos.first);
            int dy = std::abs(oPos.second - pos.second);

            if (dx <= 1 && dy <= 1 && (dx + dy) > 0) {
                count++;
            }
        }

        return count;
    }

    bool GameUtils::isCharacterStanding(const std::shared_ptr<state::Character>& character) {
        if (!character) return false;

        auto status = character->getStatus();
        return status != state::CharacterStatus::knockedDown &&
               status != state::CharacterStatus::stunned &&
               status != state::CharacterStatus::ko &&
               status != state::CharacterStatus::injured &&
               status != state::CharacterStatus::bench;
    }

    std::shared_ptr<state::Character> GameUtils::getCharacterAt(const std::shared_ptr<state::BloodBowlGame>& game,
                                                                  std::pair<int,int> position) {
        if (!game) return nullptr;

        // Check Team A
        for (const auto& ch : game->getTeamA().getCharacters()) {
            if (ch && ch->getPosition() == position) {
                return ch;
            }
        }

        // Check Team B
        for (const auto& ch : game->getTeamB().getCharacters()) {
            if (ch && ch->getPosition() == position) {
                return ch;
            }
        }

        return nullptr;
    }

    void GameUtils::handleBallBounce(const std::shared_ptr<state::BloodBowlGame>& game,
                                     std::pair<int,int> startPosition,
                                     bool& outOfBounds,
                                     bool& turnover) {
        if (!game) {
            outOfBounds = true;
            turnover = false;
            return;
        }

        std::pair<int,int> ballPos = startPosition;
        bool ballLanded = false;
        int maxBounces = 50;
        int bounceCount = 0;
        outOfBounds = false;
        turnover = false;

        while (!ballLanded && bounceCount < maxBounces) {
            ballPos = scatterOnce(ballPos);
            bounceCount++;

            // Check if ball is out of bounds
            if (ballPos.first < 0 || ballPos.first >= utility::Constants::BOARD_WIDTH ||
                ballPos.second < 0 || ballPos.second >= utility::Constants::BOARD_HEIGHT) {
                outOfBounds = true;
                ballLanded = true;
                turnover = true;
                break;
            }

            auto characterAtPos = getCharacterAt(game, ballPos);

            if (characterAtPos) {
                if (characterAtPos->getStatus() == state::CharacterStatus::knockedDown ||
                    characterAtPos->getStatus() == state::CharacterStatus::stunned) {
                    // Ball continues to bounce
                    continue;
                }

                if (characterAtPos->getStatus() == state::CharacterStatus::playable ||
                    characterAtPos->getStatus() == state::CharacterStatus::played) {

                    state::Team* opponentTeam = nullptr;
                    bool isTeamA = false;

                    for (const auto& ch : game->getTeamA().getCharacters()) {
                        if (ch.get() == characterAtPos.get()) {
                            isTeamA = true;
                            opponentTeam = &game->getTeamB();
                            break;
                        }
                    }
                    if (!isTeamA) {
                        opponentTeam = &game->getTeamA();
                    }

                    int tackleZones = countTackleZones(*characterAtPos, *opponentTeam);

                    int catchModifiers = -tackleZones;

                    bool catchSuccess = agilityTest(characterAtPos->getAgility(), catchModifiers);

                    if (catchSuccess) {
                        characterAtPos->setHasBall(true);
                        game->setBallPosition(ballPos);
                        game->setBallIsHold(true);
                        ballLanded = true;
                        turnover = false;
                    } else {
                        ballPos = scatterOnce(ballPos);

                        if (ballPos.first < 0 || ballPos.first >= utility::Constants::BOARD_WIDTH ||
                            ballPos.second < 0 || ballPos.second >= utility::Constants::BOARD_HEIGHT) {
                            outOfBounds = true;
                            turnover = true;
                        } else {
                            outOfBounds = false;
                            turnover = true;
                        }

                        game->setBallPosition(ballPos);
                        game->setBallIsHold(false);
                        ballLanded = true;
                    }
                } else {
                    ballLanded = true;
                }
            } else {
                ballLanded = true;
                turnover = false;
            }
        }

        if (!ballLanded) {
            game->setBallPosition(ballPos);
            game->setBallIsHold(false);
        }
    }

} // namespace utility
