#include "Scene.h"
#include "InputHandler.h"
#include "utility/Constants.h"

namespace render{

    Scene::Scene(SceneId sceneId, std::shared_ptr<state::BloodBowlGame> game, engine::Engine* engine)
    : engine(engine), id(sceneId), window(nullptr), game(std::move(game)), sceneData("blue","red")
    {
        window = new sf::RenderWindow(sf::VideoMode(utility::Constants::WINDOW_WIDTH,utility::Constants::WINDOW_HEIGHT),"BloodBowl");
        sceneData.init(window, this->game);

        inputHandler = std::make_unique<InputHandler>(this->game, engine);
    }

    Scene::~Scene() {
        if (window) {
            window->close();
            delete window;
            window = nullptr;
        }
    }

    void Scene::drawScene()
    {
        if (!window) {
            return;
        }
        if (game && window->isOpen()) {
            window->clear(sf::Color::Black);
            sceneData.updatePositions(game);  // Update sprite positions from game state
            sceneData.draw(window);
            window->display();
        }
    }

    void Scene::handleEvent(const sf::Event& event) {
        if (!inputHandler) return;

        if (event.type == sf::Event::MouseButtonPressed) {
            inputHandler->handleMouseClick(event.mouseButton, window);
        } else if (event.type == sf::Event::KeyPressed) {
            inputHandler->handleKeyPress(event.key);
        }
    }

    SceneId Scene::getId() const {
        return id;
    }

    void Scene::setId(SceneId idValue) {
        id = idValue;
    }

    std::shared_ptr<state::BloodBowlGame> Scene::getGame() const {
        return game;
    }
    void Scene::setGame(std::shared_ptr<state::BloodBowlGame>& newGame) {
        game = newGame;
        if (window && game && !sceneData.isBoardLoaded()) {
            sceneData.init(window, game);
        }
    }

    sf::RenderWindow* Scene::getWindow() const{
        return window;
    }

    void Scene::setWindow(sf::RenderWindow* newWindow) {
        this->window = newWindow;
        if (window && game && !sceneData.isBoardLoaded()) {
            sceneData.init(window, game);
        }
    }

    SceneData Scene::getSceneData() const {
        return sceneData;
    }
    void Scene::setSceneData(SceneData& newSceneData) {
        sceneData = newSceneData;
    }

    InputHandler* Scene::getInputHandler() const {
        return inputHandler.get();
    }

}
