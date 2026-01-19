// File: src/shared/state/Kickoff.cpp
#include <random>
#include <limits>
#include <cmath>
#include <algorithm>

#include "Team.h"
#include "BloodBowlGame.h"
#include "utility/Constants.h"
#include "utility/GameUtils.h"
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

    void Kickoff::kickBall(std::pair<int,int> targetSquare) {
        std::uniform_int_distribution<int> d8(1,8);
        std::uniform_int_distribution<int> d6(1,6);

        int direction = d8(rng)-1;
        int rebounds = d6(rng);
        std::cout   << "Nombre des premiers rebonds : " << rebounds << std::endl
                    << "Dans la direction : " << direction << std::endl;

        std::pair<int,int> newTarget;

        switch (direction) {
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

        if (not isValidKickoffTarget( newTarget, *game->getCurrentTeam() ) ) {
            int receiverId = -1;
            if (game->getCurrentTeam()) receiverId = game->getCurrentTeam()->getTeamId();
            std::cout << "Ball out of bounds -> receiving team must choose a carrier (team " << receiverId << ")\n";

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
        } else {
            game->setBallPosition(newTarget);
            std::cout << "Kickoff valide : balle placée en ("
                      << newTarget.first << "," << newTarget.second << ")\n";

            auto targetPlayer = utility::GameUtils::getCharacterAt(game, game->getBallPosition());
            if (!targetPlayer) {
                direction = d8(rng)-1;
                std::pair<int, int> ballPosition = game->getBallPosition();
                switch (direction) {
                    case 0: //South
                        ballPosition.first = ballPosition.first;
                        ballPosition.second = ballPosition.second - 1;
                        break;

                    case 1: //South-East
                        ballPosition.first = ballPosition.first + 1;
                        ballPosition.second = ballPosition.second - 1;
                        break;

                    case 2: //East
                        ballPosition.first = ballPosition.first + 1;
                        ballPosition.second = ballPosition.second;
                        break;

                    case 3:  //North-East
                        ballPosition.first = ballPosition.first + 1;
                        ballPosition.second = ballPosition.second + 1;
                        break;

                    case 4: //North
                        ballPosition.first = ballPosition.first;
                        ballPosition.second = ballPosition.second + 1;
                        break;

                    case 5: //North-West
                        ballPosition.first = ballPosition.first - 1;
                        ballPosition.second = ballPosition.second + 1;
                        break;

                    case 6: //West
                        ballPosition.first = ballPosition.first - 1;
                        ballPosition.second = ballPosition.second;
                        break;

                    case 7: //South-West
                        ballPosition.first = ballPosition.first - 1;
                        ballPosition.second = ballPosition.second - 1;
                        break;

                    default:
                        break;
                }
                if (not isValidKickoffTarget( ballPosition, *game->getCurrentTeam() ) ) {
                    int receiverId = -1;
                    if (game->getCurrentTeam()) receiverId = game->getCurrentTeam()->getTeamId();
                    std::cout << "Ball out of bounds -> receiving team must choose a carrier (team " << receiverId << ")\n";

                    Team* recvTeam = game->getCurrentTeam();

                    double bestDist = std::numeric_limits<double>::infinity();
                    std::shared_ptr<Character> chosenPlayer = nullptr;

                    for (const auto& c : recvTeam->getCharacters()) {
                        if (!c) continue;
                        auto pos = c->getPosition();
                        double dx = pos.first - ballPosition.first;
                        double dy = pos.second - ballPosition.second;
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
                } else {
                    game->setBallPosition(ballPosition);
                }
            }
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
        if (!game->isInsideBoard(target))
            return false;

        bool isTeamA = (game->getTeamA().getTeamId() == kickingTeam.getTeamId());

        int minX = isTeamA ? 0 : utility::Constants::BOARD_WIDTH / 2;
        int maxX = isTeamA ? utility::Constants::BOARD_WIDTH / 2 - 1 : utility::Constants::BOARD_WIDTH - 1;

        if (target.first < minX) {
            return false;
        }

        if (target.first > maxX) {
            return false;
        }

        return true;
    }

}
