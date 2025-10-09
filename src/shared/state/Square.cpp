#include "Square.h"

namespace state {
    Square::Square(int x, int y):x(x),y(y){}

    int Square::getX()
    {
        return this->x;
    }

    int Square::getY()
    {
        return this->y;
    }

}