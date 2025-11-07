#include "Scene.h"

namespace render{

    Scene::Scene(SceneId sceneId, std::shared_ptr<state::BloodBowlGame> game)
    : id(sceneId), window(nullptr), game(std::move(game)), sceneData("blue","red")
    {
    }

    Scene::~Scene() {}

    void Scene::init(SceneId sceneId, std::shared_ptr<state::BloodBowlGame> game)
    {
        this->id = sceneId;
        this->game = std::move(game);
        if (window && this->game && !sceneData.isBoardLoaded()) {
            sceneData.init(window, this->game);
        }
    }

    void Scene::drawScene()
    {
        if (!window) {
            return;
        }
        if (game && window->isOpen()) {
            window->clear(sf::Color::Black);
            sceneData.draw(window);
            window->display();
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

}
