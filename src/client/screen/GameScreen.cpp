#include "GameScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "../render/Scene.h"
#include "../client/InputHandler.h"
#include <engine/Engine.h>
#include <state/Setup.h>

namespace screen {
    GameScreen::GameScreen() = default;

    GameScreen::~GameScreen() = default;

    void GameScreen::init(const std::shared_ptr<state::BloodBowlGame> &g, ResourceManager *res) {
        this->resources = res;
        this->game = g;
        scene = std::make_unique<render::Scene>(render::SceneId::GAME, game);
        engine::Engine *eng = nullptr;
        if (manager) eng = manager->getEngine();
        if (eng) {
            inputHandler = std::make_unique<client::InputHandler>(game, eng);
        }
    }

    void GameScreen::setManager(SceneManager *mgr) { this->manager = mgr; }

    void GameScreen::onEnter() {
        endRequested = false;
    }

    void GameScreen::handleEvent(const sf::Event &event, sf::RenderWindow &window) {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left &&
            inputHandler && inputHandler->hasPendingBlock() &&
            !inputHandler->hasPendingPush()) {

            auto diceOptions = inputHandler->getPendingBlockDiceOptions();
            if (!diceOptions.empty() && scene) {
                auto diceBounds = scene->computeDiceBounds(diceOptions);
                sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                handleDiceClick(mousePos, diceBounds);
                return; // Event consommé
            }
        }

        // Déléguer le reste à InputHandler
        if (inputHandler) {
            inputHandler->handleEvent(event, &window);
        }

        // Échap pour quitter
        if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape) {
            endRequested = true;
            if (manager) manager->switchTo(render::SceneId::END_GAME);
        }
    }

    void GameScreen::update() {
        // update game or input-based logic if necessary
        if (game && game->getCurrentState()) {
            game->getCurrentState()->update();

            // Gérer l'IA selon l'état actuel
            if (manager && manager->getEngine()) {
                auto* currentState = game->getCurrentState();

                // Détection de l'état Setup
                if (auto* setupState = dynamic_cast<state::Setup*>(currentState)) {
                    // L'IA place ses joueurs pendant le setup
                    manager->getEngine()->runAISetupIfNeeded(setupState);
                } else {
                    // Phase de jeu normale
                    manager->getEngine()->runAITurnIfNeeded();
                }

                // Exécuter les commandes en attente
                manager->getEngine()->executeCommand();
            }

            // Vérifier si on est passé à l'état EndGame
            if (game->getCurrentState()->getName() == "EndGame" && !endRequested) {
                endRequested = true;
                if (manager) manager->switchTo(render::SceneId::END_GAME);
            }
        }
    }

    void GameScreen::draw(sf::RenderWindow &window) {
        window.clear(sf::Color::Black);
        if (scene) {
            auto renderData = buildRenderData();
            scene->drawScene(window, renderData);
        }
    }

    render::SceneId GameScreen::getId() const {
        return render::SceneId::GAME;
    }

    render::GameRenderData GameScreen::buildRenderData() const {
        render::GameRenderData data;

        if (!inputHandler || !game) {
            return data;
        }

        data.highlightedCharacter = inputHandler->getSelectedCharacter();
        data.previewPosition = inputHandler->getPreviewPosition();
        data.previewExists = inputHandler->hasPreviewPosition();
        data.previewLegal = inputHandler->isPreviewLegal();
        data.movePath = inputHandler->getMovePath();

        // Dés de block
        data.diceOptions = inputHandler->getPendingBlockDiceOptions();
        data.showDice = inputHandler->hasPendingBlock() && !data.diceOptions.empty();

        // Positions de repoussée
        data.pushPositionOptions = inputHandler->getPushPositionOptions();
        data.showPushOptions = inputHandler->hasPendingPush();

        // Infos d'état
        if (game->getCurrentState()) {
            data.stateName = game->getCurrentState()->getName();
        }
        if (game->getCurrentTeam()) {
            data.currentTeamId = game->getCurrentTeam()->getTeamId();
        }
        return data;
    }

    void GameScreen::handleDiceClick(const sf::Vector2i& mousePos, const std::vector<sf::FloatRect>& diceBounds) {
        auto mx = static_cast<float>(mousePos.x);
        auto my = static_cast<float>(mousePos.y);

        for (size_t i = 0; i < diceBounds.size(); ++i) {
            if (diceBounds[i].contains(mx, my)) {
                // Appliquer le choix (1-based index)
                if (inputHandler) {
                    inputHandler->applyPendingBlockChoice(static_cast<int>(i + 1));
                }
                return;
            }
        }
    }
} // namespace screen
