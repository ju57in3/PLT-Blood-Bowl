#include "Setup.h"


#include "Square.h"
#include "Kickoff.h"
#include "BloodBowlGame.h"


namespace state {
    Setup::Setup(BloodBowlGame *game) : AbstractState(game) {
    }

    int Setup::nbPieceOnBoard(Team team) {
        int count = 0;
        for (Piece piece : team.getPieces()) {
            if (piece.getSquare() != nullptr) {
                count++;
            }
        }
        return count;
    }

    int Setup::nbPieceOnLine(Team team) {
        std::vector<Square> linesSquares;
        // Example coordinates for the 7 squares
        std::vector<std::pair<int, int>> fixedCoords = {
            {13, 4}, {13, 5}, {13, 6}, {13, 7}, {13, 8}, {13, 9}, {13, 10}
        };

        for (const auto& coord : fixedCoords) {
            linesSquares.push_back(Square(coord.first, coord.second));
        }
        int count = 0;
        for (Piece piece : team.getPieces()) {
            if (piece.getSquare() != nullptr && piece.getSquare()->isIn(linesSquares)) {
                count++;
            }
        }
        return count;
    }


    int Setup::nbPieceOnLeft(Team team) {
        int count = 0;
        for (Piece piece : team.getPieces()) {
            Square* square = piece.getSquare();
            if (square != nullptr && square->getX() >= 0 && square->getX() <= 3) {
                count++;
            }
        }
        return count;
    }

    int Setup::nbPieceOnRight(Team team) {
        int count = 0;
        for (Piece piece : team.getPieces()) {
            Square* square = piece.getSquare();
            if (square != nullptr && square->getX() >= 11) {
                count++;
            }
        }
        return count;
    }

    bool Setup::isValidSetup(Team team) {
        return nbPieceOnBoard(team) <= 11 && nbPieceOnLine(team) >= 3 && nbPieceOnLeft(team) <= 2 && nbPieceOnRight(team) <= 2;
    }

    void Setup::update() {
        if (setupEnded) {
            game->setCurrentState(new Kickoff(game));
        }
    }



}
