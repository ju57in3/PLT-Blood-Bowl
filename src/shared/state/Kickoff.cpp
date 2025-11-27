#include "Kickoff.h"

#include "Team.h"
#include "BloodBowlGame.h"
#include "utility/Constants.h"


namespace state {
    Kickoff::Kickoff (BloodBowlGame* game) : AbstractState(game), target({-1,-1}), targetSelected(false){}

    void Kickoff::update()
    {
        if (targetSelected) {
            kickBall(target);
            game->setCurrentState(game->getStateList().at(PLAYERTURN).get());
        }
    }


    std::string Kickoff::getName() const {
        return "Kickoff";
    }

    void Kickoff::kickBall(std::pair<int,int> targetSquare)
    {
        int direction=rand()%8;
        int rebounds=rand()%6;
        std::pair<int,int> newTarget;
        switch (direction){
            case 0: //South
                newTarget.first = targetSquare.first;
                newTarget.second = targetSquare.second - rebounds;
                break;

            case 1: //South-East
                newTarget.first = targetSquare.first + rebounds;
                newTarget.second = targetSquare.second - rebounds;
                break;

            case 2: //East

                newTarget.first = targetSquare.first + rebounds;
                newTarget.second = targetSquare.second;
                break;

            case 3:  //North-East
                newTarget.first = targetSquare.first + rebounds;
                newTarget.second = targetSquare.second + rebounds;
                break;

            case 4: //North
                newTarget.first = targetSquare.first;
                newTarget.second = targetSquare.second + rebounds;
                break;

            case 5: //North-West
                newTarget.first = targetSquare.first - rebounds;
                newTarget.second = targetSquare.second + rebounds;
                break;

            case 6: //West
                newTarget.first = targetSquare.first - rebounds;
                newTarget.second = targetSquare.second;
                break;

            case 7: //South-West
                newTarget.first = targetSquare.first - rebounds;
                newTarget.second = targetSquare.second - rebounds;
                break;

            default:
                break;
        }
        game->setBallPosition(newTarget);
    }

    Kickoff::~Kickoff() {
    }

}
