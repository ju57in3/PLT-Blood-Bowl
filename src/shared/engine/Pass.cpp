#include "Pass.h"

#include <algorithm>

#include "state/Team.h"
#include <cmath>
#include <functional>
#include <iostream>
#include <utility>
#include <state/PlayerTurn.h>

using namespace utility;

namespace engine {


    Pass::Pass(std::shared_ptr<state::Character> passer, std::shared_ptr<state::Character> receiver)
        : passer(std::move(passer)), receiver(std::move(receiver)) {
    }

    Pass::~Pass() = default;

    CommandTypeId Pass::getCommandTypeId() {
        return PassId;
    }

    std::vector<std::shared_ptr<state::Character>> Pass::checkInterceptions(const std::shared_ptr<state::BloodBowlGame>& game) {
        auto from = passer->getPosition();
        auto to   = receiver->getPosition();

        int x0 = from.first;
        int y0 = from.second;
        int x1 = to.first;
        int y1 = to.second;

        int dx = std::abs(x1 - x0);
        int dy = std::abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;

        int x = x0;
        int y = y0;

        std::vector<std::pair<int,int>> linePositions;
        while (x != x1 || y != y1) {
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x += sx; }
            if (e2 < dx)  { err += dx; y += sy; }
            if (x != x1 || y != y1) { linePositions.emplace_back(x, y); }
        }

        // Déterminer équipe du passeur via adresse (plus fiable que nom)
        state::Team& teamA = game->getTeamA();
        state::Team& teamB = game->getTeamB();
        state::Team* passerTeam = nullptr;
        for (auto& c : teamA.getCharacters()) {
            if (c.get() == passer.get()) { passerTeam = &teamA; break; }
        }
        if (!passerTeam) {
            for (auto& c : teamB.getCharacters()) {
                if (c.get() == passer.get()) { passerTeam = &teamB; break; }
            }
        }
        // Opposing team
        state::Team* opposingTeamPtr = nullptr;
        if (passerTeam) {
            opposingTeamPtr = (passerTeam == &teamA) ? &teamB : &teamA;
        } else {
            // Fallback si non trouvé (ne devrait pas arriver si design correct)
            opposingTeamPtr = &teamB;
        }

        // Récupérer les personnages jouables adverses (interception seulement par jouable?)
        auto opposingPlayable = opposingTeamPtr->getPlayableCharacter();
        std::vector<std::shared_ptr<state::Character>> result;

        for (auto* character : opposingPlayable) {
            auto charPos = character->getPosition();
            for (const auto& linePos : linePositions) {
                if (charPos == linePos) { result.emplace_back(character); break; }
                int distX = std::abs(charPos.first - linePos.first);
                int distY = std::abs(charPos.second - linePos.second);
                if (distX <= 1 && distY <= 1 && (distX + distY) > 0) { result.emplace_back(character); break; }
            }
        }
        return result;
    }

    void Pass::execute(std::shared_ptr<state::BloodBowlGame>& game) {
        std::cout << "In execute of Pass command" << std::endl;

        if (!game || !passer || !receiver) {
            return;
        }
        if (!passer->getHasBall()) {
            return; // doit tenir le ballon
        }

        // Déterminer équipes
        state::Team& teamA = game->getTeamA();
        state::Team& teamB = game->getTeamB();
        state::Team* passerTeam = nullptr;

        for (auto& c : teamA.getCharacters()) {
            if (c.get() == passer.get()) {
                passerTeam = &teamA;
                break;
            }
        }
        if (!passerTeam) {
            for (auto& c : teamB.getCharacters()) {
                if (c.get() == passer.get()) {
                    passerTeam = &teamB;
                    break;
                }
            }
        }
        state::Team* opposingTeam = passerTeam == &teamA ? &teamB : &teamA;

        int passerTZ = GameUtils::countTackleZones(*passer, *opposingTeam);

        // Distance (Chebyshev)
        auto pPos = passer->getPosition();
        auto rPos = receiver->getPosition();
        int dx = std::abs(rPos.first - pPos.first);
        int dy = std::abs(rPos.second - pPos.second);
        int distance = std::max(dx, dy);

        // Modificateur de portée officiant (approximation)
        int rangeMod = 0; // Quick +1, Short 0, Long -1, Long Bomb -2
        if (distance <= 3) rangeMod = +1;          // Quick
        else if (distance <= 6) rangeMod = 0;      // Short
        else if (distance <= 9) rangeMod = -1;     // Long
        else if (distance <= 12) rangeMod = -2;    // Long Bomb
        else rangeMod = -99;                       // impossible -> fumble auto

        // Intercepteurs potentiels
        interceptors = checkInterceptions(game);
        std::shared_ptr<state::Character> chosenInterceptor = nullptr;
        if (!interceptors.empty()) {
            // Choisir le meilleur (AG la plus haute) comme approximation du choix coach
            // TODO: Choix du joueur
            chosenInterceptor = *std::max_element(interceptors.begin(), interceptors.end(), [](auto& a, auto& b){ return a->getAgility() < b->getAgility(); });
        }

        // Jet de passe
        bool automaticFumble = (rangeMod <= -50);
        bool passAccurate = false;
        if (!automaticFumble) {
            int passModifiers = rangeMod - passerTZ; // -1 par zone de tacle adverse sur passeur
            passAccurate = GameUtils::agilityTest(passer->getAgility(), passModifiers);
        }

        if (!passAccurate) {
            passer->setHasBall(false);
            game->setBallIsHold(false);
            auto dropPos = passer->getPosition();
            bool outOfBounds = false;
            bool ballTurnover = false;
            GameUtils::handleBallBounce(game, dropPos, outOfBounds, ballTurnover);
            checkAndHandleTurnover(game);
            return;
        }

        if (chosenInterceptor) {
            int interTZ = GameUtils::countTackleZones(*chosenInterceptor, *passerTeam);
            int interceptionModifiers = -2 - interTZ;
            bool interceptionSuccess = GameUtils::agilityTest(chosenInterceptor->getAgility(), interceptionModifiers);
            if (interceptionSuccess) {
                passer->setHasBall(false);
                chosenInterceptor->setHasBall(true);
                game->setBallPosition(chosenInterceptor->getPosition());
                game->setBallIsHold(true);
                checkAndHandleTurnover(game);
                return;
            }
        }

        int catcherTZ = GameUtils::countTackleZones(*receiver, *opposingTeam);
        int catchModifiers = +1 /* accurate */ - catcherTZ;
        bool catchSuccess = GameUtils::agilityTest(receiver->getAgility(), catchModifiers);

        passer->setHasBall(false);
        if (catchSuccess) {
            receiver->setHasBall(true);
            game->setBallPosition(receiver->getPosition());
            game->setBallIsHold(true);
        } else {
            auto scatterPos = receiver->getPosition();
            bool outOfBounds = false;
            bool ballTurnover = false;
            GameUtils::handleBallBounce(game, scatterPos, outOfBounds, ballTurnover);
            checkAndHandleTurnover(game);
         }

         checkAndHandleTouchdown(game);

        if (passer->getStatus() == state::CharacterStatus::playable) {
            passer->setStatus(state::CharacterStatus::played);
        }
     }
 }
