#include "Piece.h"

namespace state {
    bool Piece::moveTo(Square* square) {
        currentSquare = square;
        return true;
    }

    PieceStatus Piece::getStatus(){
        return status;
    }

    Square* Piece::getSquare(){
        return currentSquare;
    }


}