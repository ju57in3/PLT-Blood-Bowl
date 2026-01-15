//
// Created by justine on 19/12/2025.
//

#include "AdvancedAI.h"

#include <algorithm>
#include <iostream>
#include <cmath>

#include "engine/Engine.h"
#include "engine/Move.h"
#include "engine/Block.h"
#include "engine/Pass.h"

#include "state/Team.h"
#include "state/Character.h"
#include "state/CharacterStatus.h"

#include "utility/Constants.h"
#include "utility/GameUtils.h"

namespace ai {
    AdvancedAI::AdvancedAI(engine::Engine &engine, std::shared_ptr<state::BloodBowlGame> &game, int teamId) : AI(engine, game, teamId), successfulActions(), targetMode(false){

    }

    AdvancedAI::~AdvancedAI() = default;

    state::Team &AdvancedAI::myTeam() const {
        if (game->getTeamA().getTeamId() == teamId) {
            return game->getTeamA();
        } else {
            return game->getTeamB();
        }
    }

    state::Team &AdvancedAI::oppTeam() const {
        if (game->getTeamA().getTeamId() == teamId) {
            return game->getTeamB();
        } else {
            return game->getTeamA();
        }
    }

    std::shared_ptr<state::Character> AdvancedAI::findBallCarrier(state::Team &team) const {
        for (const auto& c : team.getCharacters()) {
            if (c && c->getHasBall()) {
                return c;
            }
        }
        return nullptr;
    }

    int AdvancedAI::chebyshev(std::pair<int, int> a, std::pair<int, int> b) const {
        return std::max(std::abs(a.first - b.first), std::abs(a.second - b.second));
    }

    bool AdvancedAI::isInside(std::pair<int, int> p) const {
        return p.first >= 0 && p.first < utility::Constants::BOARD_WIDTH && p.second >= 0 && p.second < utility::Constants::BOARD_HEIGHT;
    }

    std::shared_ptr<state::Character> AdvancedAI::findClosestTo(const std::vector<std::shared_ptr<state::Character> > &chars, std::pair<int, int> pos) const {
        std::shared_ptr<state::Character> best = nullptr;
        int bestD = 1e9;
        for (const auto& c : chars) {
            if (!c) continue;
            int d = chebyshev(c->getPosition(), pos);
            if (d < bestD) {
                bestD = d;
                best = c;
            }
        }
        return best;
    }

    float AdvancedAI::agilitySuccessProb(int agility, int modifiers) const {
        auto target = utility::GameUtils::agilityTarget(agility);

        int successCount = 0;
        for (int roll = 1; roll <= 6; ++roll) {
            if (roll == 1) continue;
            if (roll == 6) {
                successCount++;
                continue;
            }
            int total = roll + modifiers;
            if (total >= target) {
                successCount++;
            }
        }
        return static_cast<float>(successCount) / 6.0f;
    }

    int AdvancedAI::tackleZonesOnTile(std::pair<int, int> tile, state::Team &opp) const {
        int count = 0;
        for (const auto& c : opp.getCharacters()) {
            if (!c) continue;

            if (c->getStatus() != state::CharacterStatus::playable && c->getStatus() != state::CharacterStatus::played) {
                continue;
            }

            auto pos = c->getPosition();
            int dx = std::abs(pos.first - tile.first);
            int dy = std::abs(pos.second - tile.second);
            if (dx <= 1 && dy <= 1 && (dx +dy) > 0) count++;
        }
        return count;
    }

    float AdvancedAI::scoreMove(const std::shared_ptr<state::Character> &ch, std::pair<int, int> dest) const {
        if (!ch) return -1e9f;
        if (!isInside(dest)) return -1e9f;

        if (utility::GameUtils::getCharacterAt(game, dest)) {
            if (dest == ch->getPosition()) return -1e9f;
        }

        state::Team& me = myTeam();
        state::Team& opp = oppTeam();

        auto ballPos = game->getBallPosition();
        bool ballHeld = game->getBallIsHold();

        float score = 0.0f;

        int tz = tackleZonesOnTile(dest, opp);
        score -= 1.5f * tz;

        if (ch->getHasBall()) {
            int goalX = (me.getTeamId() == game->getTeamA().getTeamId()) ? (utility::Constants::BOARD_WIDTH - 1) : 0;
            int distNow = std::abs(ch->getPosition().first - goalX);
            int distNew = std::abs(dest.first - goalX);

            score += 3.0f * (distNow - distNew);
            score -= 2.0f * tz;
        }

        if (!ballHeld) {
            int dNow = chebyshev(ch->getPosition(), ballPos);
            int dNew = chebyshev(dest, ballPos);

            score += 2.0f * (dNow - dNew);

            if (dest == ballPos) {
                float pPick = agilitySuccessProb(ch->getAgility(), 0);
                score += 8.0f * pPick - 6.0f * (1.0f - pPick);
            }
        }

        auto oppCarrier = findBallCarrier(opp);
        if (oppCarrier) {
            int dNow = chebyshev(ch->getPosition(), oppCarrier->getPosition());
            int dNew = chebyshev(dest, oppCarrier->getPosition());
            score += 1.2f * (dNow - dNew);
        }

        if (ch->getStatus() == state::CharacterStatus::playable) {
            score += 0.5f;
        }
        return score;
    }

    float AdvancedAI::scoreBlock(const std::shared_ptr<state::Character> &attacker, const std::shared_ptr<state::Character> &defender) const {
        if (!attacker || !defender) return -1e9f;

        if (attacker->getStatus() != state::CharacterStatus::playable) return -1e9f;

        auto a = attacker->getPosition();
        auto d = defender->getPosition();
        if (chebyshev(a, d) != 1) return -1e9f;

        int aStr = attacker->getStrength();
        int dStr = defender->getStrength();

        int dice = 1;
        if (aStr > dStr) {
            dice = (aStr >= 2 * dStr) ? 3 : 2;
        } else if (aStr < dStr) {
            dice = (2*aStr <= dStr) ? 3 : 2;
        }

        bool chooseMax = (aStr >= dStr);

        auto cdf = [&](int v)->float {
            return static_cast<float>(v) / 6.0f;
        };
        auto sf = [&](int v)->float {
            return static_cast<float>(7 - v) / 6.0f;
        };

        auto pMaxEq = [&](int v)->float {
            float p1 = std::pow(cdf(v), dice);
            float p0 = (v > 1) ? std::pow(cdf(v-1), dice) : 0.0f;
            return p1 - p0;
        };

        auto pMinEq = [&](int v)->float {
            float p1 = 1.0f - std::pow(sf(v), dice);
            float p0 = (v < 6) ? std::pow(sf(v+1), dice) : 0.0f;
            return p1 - p0;
        };

        float p[7] = {0};
        for (int v = 1; v <= 6; v++) {
            p[v] = chooseMax ? pMaxEq(v) : pMinEq(v);
        }

        float score = 0.0f;

        bool attackerHasBall = attacker->getHasBall();
        bool defenderHasBall = defender->getHasBall();

        auto val = [&](int face)->float {
            if (face == 1) return attackerHasBall ? -18.0f : -10.0f;
            if (face == 2) return attackerHasBall ? -12.0f : -6.0f;
            if (face == 3 || face == 4) return defenderHasBall ? +4.0f : +1.0f;
            if (face == 5) return defenderHasBall ? +10.0f : +5.0f;
            if (face == 6) return defenderHasBall ? +12.0f : +6.0f;
            return 0.0f;
        };

        for (int face = 1; face <= 6; face++) {
            score += p[face] * val(face);
        }

        if (defenderHasBall) score += 3.0f;

        return score;
    }

    float AdvancedAI::scorePass(const std::shared_ptr<state::Character> &passer, const std::shared_ptr<state::Character> &receiver) const {
        if (!passer || !receiver) return -1e9f;
        if (!passer->getHasBall()) return -1e9f;
        if (passer->getStatus() != state::CharacterStatus::playable) return -1e9f;
        if (receiver->getStatus() != state::CharacterStatus::playable) return -1e9f;

        state::Team& me = myTeam();
        state::Team& opp = oppTeam();

        auto pPos = passer->getPosition();
        auto rPos = receiver->getPosition();

        int dist = chebyshev(pPos, rPos);
        if (dist > 12) return -1e9f;

        int rangeMod = 0;
        if (dist <= 3) rangeMod = +1;
        else if (dist <= 6) rangeMod = 0;
        else if (dist <= 9) rangeMod = -1;
        else rangeMod = -2;

        int passerTZ = utility::GameUtils::countTackleZones(*passer, opp);
        float pPass = agilitySuccessProb(passer->getAgility(), rangeMod - passerTZ);

        int recvTZ = utility::GameUtils::countTackleZones(*receiver, opp);
        float pCatch = agilitySuccessProb(receiver->getAgility(), +1 - recvTZ);

        float interceptionPenalty = 0.0f;
        interceptionPenalty += 0.08f * tackleZonesOnTile(rPos, opp);

        float pTotal = pPass * (1.0f - interceptionPenalty) * pCatch;
        pTotal = std::clamp(pTotal, 0.0f, 1.0f);

        int goalX = (me.getTeamId() == game->getTeamA().getTeamId()) ? (utility::Constants::BOARD_WIDTH - 1) : 0;
        int prog = std::abs(pPos.first - goalX) - std::abs(rPos.first - goalX);

        float score = 0.0f;
        score += 2.5f * prog;
        score += 8.0f * pTotal - 10.0f * (1.0f - pTotal);

        return score;
    }

    bool AdvancedAI::runAI() {
        if (!game) return false;

        state::Team& me = myTeam();
        state::Team& opp = oppTeam();

        auto myCarrier = findBallCarrier(me);
        auto oppCarrier = findBallCarrier(opp);

        targetMode = (oppCarrier != nullptr);

        enum class ActType { Move, Block, Pass };
        struct Candidate {
            ActType type;
            float score;
            std::shared_ptr<state::Character> a;
            std::shared_ptr<state::Character> b;
            std::pair<int,int> dest;
        };

        Candidate best;
        best.score = -1e9f;

        {
            auto playable = me.getPlayableCharacter();
            for (auto& c : playable) {
                if (!c) continue;
                auto blockables = utility::GameUtils::blockableCharacters(c, opp);
                for (auto& def : blockables) {
                    float sc = scoreBlock(c, def);
                    if (sc > best.score) {
                        best = {ActType::Block, sc, c, def, {0,0}};
                    }
                }
            }
        }

        {
            auto playable = me.getPlayableCharacter();
            for (auto& ch : playable) {
                if (!ch) continue;

                auto pos = ch->getPosition();
                int mv = ch->getMovement();

                for (int dx = -mv; dx <= mv; dx++) {
                    for (int dy = -mv; dy <= mv; dy++) {
                        if (std::max(std::abs(dx), std::abs(dy)) > mv) continue;
                        std::pair<int,int> dest = {pos.first + dx, pos.second + dy};
                        if (!isInside(dest)) continue;

                        float sc = scoreMove(ch, dest);
                        if (sc > best.score) {
                            best = {ActType::Move, sc, ch, nullptr, dest};
                        }
                    }
                }
            }
        }

        if (best.score <= -1e8f) {
            std::cout << "[ADVANCED AI] No valid actions available.\n";
            return false;
        }

        switch (best.type) {
            case ActType::Move: {
                engine.addCommand(std::make_unique<::engine::Move>(best.a, best.dest));
                break;
            }
                case ActType::Block: {
                engine.addCommand(std::make_unique<::engine::Block>(best.a, best.b));
                break;
            }
            case ActType::Pass: {
                engine.addCommand(std::make_unique<::engine::Pass>(best.a, best.b));
                break;
            }
        }

        std::cout << "[ADVANCED AI] Best action score=" << best.score << "\n";
        return true;
    }
}
