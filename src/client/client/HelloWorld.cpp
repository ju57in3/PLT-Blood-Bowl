#include "HelloWorld.h"

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
}
