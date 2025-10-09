#include "Board.h"

namespace state{

    void Board::moveBall(Square* square) {
        ball.setPosition(square);
    }

}
