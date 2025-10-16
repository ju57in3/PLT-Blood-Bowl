#include "HelloWorld.h"

#include <iostream>
#include <string>

namespace client {
    void HelloWorld::setQUOI(std::string FEUR) {
        this->QUOI = FEUR;
    }

    void HelloWorld::setCOMBIEN(int UNPEU) {
        this->COMBIEN = UNPEU;
    }

    std::string HelloWorld::getQUOI() { return QUOI; }

    int HelloWorld::getCOMBIEN() { return COMBIEN; }

    std::ostream& operator<<(std::ostream& os, const HelloWorld& hello) {
        for (int i =0; i < hello.COMBIEN; i++) {
            os << hello.QUOI << std::endl;
        }
        return os;
    }
}
