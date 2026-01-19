//
// Created by justine on 12/12/2025.
//
#include "HeuristicAI.h"

#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <engine/Block.h>
#include <engine/Move.h>
#include <engine/Pass.h>
#include <engine/EndTurn.h>
#include <utility/GameUtils.h>
#include <utility/Constants.h>
#include <state/PlayerTurn.h>

namespace ai {
    HeuristicAI::HeuristicAI(engine::Engine& engine, const std::shared_ptr<state::BloodBowlGame>& game, int teamId) : AI(engine, game, teamId){

    }

    HeuristicAI::~HeuristicAI() = default;

    bool HeuristicAI::runAI() {
        state::Team *myTeam = nullptr;
        state::Team *opponentTeam = nullptr;

        if (game->getTeamA().getTeamId() == teamId) {
            myTeam = &game->getTeamA();
            opponentTeam = &game->getTeamB();
        } else {
            myTeam = &game->getTeamB();
            opponentTeam = &game->getTeamA();
        }

        if (!myTeam || !opponentTeam) return false;

        auto* ptStart = dynamic_cast<state::PlayerTurn*>(game->getCurrentState());
        if (!ptStart) {
            std::cout << "[HEURISTIC AI] : Not in PlayerTurn state, skipping AI actions (current: "
                      << (game->getCurrentState() ? game->getCurrentState()->getName() : "None")
                      << ")\n";
            return false;
        }

        const int W = utility::Constants::BOARD_WIDTH;
        const int H = utility::Constants::BOARD_HEIGHT;

        auto inside = [&](std::pair<int, int> p) {
            return p.first >= 0 && p.first < W && p.second >= 0 && p.second < H;
        };

        auto chebyshevDist = [&](std::pair<int, int> a, std::pair<int, int> b) {
            return std::max(std::abs(a.first - b.first), std::abs(a.second - b.second));
        };

        auto findBallCarrier = [&](state::Team &t) -> std::shared_ptr<state::Character> {
            for (const auto &ch: t.getCharacters()) {
                if (ch && ch->getHasBall()) return ch;
            }
            return nullptr;
        };

        auto isAhead = [&](int targetX, int passerX, int receiverX) {
            // "devant" = plus proche de la ligne d'en-but adverse en x
            if (targetX > passerX) return receiverX > passerX; // on attaque vers la droite
            return receiverX < passerX; // on attaque vers la gauche
        };

        auto isPassPossibleRange = [&](int distCheb) {
            // On limite volontairement à Quick/Short pour rester "heuristique simple"
            return distCheb <= 6;
        };

        auto findSimpleReceiverAhead = [&](const std::shared_ptr<state::Character> &passer,
                                           int targetX) -> std::shared_ptr<state::Character> {
            if (!passer) return nullptr;
            auto pPos = passer->getPosition();

            // Choix simple: le 1er coéquipier jouable "devant" à portée quick/short
            for (const auto &r: myTeam->getCharacters()) {
                if (!r) continue;
                if (r.get() == passer.get()) continue;
                if (r->getStatus() != state::CharacterStatus::playable) continue;

                auto rPos = r->getPosition();
                int d = chebyshevDist(pPos, rPos);
                if (!isPassPossibleRange(d)) continue;
                if (!isAhead(targetX, pPos.first, rPos.first)) continue;

                return r;
            }
            return nullptr;
        };

        // endzone adverse
        const bool iAmTeamA = (myTeam->getTeamId() == game->getTeamA().getTeamId());
        const int targetX = iAmTeamA ? (W - 1) : 0;

        std::cout << "\n[HEURISTIC AI] : Team " << teamId << " is playing.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        auto ballPos = game->getBallPosition();
        std::cout << "[HEURISTIC AI] : Ball position: (" << ballPos.first << ", " << ballPos.second << ")\n";

        if (!inside(ballPos)) {
            std::cerr << "[HEURISTIC AI] : WARNING ! Invalid ball position! Ball may not be initialized.\n";
        }

        auto myCarrier = findBallCarrier(*myTeam);
        auto oppCarrier = findBallCarrier(*opponentTeam);

        // Parcourir tous les personnages jouables et leur assigner une action
        for (const auto &c: myTeam->getCharacters()) {
            if (!c || c->getStatus() != state::CharacterStatus::playable) continue;

            // PRIORITE 1: si c'est notre porteur -> marquer (courir), ou passer si bloqué
            if (myCarrier && c.get() == myCarrier.get()) {
                auto currentPos = c->getPosition();

                // Si très proche de la ligne, on fonce (même logique de pas à pas que ton code)
                // Sinon on fonce aussi, mais on garde ta manière de construire targetPos.
                int maxMove = c->getMovement();
                std::pair<int, int> targetPos = currentPos;

                // Pression: tackle zones sur le porteur
                int tz = utility::GameUtils::countTackleZones(*c, *opponentTeam);

                // Si englué, on tente une passe simple quick/short vers un coéquipier "devant"
                if (tz >= 2) {
                    auto receiver = findSimpleReceiverAhead(c, targetX);
                    if (receiver) {
                        std::cout << "[HEURISTIC AI] :" << c->getName()
                                << " is under pressure (" << tz << " TZ), tries pass to "
                                << receiver->getName() << "\n";
                        engine.addCommand(std::make_unique<engine::Pass>(c, receiver));
                        std::this_thread::sleep_for(std::chrono::milliseconds(200));
                        continue;
                    }
                }

                // Sinon: courir vers endzone (déplacement pas à pas comme ton move-to-ball)
                for (int step = 0; step < maxMove; ++step) {
                    if (targetPos.first == targetX) break;

                    int nextX = targetPos.first;
                    int nextY = targetPos.second;

                    // avancer en x vers la ligne
                    if (targetX > targetPos.first) nextX += 1;
                    else if (targetX < targetPos.first) nextX -= 1;

                    if (nextX < 0 || nextX >= W || nextY < 0 || nextY >= H) break;
                    targetPos = {nextX, nextY};
                }

                std::cout << "[HEURISTIC AI] : Ball carrier " << c->getName()
                        << " runs toward endzone x=" << targetX
                        << " to (" << targetPos.first << "," << targetPos.second << ")\n";

                engine.addCommand(std::make_unique<engine::Move>(c, targetPos));
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                continue;
            }

            // PRIORITE 2: Bloquer un adversaire adjacent (comme ton code)
            // Petit tweak: si l'adversaire a le ballon et est adjacent, on le cible en priorité
            auto blockables = utility::GameUtils::blockableCharacters(c, *opponentTeam);
            if (!blockables.empty()) {
                std::shared_ptr<state::Character> target = blockables.front();

                if (oppCarrier) {
                    for (auto &b: blockables) {
                        if (b && b.get() == oppCarrier.get()) {
                            target = b;
                            break;
                        }
                    }
                }

                std::cout << "[HEURISTIC AI] : " << c->getName() << " blocking " << target->getName() << "\n";
                engine.addCommand(std::make_unique<engine::Block>(c, target));
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                continue;
            }

            // PRIORITE 3: déplacement "objectif"
            // - si adversaire porteur: aller vers lui
            // - sinon: aller vers la balle (comme avant)
            std::pair<int, int> objective = ballPos;
            if (oppCarrier) objective = oppCarrier->getPosition();

            int maxMove = c->getMovement();
            std::pair<int, int> currentPos = c->getPosition();
            std::pair<int, int> targetPos = currentPos;

            int dx = objective.first - currentPos.first;
            int dy = objective.second - currentPos.second;
            int dist = std::abs(dx) + std::abs(dy);

            if (dist > 2 && inside(objective)) {
                std::cout << "[HEURISTIC AI] : " << c->getName() << " at (" << currentPos.first << ","
                        << currentPos.second << ") moving towards "
                        << (oppCarrier ? "opponent carrier" : "ball")
                        << " at (" << objective.first << "," << objective.second << ")\n";

                for (int step = 0; step < maxMove; ++step) {
                    dx = objective.first - targetPos.first;
                    dy = objective.second - targetPos.second;

                    if (dx == 0 && dy == 0) break;

                    int nextX = targetPos.first;
                    int nextY = targetPos.second;

                    if (std::abs(dx) >= std::abs(dy) && dx != 0) {
                        nextX += (dx > 0) ? 1 : -1;
                    } else if (dy != 0) {
                        nextY += (dy > 0) ? 1 : -1;
                    }

                    if (nextX < 0 || nextX >= W || nextY < 0 || nextY >= H) break;
                    targetPos = {nextX, nextY};
                }

                std::cout << "[HEURISTIC AI] : Final target position: (" << targetPos.first << ","
                        << targetPos.second << ")\n";

                engine.addCommand(std::make_unique<engine::Move>(c, targetPos));
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                continue;
            }

            // PRIORITE 4: mouvement aléatoire (comme ton code), mais seulement si objectif invalide/proche
            std::uniform_int_distribution<int> dDist(1, maxMove);
            int moveDistance = dDist(utility::GameUtils::getRNG());

            for (int step = 0; step < moveDistance; ++step) {
                std::pair<int, int> nextPos = utility::GameUtils::scatterOnce(targetPos);

                if (nextPos.first < 0 || nextPos.first >= W ||
                    nextPos.second < 0 || nextPos.second >= H) {
                    break;
                }
                targetPos = nextPos;
            }

            std::cout << "[HEURISTIC AI] " << c->getName() << " random move\n";
            engine.addCommand(std::make_unique<engine::Move>(c, targetPos));
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        if (dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) {
            std::cout << "[HEURISTIC AI] Turn complete, adding EndTurn command\n";
            engine.addCommand(std::make_unique<engine::EndTurn>());
        } else {
            std::cout << "[HEURISTIC AI] Turn complete, but not in PlayerTurn anymore (current: "
                      << (game->getCurrentState() ? game->getCurrentState()->getName() : "None")
                      << "), skipping EndTurn.\n";
        }

        return false;
    }
}
