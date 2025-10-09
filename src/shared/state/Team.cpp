#include "Team.h"
#include "Piece.h"

namespace state {
    std::vector<Piece> Team::getPieces() {
        return pieces;
    }

    int Team::getScore(){
        return score;
    }

    std::vector<Piece> Team::getPlayablePiece()
    {
        std::vector<Piece> pieces;
        for (int i = 0; i < pieces.size(); i++)
        {
            if (pieces[i].getStatus() == playable)
            {
                pieces.push_back(pieces[i]);
            }
        }
        return pieces;
    }
}