#include "Kickoff.h"

#include "Square.h"
#include "Team.h"



namespace state {
    Kickoff::Kickoff (BloodBowlGame* game) : AbstractState(game){}

    void Kickoff::update()
    {

    }
    void Kickoff::kickBall(Square* initTarget, Ball ball)         //Ball ball en argument?!
    {
        int direction=rand()%8;
        int rebounds=rand()%6;
        switch (direction)
        {
        case 0: //South
            {
                Square* sSquare = new Square(initTarget->getX(),initTarget->getY()-rebounds);
                ball.setPosition(sSquare);
            }
        case 1: //South-East
            {
                Square* seSquare = new Square(initTarget->getX()+rebounds,initTarget->getY()-rebounds);
                ball.setPosition(seSquare);
            }
        case 2: //East
            {
                Square* eSquare = new Square(initTarget->getX()+rebounds,initTarget->getY());
                ball.setPosition(eSquare);
            }
        case 3:  //North-East
            {
                Square* neSquare = new Square(initTarget->getX()+rebounds,initTarget->getY()+rebounds);
                ball.setPosition(neSquare);
            }
        case 4: //North
            {
                Square* nSquare = new Square(initTarget->getX(),initTarget->getY()+rebounds);
                ball.setPosition(nSquare);
            }
        case 5: //North-West
            {
                Square* nwSquare = new Square(initTarget->getX()-rebounds,initTarget->getY()+rebounds);
                ball.setPosition(nwSquare);
            }
        case 6: //West
            {
                Square* wSquare = new Square(initTarget->getX()-rebounds,initTarget->getY());
                ball.setPosition(wSquare);
            }
        case 7: //South-West
            {
                Square* swSquare = new Square(initTarget->getX()-rebounds,initTarget->getY()-rebounds);
                ball.setPosition(swSquare);
            }
        default: ;
        }
        ball.isHold = false;
    }

}
