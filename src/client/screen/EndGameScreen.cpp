#include "EndGameScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "LayoutHelper.h"
#include "../../shared/utility/Constants.h"
#include "../../shared/state/EndGame.h"
#include "../../shared/state/TeamManager.h"

namespace screen {
    EndGameScreen::EndGameScreen() = default;

    EndGameScreen::~EndGameScreen() = default;

    void EndGameScreen::init(const std::shared_ptr<state::BloodBowlGame> &gamePtr, ResourceManager *resourcesPtr) {
        this->resources = resourcesPtr;
        this->game = gamePtr;
        const float windowWidth = utility::Constants::WINDOW_WIDTH;
        const float windowHeight = utility::Constants::WINDOW_HEIGHT;

        try { font = resources->getFont("fonts/bloodbowl.ttf"); } catch (...) {
        }

        // Titre principal - centré
        title.setFont(font);
        title.setString("Fin de la partie !");
        title.setCharacterSize(36);
        title.setFillColor(sf::Color::White);
        LayoutHelper::setRelativeY(title, windowHeight, 0.05f);
        LayoutHelper::centerHorizontally(title, windowWidth);

        // Affichage des scores
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        LayoutHelper::setRelativeY(scoreText, windowHeight, 0.25f);
        LayoutHelper::centerHorizontally(scoreText, windowWidth);

        // Texte du gagnant
        winnerText.setFont(font);
        winnerText.setCharacterSize(28);
        winnerText.setFillColor(sf::Color::Yellow);
        LayoutHelper::setRelativeY(winnerText, windowHeight, 0.15f);
        LayoutHelper::centerHorizontally(winnerText, windowWidth);

        // Bouton Rejouer - centré au milieu
        restartButton.setSize({200, 50});
        restartButton.setFillColor(sf::Color(50, 150, 50));
        restartButton.setPosition(windowWidth / 2 - 220, windowHeight / 2 + 50);

        restartText.setFont(font);
        restartText.setString("Rejouer");
        restartText.setCharacterSize(22);
        restartText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(restartText, restartButton);

        // Bouton retour au menu - à côté du bouton Rejouer
        backButton.setSize({200, 50});
        backButton.setFillColor(sf::Color(70, 70, 70));
        backButton.setPosition(windowWidth / 2 + 20, windowHeight / 2 + 50);

        backText.setFont(font);
        backText.setString("Menu");
        backText.setCharacterSize(22);
        backText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(backText, backButton);
    }

    void EndGameScreen::setManager(SceneManager *mgr) { manager = mgr; }

    void EndGameScreen::onEnter() {
        // Récupérer le jeu actuel du SceneManager
        if (manager) {
            game = manager->getGame();
        }
    }

    void EndGameScreen::handleEvent(const sf::Event &event, sf::RenderWindow &window) {
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mpos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

            if (backButton.getGlobalBounds().contains(mpos)) {
                // Sauvegarder les équipes avant de retourner au menu
                // Les changements persistants (XP, blessures, etc.) seront conservés
                auto& teamManager = state::TeamManager::getInstance();
                teamManager.saveToDisk("teams.json");

                if (manager) manager->switchTo(render::SceneId::HOME);
            }
            else if (restartButton.getGlobalBounds().contains(mpos)) {
                // Réinitialiser le jeu et retourner au Setup
                if (game) {
                    // Position hors plateau pour indiquer que le personnage n'est pas sur le terrain
                    const std::pair<int, int> OFF_BOARD = {-1, -1};

                    // Remettre tous les personnages sur le banc ET réinitialiser leurs positions
                    for (auto& characterPtr : game->getTeamA().getCharacters()){
                        if (characterPtr != nullptr){
                            characterPtr->setStatus(state::bench);
                            characterPtr->setPosition(OFF_BOARD);  // Réinitialiser la position
                            characterPtr->setHasBall(false);       // Le personnage n'a plus la balle
                            characterPtr->gotUp = false;           // Réinitialiser le flag gotUp
                        }
                    }
                    for (auto& characterPtr : game->getTeamB().getCharacters()){
                        if (characterPtr != nullptr){
                            characterPtr->setStatus(state::bench);
                            characterPtr->setPosition(OFF_BOARD);  // Réinitialiser la position
                            characterPtr->setHasBall(false);       // Le personnage n'a plus la balle
                            characterPtr->gotUp = false;           // Réinitialiser le flag gotUp
                        }
                    }

                    // Réinitialiser l'état de la balle
                    game->setBallPosition(OFF_BOARD);
                    game->setBallIsHold(false);

                    // Réinitialiser le compteur de tours
                    game->setTurnCounter(0);

                    // Revenir à l'état Setup
                    game->setCurrentState(game->getStateList().at(state::SETUP).get());

                    // Retourner à l'écran de jeu
                    if (manager) manager->switchTo(render::SceneId::GAME);
                }
            }
        }
    }

    void EndGameScreen::update() {
        // Mettre à jour l'affichage des scores
        if (game) {
            int scoreA = game->getTeamA().getScore();
            int scoreB = game->getTeamB().getScore();

            scoreText.setString(
                game->getTeamA().getName() + ": " + std::to_string(scoreA) +
                "  -  " +
                game->getTeamB().getName() + ": " + std::to_string(scoreB)
            );
            LayoutHelper::centerHorizontally(scoreText, utility::Constants::WINDOW_WIDTH);

            // Déterminer le gagnant
            if (scoreA > scoreB) {
                winnerText.setString(game->getTeamA().getName() + " remporte la victoire !");
            } else if (scoreB > scoreA) {
                winnerText.setString(game->getTeamB().getName() + " remporte la victoire !");
            } else {
                winnerText.setString("Match nul !");
            }
            LayoutHelper::centerHorizontally(winnerText, utility::Constants::WINDOW_WIDTH);
        }
    }

    void EndGameScreen::draw(sf::RenderWindow &window) {
        window.clear(sf::Color(30, 30, 30));
        window.draw(title);
        window.draw(winnerText);
        window.draw(scoreText);
        window.draw(restartButton);
        window.draw(restartText);
        window.draw(backButton);
        window.draw(backText);
    }

    render::SceneId EndGameScreen::getId() const {
        return render::SceneId::END_GAME;
    }
} // namespace screen
