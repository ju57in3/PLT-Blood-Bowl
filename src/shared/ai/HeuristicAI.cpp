#include "HeuristicAI.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <utility>
#include <vector>
#include <iostream>
#include <iomanip>

#include "engine/Engine.h"
#include "engine/Move.h"
#include "engine/Block.h"
#include "utility/GameUtils.h"
#include "utility/Constants.h"
#include "state/Team.h"
#include "state/Character.h"
#include "state/CharacterStatus.h"
#include "state/PlayerTurn.h"

namespace {
    using Pos = std::pair<int, int>;

    static int chebyshev(const Pos &a, const Pos &b) {
        return std::max(std::abs(a.first - b.first), std::abs(a.second - b.second));
    }

    static bool isPlayableForAction(const std::shared_ptr<state::Character> &c) {
        if (!c) return false;
        return c->getStatus() == state::CharacterStatus::playable;
    }

    static bool isStanding(const std::shared_ptr<state::Character> &c) {
        return utility::GameUtils::isCharacterStanding(c);
    }

    static bool isOccupied(const std::shared_ptr<state::BloodBowlGame> &game, const Pos &p) {
        return (utility::GameUtils::getCharacterAt(game, p) != nullptr);
    }

    static bool insideBoard(const std::shared_ptr<state::BloodBowlGame> &game, const Pos &p) {
        return game && game->isInsideBoard(p);
    }

    static state::Team *getTeamById(const std::shared_ptr<state::BloodBowlGame> &game, int teamId) {
        if (!game) return nullptr;
        if (game->getTeamA().getTeamId() == teamId) return &game->getTeamA();
        if (game->getTeamB().getTeamId() == teamId) return &game->getTeamB();
        return nullptr;
    }

    static state::Team *getOpponentTeam(const std::shared_ptr<state::BloodBowlGame> &game, int teamId) {
        if (!game) return nullptr;
        if (game->getTeamA().getTeamId() == teamId) return &game->getTeamB();
        if (game->getTeamB().getTeamId() == teamId) return &game->getTeamA();
        return nullptr;
    }

    static int opponentTouchdownX(const std::shared_ptr<state::BloodBowlGame> &game, int myTeamId) {
        const int w = utility::Constants::BOARD_WIDTH;
        const bool iAmTeamA = (game && game->getTeamA().getTeamId() == myTeamId);
        return iAmTeamA ? (w - 1) : 0;
    }

    static std::shared_ptr<state::Character> findBallCarrier(state::Team *team) {
        if (!team) return nullptr;
        for (auto &c: team->getCharacters()) {
            if (c && c->getHasBall()) return c;
        }
        return nullptr;
    }

    static std::shared_ptr<state::Character> closestPlayableTo(state::Team *team, const Pos &target) {
        if (!team) return nullptr;
        std::shared_ptr<state::Character> best = nullptr;
        int bestD = std::numeric_limits<int>::max();

        for (auto &c: team->getCharacters()) {
            if (!isPlayableForAction(c) || !isStanding(c)) continue;
            int d = chebyshev(c->getPosition(), target);
            if (d < bestD) {
                bestD = d;
                best = c;
            }
        }
        return best;
    }

    static std::shared_ptr<state::Character> closestStandingOpponent(state::Team *opp, const Pos &from) {
        if (!opp) return nullptr;
        std::shared_ptr<state::Character> best = nullptr;
        int bestD = std::numeric_limits<int>::max();

        for (auto &c: opp->getCharacters()) {
            if (!c) continue;
            if (!isStanding(c)) continue;
            auto st = c->getStatus();
            if (st != state::CharacterStatus::playable && st != state::CharacterStatus::played) continue;

            int d = chebyshev(c->getPosition(), from);
            if (d < bestD) {
                bestD = d;
                best = c;
            }
        }
        return best;
    }

    static Pos bestMoveToward(const std::shared_ptr<state::BloodBowlGame> &game,
                              const std::shared_ptr<state::Character> &who,
                              const Pos &target) {
        if (!game || !who) return target;

        const int mv = who->getMovement();
        const Pos start = who->getPosition();

        Pos best = start;
        int bestD = chebyshev(start, target);

        for (int dx = -mv; dx <= mv; ++dx) {
            for (int dy = -mv; dy <= mv; ++dy) {
                Pos p{start.first + dx, start.second + dy};
                if (!insideBoard(game, p)) continue;
                if (p != start && isOccupied(game, p)) continue;

                int d = chebyshev(p, target);
                if (d < bestD) {
                    bestD = d;
                    best = p;
                }
            }
        }
        return best;
    }
} // namespace

namespace ai {
    HeuristicAI::HeuristicAI(engine::Engine &engine,
                             const std::shared_ptr<state::BloodBowlGame> &game,
                             int teamId)
        : ai::AI(engine, game, teamId) {
    }

    HeuristicAI::~HeuristicAI() = default;

    bool HeuristicAI::runAI() {
        auto &eng = this->engine;
        auto gs = this->game;
        int myId = this->teamId;

        if (!gs) {
            std::cout << "[HEURISTIC AI] : Stop ! No game state.\n";
            return false;
        }

        std::cout << "\n--- [HEURISTIC AI] : My turn (team " << myId << ") ---\n";

        if (!gs->getCurrentTeam() || gs->getCurrentTeam()->getTeamId() != myId) {
            std::cout << "[HEURISTIC AI] : Stop ! Not my turn.\n";
            return false;
        }

        auto *pt = dynamic_cast<state::PlayerTurn *>(gs->getCurrentState());
        if (!pt) {
            std::cout << "[HEURISTIC AI] : Stop ! Not in PlayerTurn state.\n";
            return false;
        }
        if (pt->getTurnOver()) {
            std::cout << "[HEURISTIC AI] : Stop ! My turn already over.\n";
            return false;
        }
        if (pt->getTouchDown()) {
            std::cout << "[HEURISTIC AI] : Stop ! Touchdown already scored.\n";
            return false;
        }

        state::Team *myTeam = getTeamById(gs, myId);
        state::Team *oppTeam = getOpponentTeam(gs, myId);
        if (!myTeam || !oppTeam) {
            std::cout << "[HEURISTIC AI] : Stop ! Can't resolve teams.\n";
            return false;
        }

        auto isOpponent = [&](const std::shared_ptr<state::Character> &c) -> bool {
            if (!c) return false;
            for (auto &oc: oppTeam->getCharacters()) {
                if (oc.get() == c.get()) return true;
            }
            return false;
        };

        const int MAX_ACTIONS_PER_TURN = 11;
        int actionsDone = 0;
        bool didAnything = false;

        // Petit header contexte
        {
            auto ball = gs->getBallPosition();
            std::cout << "[HEURISTIC AI] : Ball at (" << ball.first << "," << ball.second << ")\n";
        }

        while (actionsDone < MAX_ACTIONS_PER_TURN) {
            if (!gs->getCurrentTeam() || gs->getCurrentTeam()->getTeamId() != myId) {
                std::cout << "[HEURISTIC AI] : Stop ! Not my turn anymore.\n";
                break;
            }
            if (pt->getTurnOver()) {
                std::cout << "[HEURISTIC AI] : Stop ! Turnover.\n";
                break;
            }
            if (pt->getTouchDown()) {
                std::cout << "[HEURISTIC AI] : Stop ! Touchdown scored.\n";
                break;
            }

            const Pos ballPos = gs->getBallPosition();
            auto myCarrier = findBallCarrier(myTeam);
            auto oppCarrier = findBallCarrier(oppTeam);

            bool anyPlayable = false;
            for (auto &c: myTeam->getCharacters()) {
                if (isPlayableForAction(c) && isStanding(c)) {
                    anyPlayable = true;
                    break;
                }
            }
            if (!anyPlayable) {
                std::cout << "[HEURISTIC AI] : No playable players left.\n";
                break;
            }

            bool didActionThisLoop = false;
            if (oppCarrier) {
                for (auto &me: myTeam->getCharacters()) {
                    if (!isPlayableForAction(me) || !isStanding(me)) continue;
                    if (chebyshev(me->getPosition(), oppCarrier->getPosition()) == 1) {
                        std::cout << "[HEURISTIC AI] : "
                                << me->getName() << " blocks carrier " << oppCarrier->getName()
                                << " from (" << me->getPosition().first << "," << me->getPosition().second << ")\n";

                        eng.addCommand(std::make_unique<engine::Block>(me, oppCarrier));
                        eng.executeCommand();

                        didAnything = true;
                        actionsDone++;
                        didActionThisLoop = true;
                        break;
                    }
                }
                if (didActionThisLoop) continue;
            }

            if (myCarrier && isPlayableForAction(myCarrier) && isStanding(myCarrier)) {
                int tdX = opponentTouchdownX(gs, myId);
                Pos tdTarget{tdX, myCarrier->getPosition().second};
                Pos moveTo = bestMoveToward(gs, myCarrier, tdTarget);

                std::cout << "[HEURISTIC AI] : AI carry ball to touchdown (x=" << tdX << ") and move "
                        << myCarrier->getName()
                        << " from (" << myCarrier->getPosition().first << "," << myCarrier->getPosition().second << ")"
                        << " to (" << moveTo.first << "," << moveTo.second << ")\n";

                eng.addCommand(std::make_unique<engine::Move>(myCarrier, moveTo));
                eng.executeCommand();

                didAnything = true;
                actionsDone++;
                continue;
            }

            Pos chaseTarget = ballPos;
            if (oppCarrier) chaseTarget = oppCarrier->getPosition();

            auto chaser = closestPlayableTo(myTeam, chaseTarget);

            if (chaser) {
                // Option : block opportuniste si adjacent à un adversaire (mais jamais soi-même)
                auto nearestOpp = closestStandingOpponent(oppTeam, chaser->getPosition());
                if (nearestOpp && isOpponent(nearestOpp) &&
                    chebyshev(chaser->getPosition(), nearestOpp->getPosition()) == 1) {
                    std::cout << "[HEURISTIC AI] : AI "
                            << chaser->getName() << " blocks " << nearestOpp->getName()
                            << " from (" << chaser->getPosition().first << "," << chaser->getPosition().second << ")\n";

                    eng.addCommand(std::make_unique<engine::Block>(chaser, nearestOpp));
                    eng.executeCommand();

                    didAnything = true;
                    actionsDone++;
                    continue;
                }

                Pos moveTo = bestMoveToward(gs, chaser, chaseTarget);

                std::cout << "[HEURISTIC AI] : AI move "
                        << chaser->getName()
                        << " from (" << chaser->getPosition().first << "," << chaser->getPosition().second << ")"
                        << " to (" << moveTo.first << "," << moveTo.second << ")"
                        << " toward (" << chaseTarget.first << "," << chaseTarget.second << ")\n";

                eng.addCommand(std::make_unique<engine::Move>(chaser, moveTo));
                eng.executeCommand();

                didAnything = true;
                actionsDone++;
                continue;
            }

            std::cout << "[HEURISTIC AI] : No action found for this loop.\n";
            break;
        }

        std::cout << "[HEURISTIC AI] : End turn. \n";
        pt->setTurnOver(true);

        return didAnything;
    }
} // namespace ai
