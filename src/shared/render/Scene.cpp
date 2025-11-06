#include "Scene.h"

namespace render{

    Scene::Scene(SceneId sceneId, std::shared_ptr<state::BloodBowlGame> game)
    : id(sceneId), game(game), sceneData()
    {

    }

    Scene::~Scene() {}

    void Scene::init(SceneId sceneId, std::shared_ptr<state::BloodBowlGame> game)
    {
        this->id = sceneId;
        this->game = std::move(game);
    }

    void Scene::drawScene()
    {
        if (game && window.isOpen()) {
            window.clear(sf::Color::Black);
            sceneData.draw(window);
            window.display();
        }
    }

    SceneId Scene::getId() const {
        return id;
    }
    void Scene::setId(SceneId newId) {
        id = newId;
    }

    const std::shared_ptr<state::BloodBowlGame>& Scene::getGame() const {
        return game;
    }
    void Scene::setGame(const std::shared_ptr<state::BloodBowlGame>& newGame) {
        game = newGame;
    }

    const SceneData& Scene::getSceneData() const {
        return sceneData;
    }
    void Scene::setSceneData(const SceneData& newSceneData) {
        sceneData = newSceneData;
    }

}
