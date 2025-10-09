#include "Board.h"

namespace state{

    Square Board::getSquare(int x, int y){
        return this->squares[x][y];
    }

    void Board::moveBall(Square *square) {
        ball.currentSquare = square;
    }

}
