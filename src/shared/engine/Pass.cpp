#include "Pass.h"

#include <algorithm>

#include "state/Team.h"
#include <cmath>
#include <functional>
#include <utility>
#include <state/PlayerTurn.h>
#include <random>

namespace engine {

    // TODO: généralisation de countTackleZone
    static int countTackleZones(const state::Character& target, const state::Team& opponent) {
        auto pos = target.getPosition();
        int cnt = 0;
        for (auto& character : opponent.getCharacters()) {
            if (!character) continue;
            const auto& c = *character;
            if (c.getStatus() == state::playable || c.getStatus() == state::played) {
                auto oPos = c.getPosition();
                int dx = std::abs(oPos.first - pos.first);
                int dy = std::abs(oPos.second - pos.second);
                if (dx <= 1 && dy <= 1 && (dx + dy) > 0) {
                    cnt++;
                }
            }
        }
        return cnt;
    }
    // TODO : generalise agilityTarget
    static int agilityTarget(int ag) {
        if (ag <= 1) return 6;
        if (ag == 2) return 5;
        if (ag == 3) return 4;
        if (ag == 4) return 3;
        if (ag == 5) return 2;
        return 2;
    }

    // TODO: generalise agilityTest
    // TODO: Clarify lambda expression for roller
    static bool agilityTest(int agility, int modifiers, std::function<int(int)> roller) {
        int need = agilityTarget(agility);
        int roll = roller(6); // D6
        if (roll == 1) return false; // natural 1 échec auto
        if (roll == 6) return true;  // natural 6 succès auto (sauf conditions extrêmes, ignorées ici)
        int total = roll + modifiers;
        return total >= need;
    }

    // TODO : generalise scatter
    static std::pair<int,int> scatterOnce(std::pair<int,int> from, int width, int height) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<int> d8(1,8);
        int dir = d8(gen);
        int dx = 0, dy = 0;
        switch(dir) {
            case 1: dy = 1; break;        // N
            case 2: dx = 1; dy = 1; break;// NE
            case 3: dx = 1; break;        // E
            case 4: dx = 1; dy = -1; break;// SE
            case 5: dy = -1; break;       // S
            case 6: dx = -1; dy = -1; break;// SW
            case 7: dx = -1; break;       // W
            case 8: dx = -1; dy = 1; break; // NW
        }
        int nx = from.first + dx;
        int ny = from.second + dy;
        return {nx, ny};
    }

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

    void Pass::execute(const std::shared_ptr<state::BloodBowlGame>& game) {
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

        int passerTZ = countTackleZones(*passer, *opposingTeam);

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
            passAccurate = agilityTest(passer->getAgility(), passModifiers, [&](int r){ return rand() % r; });
        }

        if (!passAccurate) {
            passer->setHasBall(false);
            game->setBallIsHold(false);
            auto dropPos = passer->getPosition();
            // Premier rebond
            auto newPos = scatterOnce(dropPos, game->getWidth(), game->getHeight());
            game->setBallPosition(newPos);
            if (auto* pt = dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) {
                pt->setTurnOver(true);
            }
            return;
        }

        // Tentative d'interception avant la réception
        if (chosenInterceptor) {
            // Modificateurs interception: -2 + zones de tacle (déjà inclus via countTackleZones sur inter par adversaires? Officiellement -2 et -1 par TZ adverse sur intercepteur). On ajoute -2 et - countTackleZones par équipe du passeur.
            int interTZ = countTackleZones(*chosenInterceptor, *passerTeam);
            int interceptionModifiers = -2 - interTZ;
            bool interceptionSuccess = agilityTest(chosenInterceptor->getAgility(), interceptionModifiers, [&](int r){ return rand() % r; });
            if (interceptionSuccess) {
                passer->setHasBall(false);
                chosenInterceptor->setHasBall(true);
                game->setBallPosition(chosenInterceptor->getPosition());
                game->setBallIsHold(true);
                if (auto* pt = dynamic_cast<state::PlayerTurn*>(game->getCurrentState())){
                    pt->setTurnOver(true);
                }
                return;
            }
        }

        int catcherTZ = countTackleZones(*receiver, *opposingTeam);
        int catchModifiers = +1 /* accurate */ - catcherTZ;
        bool catchSuccess = agilityTest(receiver->getAgility(), catchModifiers, [&](int r){ return rand() % r; });

        passer->setHasBall(false);
        if (catchSuccess) {
            receiver->setHasBall(true);
            game->setBallPosition(receiver->getPosition());
            game->setBallIsHold(true);
            if (receiver->getStatus() == state::playable) receiver->setStatus(state::played);
        } else {
            auto scatterPos = receiver->getPosition();
            scatterPos = scatterOnce(scatterPos, game->getWidth(), game->getHeight());
            game->setBallPosition(scatterPos);
            game->setBallIsHold(false);
            // Turnover
            if (auto* pt = dynamic_cast<state::PlayerTurn*>(game->getCurrentState())) {
                pt->setTurnOver(true);
            }
        }
    }
}
