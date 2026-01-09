#include "EndGame.h"
#include "Team.h"
#include "BloodBowlGame.h"
#include "Setup.h"

namespace state {
    EndGame::EndGame(BloodBowlGame* game):AbstractState(game) {
        shouldRestart = false;
        shouldQuit = false;
    }

    void EndGame::update() {}

    EndGame::~EndGame() = default;

    bool EndGame::getShouldRestart() const {
        return shouldRestart;
    }

    void EndGame::setShouldRestart(bool restartValue) {
        this->shouldRestart = restartValue;
    }

    bool EndGame::getShouldQuit() const {
        return shouldQuit;
    }

    void EndGame::setShouldQuit(bool quitValue) {
        this->shouldQuit = quitValue;
    }

    std::string EndGame::getName() const {
        return "EndGame";
    }


}
