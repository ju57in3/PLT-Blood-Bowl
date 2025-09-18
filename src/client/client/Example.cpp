#include <state/Example.h>  // Included from library shared_static
#include "Example.h"

namespace client {

void Example::setX (int x) {
    // Create an object from "shared" library
    state::Example y {};

    this->x = x;
}

}

