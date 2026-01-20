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

        try { font = resources->getFont("fonts/bloodbowl.ttf"); } catch (...) {
        }

        // Titre principal - centré
        title.setFont(font);
        title.setString("Fin de la partie !");
        title.setCharacterSize(48);
        title.setFillColor(sf::Color::White);
        LayoutHelper::setRelativePosition(title, 0.5f, 0.10f);

        // Texte du gagnant
        winnerText.setFont(font);
        winnerText.setCharacterSize(32);
        winnerText.setFillColor(sf::Color::Yellow);
        LayoutHelper::setRelativePosition(winnerText, 0.5f, 0.22f);

        // Affichage des scores
        scoreText.setFont(font);
        scoreText.setCharacterSize(28);
        scoreText.setFillColor(sf::Color::White);
        LayoutHelper::setRelativePosition(scoreText, 0.5f, 0.32f);

        // Bouton Rejouer - centré
        restartButton.setSize({240, 65});
        restartButton.setFillColor(sf::Color(70, 150, 70));
        restartButton.setOutlineColor(sf::Color(150, 150, 170));
        restartButton.setOutlineThickness(2);
        LayoutHelper::setRelativePosition(restartButton, 0.35f, 0.55f);

        restartText.setFont(font);
        restartText.setString("Rejouer");
        restartText.setCharacterSize(26);
        restartText.setFillColor(sf::Color::White);
        LayoutHelper::centerTextInRect(restartText, restartButton);

        // Bouton retour au menu - à côté du bouton Rejouer
        backButton.setSize({240, 65});
        backButton.setFillColor(sf::Color(80, 80, 100));
        backButton.setOutlineColor(sf::Color(150, 150, 170));
        backButton.setOutlineThickness(2);
        LayoutHelper::setRelativePosition(backButton, 0.65f, 0.55f);

        backText.setFont(font);
        backText.setString("Menu");
        backText.setCharacterSize(26);
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

                    // Reset le score
                    game->getTeamA().setScore(0);
                    game->getTeamB().setScore(0);

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
            LayoutHelper::setRelativePosition(scoreText, 0.5f, 0.25f);

            // Déterminer le gagnant
            if (scoreA > scoreB) {
                winnerText.setString(game->getTeamA().getName() + " remporte la victoire !");
            } else if (scoreB > scoreA) {
                winnerText.setString(game->getTeamB().getName() + " remporte la victoire !");
            } else {
                winnerText.setString("Match nul !");
            }
            LayoutHelper::setRelativePosition(winnerText, 0.5f, 0.17f);
        }
    }

    void EndGameScreen::draw(sf::RenderWindow &window) {
        window.clear(sf::Color(50, 50, 60)); // Uniform gray background
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
