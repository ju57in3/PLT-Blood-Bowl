#include "GameScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "../render/Scene.h"
#include "../client/InputHandler.h"
#include <engine/Engine.h>

namespace screen {
    GameScreen::GameScreen() = default;

    GameScreen::~GameScreen() = default;

    void GameScreen::init(const std::shared_ptr<state::BloodBowlGame> &g, ResourceManager *res) {
        this->resources = res;
        this->game = g;
        // create render::Scene
        scene = std::make_unique<render::Scene>(render::SceneId::GAME, game);
        // optional: load a font or other resources if needed for overlays
        // engine is provided via manager
        engine::Engine *eng = nullptr;
        if (manager) eng = manager->getEngine();
        if (eng) {
            // create InputHandler using engine's pointer and game
            inputHandler = std::make_unique<client::InputHandler>(game, eng);
        }
    }

    void GameScreen::setManager(SceneManager *mgr) { this->manager = mgr; }

    void GameScreen::handleEvent(const sf::Event &event, sf::RenderWindow &window) {
        // forward to InputHandler if present
        if (inputHandler) {
            // construct dummy dice bounds
            std::vector<sf::FloatRect> dummy;
            inputHandler->handleEvent(event, &window, dummy);
        }
        // allow pressing escape to end game and go to end screen
        if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape) {
            endRequested = true;
            if (manager) manager->switchTo(render::SceneId::END_GAME);
        }
    }

    void GameScreen::update(float dt) {
        // update game or input-based logic if necessary
        if (game && game->getCurrentState()) game->getCurrentState()->update();
    }

    void GameScreen::draw(sf::RenderWindow &window) {
        window.clear(sf::Color::Black);
        if (scene) {
            // Draw the game scene using scene->drawScene
            scene->drawScene(window, nullptr, {0, 0}, false, false, {}, false, {}, {});
        }
    }

    render::SceneId GameScreen::getId() const {
        return render::SceneId::GAME;
    }
} // namespace screen
