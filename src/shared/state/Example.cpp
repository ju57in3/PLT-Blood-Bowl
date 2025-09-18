#include "Example.h"

#include <cstdlib>

namespace state {


//    void Example::setA (int a) {
//        this->a.setX(a);
//    }

    Example::Example() {

    }

    const ExampleA &Example::getA() const {
        return this->a;
    }

    void Example::setA(const ExampleA &a) {
        this->a = a;
    }

    const ExampleB &Example::getB() const {
        return *(this->b);
    }

    void Example::setB(const ExampleB &b) {
        this->b = &b;
    }
}
