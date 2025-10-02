#include "HelloWorld.h"

#include <string>

namespace client {

    void setQUOI(std::string FEUR) : QUOI(FEUR) {}

    void setCOMBIEN(int UNPEU) : COMBIEN(UNPEU) {}

    std::string getQUOI() { return QUOI; }

    int getCOMBIEN() { return COMBIEN; }
}
