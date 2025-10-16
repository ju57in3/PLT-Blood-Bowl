#include "Kickoff.h"

#include "Team.h"
#include "BloodBowlGame.h"
#include "PlayerTurn.h"


namespace state {
    Kickoff::Kickoff (BloodBowlGame* game) : AbstractState(game){}

    void Kickoff::update()
    {
        int side;
        if (game->getCurrentTeam().getTeamId() == game->getTeamA().getTeamId()) {
            side = game->getWidth()/2;
        }else {
            side = 0;
        }

        int targetX = rand() % (game->getWidth() / 2) + (side);
        int targetY = rand() % game->getHeight();
        std::pair<int,int> targetSquare;
        targetSquare.first = targetX;
        targetSquare.second = targetY;

        kickBall(targetSquare);
        game->setCurrentState(game->getStateList().at(PLAYERTURN));
    }


    void Kickoff::kickBall(std::pair<int,int> targetSquare)
    {
        int direction=rand()%8;
        int rebounds=rand()%6;
        std::pair<int,int> newTarget;
        switch (direction)
        {
        case 0: //South
            {
                newTarget.first = targetSquare.first;
                newTarget.second = targetSquare.second - rebounds;
            }
        case 1: //South-East
            {
                newTarget.first = targetSquare.first + rebounds;
                newTarget.second = targetSquare.second - rebounds;
            }
        case 2: //East
            {
                newTarget.first = targetSquare.first + rebounds;
                newTarget.second = targetSquare.second;
            }
        case 3:  //North-East
            {
                newTarget.first = targetSquare.first + rebounds;
                newTarget.second = targetSquare.second + rebounds;
            }
        case 4: //North
            {
                newTarget.first = targetSquare.first;
                newTarget.second = targetSquare.second + rebounds;
            }
        case 5: //North-West
            {
                newTarget.first = targetSquare.first - rebounds;
                newTarget.second = targetSquare.second + rebounds;
            }
        case 6: //West
            {
                newTarget.first = targetSquare.first - rebounds;
                newTarget.second = targetSquare.second;
            }
        case 7: //South-West
            {
                newTarget.first = targetSquare.first - rebounds;
                newTarget.second = targetSquare.second - rebounds;
            }
        default: ;
        }
        game->setBallPosition(newTarget);
    }

}
