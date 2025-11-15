#include "Scene.h"

#include "utility/Constants.h"

namespace render{

    Scene::Scene(SceneId sceneId, std::shared_ptr<state::BloodBowlGame> game)
    : id(sceneId), window(nullptr), game(std::move(game)), sceneData("blue","red")
    {
        window = new sf::RenderWindow(sf::VideoMode(1920,1080),"BloodBowl");
        sceneData.init(window, this->game);
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
