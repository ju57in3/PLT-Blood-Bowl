#include <iostream>

// The following lines are here to check that SFML is installed and working
#include <SFML/Graphics.hpp>

void testSFML() {
    sf::Texture texture;
}
// end of test SFML

#include <client.h>

using namespace std;
using namespace client;

int main(int argc,char* argv[])
{
    HelloWorld hello;
    hello.setCOMBIEN(3);
    hello.setQUOI("Hello World !");

    for (int i =0; i < hello.getCOMBIEN(); i++) {
        cout << hello.getQUOI() << endl;
    }

    return 0;
}
