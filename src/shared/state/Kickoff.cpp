// File: src/shared/state/Kickoff.cpp
#include <random>
#include <limits>
#include <cmath>
#include <algorithm>

#include "Team.h"
#include "BloodBowlGame.h"
#include "utility/Constants.h"
#include "Kickoff.h"

#include <iostream>
#include <ostream>

static std::mt19937 rng(std::random_device{}());

namespace state {
    Kickoff::Kickoff (BloodBowlGame* game) : AbstractState(game), target({-1,-1}), targetSelected(false){}

    void Kickoff::update()
    {
        if (targetSelected) {
            kickBall(target);
            game->setCurrentState(game->getStateList().at(PLAYERTURN).get());
            targetSelected = false;
        }
    }


    std::string Kickoff::getName() const {
        return "Kickoff";
    }

    void Kickoff::kickBall(std::pair<int,int> targetSquare)
    {
        std::uniform_int_distribution<int> d8(1,8);
        std::uniform_int_distribution<int> d6(1,6);

        int direction = d8(rng);
        int rebounds = d6(rng);
        std::cout << rebounds << std::endl;

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

        // Clamp to board bounds
        if (newTarget.first < 0) newTarget.first = 0;
        if (newTarget.first >= utility::Constants::BOARD_WIDTH) newTarget.first = utility::Constants::BOARD_WIDTH - 1;
        if (newTarget.second < 0) newTarget.second = 0;
        if (newTarget.second >= utility::Constants::BOARD_HEIGHT) newTarget.second = utility::Constants::BOARD_HEIGHT - 1;

        int receiverId = -1;
        if (game->getCurrentTeam()) receiverId = game->getCurrentTeam()->getTeamId();

        int midX = utility::Constants::BOARD_WIDTH / 2;
        bool leftReceivingHalf = false;
        if (receiverId == 1) {
            leftReceivingHalf = (newTarget.first >= midX);
        } else {
            leftReceivingHalf = (newTarget.first < midX);
        }

        game->setBallPosition(newTarget);

        if (leftReceivingHalf) {
            std::cout << "Ball left receiving team's half -> receiving team must choose a carrier (team " << receiverId << ")\n";

            Team* recvTeam = game->getCurrentTeam();

            double bestDist = std::numeric_limits<double>::infinity();
            std::shared_ptr<Character> chosenPlayer = nullptr;

            for (const auto& c : recvTeam->getCharacters()) {
                if (!c) continue;
                auto pos = c->getPosition();
                double dx = pos.first - newTarget.first;
                double dy = pos.second - newTarget.second;
                double dist = std::sqrt(dx*dx + dy*dy);
                if (dist < bestDist) {
                    bestDist = dist;
                    chosenPlayer = c;
                }
            }
            chosenPlayer->setHasBall(true);
            auto carrierPos = chosenPlayer->getPosition();
            game->setBallPosition(carrierPos);
            std::cout << "Receiving team chose player at (" << carrierPos.first << "," << carrierPos.second << ") to carry the ball.\n";

        }
    }

    Kickoff::~Kickoff() = default;

    void Kickoff::setTarget(std::pair<int, int> target) {
        this->target = target;
    }

    void Kickoff::setTargetSelected(bool newValue) {
        targetSelected = newValue;
    }

    bool Kickoff::isValidKickoffTarget(std::pair<int, int> target, const Team& kickingTeam) const {
        int minX = (kickingTeam.getTeamId() == 1) ? 0 : utility::Constants::BOARD_WIDTH / 2;
        int maxX = (kickingTeam.getTeamId() == 1) ? (utility::Constants::BOARD_WIDTH / 2 - 1) : (utility::Constants::BOARD_WIDTH - 1);

        if (target.first < minX || target.first > maxX) {
            return false;
        }

        if (target.second < 0 || target.second >= utility::Constants::BOARD_HEIGHT) {
            return false;
        }

        return true;
    }

}
