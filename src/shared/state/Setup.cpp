#include "Setup.h"


#include "Square.h"
#include "Kickoff.h"
#include "BloodBowlGame.h"


namespace state {
    Setup::Setup(BloodBowlGame *game) : AbstractState(game) {
    }

    int Setup::nbCharacterOnBoard(Team team) {
        int count = 0;
        for (Character piece : team.getCharacters()) {
            if (piece.getPosition() != -1) {
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
            if (piece.getPosition() != -1 && piece.getPosition()) {
                count++;
            }
        }
        return count;
    }


    int Setup::nbCharacterOnLeft(Team team) {
        int count = 0;
        for (Character piece : team.getCharacters()) {
            Square* square = piece.getSquare();
            if (square != nullptr && square->getX() >= 0 && square->getX() <= 3) {
                count++;
            }
        }
        return count;
    }

    int Setup::nbCharacterOnRight(Team team) {
        int count = 0;
        for (Character piece : team.getCharacters()) {
            Square* square = piece.getSquare();
            if (square != nullptr && square->getX() >= 11) {
                count++;
            }
        }
        return count;
    }

    bool Setup::isValidSetup(Team team) {
        return nbCharacterOnBoard(team) <= 11 && nbCharacterOnLine(team) >= 3 && nbCharacterOnLeft(team) <= 2 && nbCharacterOnRight(team) <= 2;
    }

    void Setup::update() {
        if (setupEnded) {
            game->setCurrentState(new state::Kickoff(game));
        }
    }



}
