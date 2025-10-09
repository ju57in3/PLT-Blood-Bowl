#include "Ball.h"

namespace state {
    Square* Ball::getPosition (){
      return this->currentSquare;
    }

    void Ball::setPosition(Square* square){
      this->currentSquare = square;
    }

}