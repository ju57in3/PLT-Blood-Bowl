#include "Kickoff.h"

#include "Square.h"
#include "Team.h"
#include "BloodBowlGame.h"
#include "PlayerTurn.h"


namespace state {
    Kickoff::Kickoff (BloodBowlGame* game) : AbstractState(game){}

    void Kickoff::update()
    {
        int targetX = rand() % (game->getBoard().getWidth() / 2) + (game->getCurrentTeam() == game->getTeamA() ? game->getBoard().getWidth() / 2 : 0);
        int targetY = rand() % game->getBoard().getHeight();
        Square targetSquare = Square(targetX, targetY);

        kickBall(targetSquare);

        game->setCurrentState(new state::PlayerTurn(game));
    }
    void Kickoff::kickBall(Square initTarget)
    {
        int direction=rand()%8;
        int rebounds=rand()%6;
        switch (direction)
        {
        case 0: //South
            {
                Square sSquare = Square(initTarget.getX(),initTarget.getY()-rebounds);
                game->getBoard().setBallPosition(sSquare);
            }
        case 1: //South-East
            {
                Square seSquare = Square(initTarget.getX()+rebounds,initTarget.getY()-rebounds);
                game->getBoard().setBallPosition(seSquare);
            }
        case 2: //East
            {
                Square eSquare = Square(initTarget.getX()+rebounds,initTarget.getY());
                game->getBoard().setBallPosition(eSquare);
            }
        case 3:  //North-East
            {
                Square neSquare = Square(initTarget.getX()+rebounds,initTarget.getY()+rebounds);
                game->getBoard().setBallPosition(neSquare);
            }
        case 4: //North
            {
                Square nSquare = Square(initTarget.getX(),initTarget.getY()+rebounds);
                game->getBoard().setBallPosition(nSquare);
            }
        case 5: //North-West
            {
                Square nwSquare = Square(initTarget.getX()-rebounds,initTarget.getY()+rebounds);
                game->getBoard().setBallPosition(nwSquare);
            }
        case 6: //West
            {
                Square wSquare = Square(initTarget.getX()-rebounds,initTarget.getY());
                game->getBoard().setBallPosition(wSquare);
            }
        case 7: //South-West
            {
                Square swSquare = Square(initTarget.getX()-rebounds,initTarget.getY()-rebounds);
                game->getBoard().setBallPosition(swSquare);
            }
        default: ;
        }
    }

}
