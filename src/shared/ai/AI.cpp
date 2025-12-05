//
// Created by justine on 04/12/2025.
//
#include "AI.h"

#include <set>
#include <random>

#include "state/BloodBowlGame.h"
#include "engine/Engine.h"
#include "utility/GameUtils.h"

namespace ai {
    // Fixed field size (26 x 15)
    constexpr int BOARD_WIDTH = 26;
    constexpr int BOARD_HEIGHT = 15;

    // Coordinates of the midline
    constexpr int MID_Y1 = 6;
    constexpr int MID_Y2 = 7;
    constexpr int MID_Y3 = 8;

    //For the left team (teamId == 1), we place the 3 players at x = 12
    constexpr int MID_X_LEFT = 12;
    //For the right team (teamId == 2), we place the 3 players at x = 13
    constexpr int MID_X_RIGHT = 13;

    AI::AI(engine::Engine& engine, const std::shared_ptr<state::BloodBowlGame>& game, int teamId) : engine(engine), game(game), teamId(teamId) {

    }

    AI::~AI() = default;


    void AI::placePlayers() {
        if (!game) {
            return;
        }
        // Reclaim the controlled team
        state::Team& team =
            (teamId == 1) ? game->getTeamA() : game->getTeamB();

        // Reclaim the team's characters
        auto& characters = team.getCharacters();

        if (characters.empty()) {
            return;
        }

        // We never place more than 11 players
        const std::size_t maxPlayers = std::min<std::size_t>(11, characters.size());

        // All the cells already occupied
        std::set<std::pair<int, int>> occupied;

        // Determine the center line column for this team
        const int midX = (teamId == 1) ? MID_X_LEFT : MID_X_RIGHT;

        // Place up to 3 players on the 3 squares of the center line
        std::vector<std::pair<int, int>> losPositions = {
            {midX, MID_Y1},
            {midX, MID_Y2},
            {midX, MID_Y3}
        };

        std::size_t playerIndex = 0;

        for (std::size_t i = 0; i < losPositions.size() && playerIndex < maxPlayers; ++i) {
            auto& character = characters[playerIndex];
            if (!character) {
                ++playerIndex;
                continue;
            }

            const auto& pos = losPositions[i];
            character->setPosition(pos);
            occupied.insert(pos);
            ++playerIndex;
        }

        // Define the half of the field allowed for random placement
        int minX, maxX;
        if (teamId == 1) {
            // left team : left half
            minX = 0;
            maxX = BOARD_WIDTH / 2 - 1;
        } else {
            // right team : right half
            minX = BOARD_WIDTH / 2;
            maxX = BOARD_WIDTH - 1;
        }

        // RNG for random placement
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> distX(minX, maxX);
        std::uniform_int_distribution<int> distY(0, BOARD_HEIGHT - 1);

        // Place the remaining players randomly in the allowed half
        while (playerIndex < maxPlayers) {
            auto& character = characters[playerIndex];
            if (!character) {
                ++playerIndex;
                continue;
            }

            std::pair<int, int> pos;
            do {
                pos.first = distX(rng);
                pos.second = distY(rng);
            } while (occupied.count(pos) > 0);

            occupied.insert(pos);
            character->setPosition(pos);
            ++playerIndex;
        }
    }

    bool AI::runAI() {
        // Placeholder implementation
        return true;
    }
}