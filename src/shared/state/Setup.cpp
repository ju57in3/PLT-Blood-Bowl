#include "Setup.h"

#include <iostream>


#include "Kickoff.h"
#include "BloodBowlGame.h"
#include "utility/Constants.h"

constexpr std::pair<int, int> HORS_TABLEAU = {-1, -1};

namespace state {
    Setup::Setup(BloodBowlGame *game) : AbstractState(game) {
    }

    int Setup::nbCharacterOnBoard(const Team& team) const{
        int count = 0;
        for (const auto& pptr : team.getCharacters()) {
            if (pptr != nullptr) {
                const Character& piece = *pptr;
                if (piece.getStatus() == playable && piece.getPosition() != HORS_TABLEAU) {
                    count++;
                }
            }
        }
        return count;
    }

    int Setup::nbCharacterOnLine(const Team& team) const {

        bool isTeamA = (team.getTeamId() == game->getTeamA().getTeamId());
        int frontlineCol;
        if (isTeamA) {
            frontlineCol = 12;
        } else {
            frontlineCol = 13;
        }

        int count = 0;
        for (const auto& ptr : team.getCharacters()) {
            if (ptr != nullptr) {
                const Character& piece = *ptr;
                auto pos = piece.getPosition();
                if (piece.getStatus() == playable && piece.getPosition() != HORS_TABLEAU && pos.first == frontlineCol) {
                    if (pos.second >= 4 && pos.second <= (utility::Constants::BOARD_HEIGHT - 4)) {
                        count++;
                    }
                }
            }
        }
        return count;
    }

    int Setup::nbCharacterOnTop(const Team& team) const{
        int height = utility::Constants::BOARD_HEIGHT;
        int limitTopRow = height - 4; // (height-1) - 3 => les 4 lignes du haut du terrain  (Attention height est le nombre de lignes!!)

        int count = 0;
        for (const auto& ptr : team.getCharacters()) {
            if (ptr != nullptr) {
                const Character& piece = *ptr;
                auto pos = piece.getPosition();
                if (piece.getStatus() == playable && pos != HORS_TABLEAU && pos.second >= limitTopRow) { //Attention .second ! On souhaite comparer des ordonnées!
                    count++;
                }
            }
        }
        return count;
    }

    int Setup::nbCharacterOnBottom(const Team& team) const{
        int limitBottomRow = 3;

        int count = 0;
        for (const auto& ptr : team.getCharacters()) {
            if (ptr != nullptr) {
                const Character& piece = *ptr;
                auto pos = piece.getPosition();
                if (piece.getStatus() == playable && pos != HORS_TABLEAU && pos.second <= limitBottomRow) { //Idem
                    count++;
                }
            }
        }
        return count;
    }


    bool Setup::isValidSetup(const Team& team) const {
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
        if (teamSetupDone[0] && teamSetupDone[1]) {
            game->setCurrentState(game->getStateList().at(KICKOFF).get());
            teamSetupDone[1] = false;
            teamSetupDone[0] = false;
        }
    }

    void Setup::endSetup() {
        // mark current team as done
        if (game && game->getCurrentTeam()) {
            int id = game->getCurrentTeam()->getTeamId();
            if (id == game->getTeamA().getTeamId()) teamSetupDone[0] = true;
            else teamSetupDone[1] = true;
            std::cout << "Team " << id << " finished setup.\n";
            // switch team for next player's setup if other team still needs to place
            game->setCurrentTeam((game->getCurrentTeam() == &game->getTeamA()) ? &game->getTeamB() : &game->getTeamA());
        }
    }

    Setup::~Setup() {

    }

    bool Setup::getSetupEnded()
    {
        return teamSetupDone[0] && teamSetupDone[1];
    }

    void Setup::setSetupEnded(bool setupStatus)
    {
        this->setupEnded = setupStatus;
    }

    std::string Setup::getName() const {
        return "Setup";
    }


}
