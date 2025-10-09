#include "Square.h"

#include <vector>

namespace state {

    bool Square::isIn(std::vector<Square> list) {
        if (list.size() == 0) {
            return false;
        }
        for (Square square : list) {
            if (this->x == square.getX() && this->y == square.getY()) {
                return true;
            }
        }
        return false;
    }

}
