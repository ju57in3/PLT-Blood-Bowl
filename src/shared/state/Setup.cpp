#include "Setup.h"


#include "Kickoff.h"
#include "BloodBowlGame.h"

constexpr std::pair<int, int> HORS_TABLEAU = {-1, -1};

namespace state {
    Setup::Setup(BloodBowlGame *game) : AbstractState(game) {
    }

    int Setup::nbCharacterOnBoard(Team team) {
        int count = 0;

        for (Character piece : team.getCharacters()) {
            if (piece.getStatus() == playable && piece.getPosition() != HORS_TABLEAU) {
                count++;
            }
        }
        return count;
    }

    int Setup::nbCharacterOnLine(Team team) {
        std::vector<int> linesSquares;
        // Example coordinates for the 7 squares
        std::vector<std::pair<int, int>> fixedCoords = {
            {13, 4}, {13, 5}, {13, 6}, {13, 7}, {13, 8}, {13, 9}, {13, 10}
        };

        for (const auto& coord : fixedCoords) {
            linesSquares.push_back(coord.first * coord.second);
        }
        int count = 0;
        for (Character piece : team.getCharacters()) {
            std::pair<int, int> pos = piece.getPosition();
            if (pos != HORS_TABLEAU) {
                for (const auto& coord : fixedCoords) {
                    if (pos == coord) {
                        count++;
                        break;
                    }
                }
            }
        }
        return count;
    }


    int Setup::nbCharacterOnTop(Team team) {
        int count = 0;
        for (Character piece : team.getCharacters()) {
            std::pair<int,int> position = piece.getPosition();
            if (position.first != -1 && position.first >= 11) {
                count++;
            }
        }
        return count;
    }

    int Setup::nbCharacterOnBottom(Team team) {
        int count = 0;
        for (Character piece : team.getCharacters()) {
            std::pair<int,int> position = piece.getPosition();

            if (position.first != -1 && position.first <= 4) {
                count++;
            }
        }
        return count;
    }

    bool Setup::isValidSetup(Team team) {
        return nbCharacterOnBoard(team) <= 11 && nbCharacterOnLine(team) >= 3 && nbCharacterOnTop(team) <= 2 && nbCharacterOnBottom(team) <= 2;
    }

    void Setup::update() {
        if (setupEnded) {
            game->setCurrentState(new state::Kickoff(game));
        }
    }



}
