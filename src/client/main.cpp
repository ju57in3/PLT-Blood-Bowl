#include <iostream>

// Les lignes suivantes ne servent qu'à vérifier que la compilation avec SFML fonctionne
#include <SFML/Graphics.hpp>

void testSFML() {
    sf::Texture texture;
}

// Fin test SFML

#include <state.h>

using namespace std;
using namespace state;

int main(int argc,char* argv[])
{
    Example example;
    ExampleA exampleA;
    exampleA.setX(53);
    example.setA(exampleA);

    cout << "It works !" << endl;

    return 0;
}
