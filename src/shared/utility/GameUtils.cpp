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

            // Adjacent square (but not same square)
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

    void GameUtils::handleBallBounce(const std::shared_ptr<state::BloodBowlGame>& game,
                                     std::pair<int,int> startPosition,
                                     bool& outOfBounds) {
        if (!game) {
            outOfBounds = true;
            return;
        }

        std::pair<int,int> ballPos = startPosition;
        bool ballLanded = false;
        int maxBounces = 20; // Safety limit to avoid infinite loops
        int bounceCount = 0;
        outOfBounds = false;

        while (!ballLanded && bounceCount < maxBounces) {
            ballPos = scatterOnce(ballPos);
            bounceCount++;

            // Check if ball is out of bounds
            if (ballPos.first < 0 || ballPos.first >= utility::Constants::BOARD_WIDTH ||
                ballPos.second < 0 || ballPos.second >= utility::Constants::BOARD_HEIGHT) {
                outOfBounds = true;
                ballLanded = true;
                break;
            }

            // Check if there's a standing character at this position
            bool occupied = false;

            for (const auto& ch : game->getTeamA().getCharacters()) {
                if (ch && ch->getPosition() == ballPos && isCharacterStanding(ch)) {
                    occupied = true;
                    break;
                }
            }

            if (!occupied) {
                for (const auto& ch : game->getTeamB().getCharacters()) {
                    if (ch && ch->getPosition() == ballPos && isCharacterStanding(ch)) {
                        occupied = true;
                        break;
                    }
                }
            }

            // If the square is not occupied by a standing player, the ball lands
            if (!occupied) {
                ballLanded = true;
            }
        }

        game->setBallPosition(ballPos);
        game->setBallIsHold(false);
    }

} // namespace engine

