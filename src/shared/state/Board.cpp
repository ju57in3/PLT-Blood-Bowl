#include "Board.h"

namespace state{

    state::Square Board::getSquare(int x, int y){
        return this->squares[x][y];
    }
}
