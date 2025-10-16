#include "Setup.h"


#include "Kickoff.h"
#include "BloodBowlGame.h"
#include <set>

constexpr std::pair<int, int> HORS_TABLEAU = {-1, -1};

namespace state {
    Setup::Setup(BloodBowlGame *game) : AbstractState(game) {
    }

    int Setup::nbCharacterOnBoard(Team team) {
        int count = 0;
        for (Character piece : team.getCharacters()) {
            if (piece.getStatus() == playable && piece.getPosition() != HORS_TABLEAU) {
                count++;
            }
        }
        return count;
    }

    int Setup::nbCharacterOnLine(Team team) {

        bool isTeamA = (team.getTeamId() == game->getTeamA().getTeamId());
        int frontlineCol;
        if (isTeamA) {
            frontlineCol = 12;
        } else {
            frontlineCol = 13;
        }

        int count = 0;
        for (Character piece : team.getCharacters()) {
            if (piece.getStatus() != playable) continue;
            auto pos = piece.getPosition();
            if (pos == HORS_TABLEAU) continue;
            // pos.first is row
            if (pos.first == frontlineCol) count++;
        }
        return count;
    }

    int Setup::nbCharacterOnTop(Team team) {
        int height = game->getHeight();
        int limitTopRow = height - 4; // (height-1) - 4 => les 4 lignes du haut du terrain  (Attention height est le nombre de lignes!!)

        int count = 0;
        for (Character piece : team.getCharacters()) {
            auto pos = piece.getPosition();
            if (piece.getStatus() == playable && pos != HORS_TABLEAU && pos.second >= limitTopRow) { //Attention .second ! On souhaite comparer des ordonnées!
                count++;
            }
        }
        return count;
    }

    int Setup::nbCharacterOnBottom(Team team) {
        int limitBottomRow = 3;

        int count = 0;
        for (Character piece : team.getCharacters()) {
            auto pos = piece.getPosition();
            if (piece.getStatus() == playable && pos != HORS_TABLEAU && pos.second <= limitBottomRow) { //Idem
                count++;
            }
        }
        return count;
    }


    bool Setup::isValidSetup(Team team) {
        int onBoard = nbCharacterOnBoard(team);
        if (onBoard > 11) return false;

        int onLine = nbCharacterOnLine(team);
        if (onLine < 3) return false;

        int onTop = nbCharacterOnTop(team);
        if (onTop > 2) return false;

        int onBottom = nbCharacterOnBottom(team);
        if (onBottom > 2) return false;

        return true;
    }

    void Setup::update() {
        if (setupEnded) {
            game->setCurrentState(game->getStateList().at(KICKOFF));
        }
    }

    void Setup::endSetup() {
        setupEnded = true;
    }



}
