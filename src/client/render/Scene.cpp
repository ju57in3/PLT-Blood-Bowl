#include "Scene.h"

namespace render {

    Scene::Scene(SceneId sceneId, std::shared_ptr<state::BloodBowlGame> game)
        : id(sceneId), game(std::move(game)), sceneData("blue", "red") {
    }

    Scene::~Scene() = default;


    void Scene::drawScene (sf::RenderWindow& window, const GameRenderData& renderData) {
        if (!game || !window.isOpen()) {
            return;
        }

        if (!sceneData.isBoardLoaded()) {
            sceneData.init(game);
        }

        window.clear(sf::Color::Black);
        sceneData.updatePositions(game);
        sceneData.draw(window, renderData);
        window.display();
    }


    SceneId Scene::getId() const {
        return id;
    }

    void Scene::setId(SceneId idValue) {
        id = idValue;
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

    std::vector<sf::FloatRect> Scene::computeDiceBounds(const std::vector<int>& diceOptions) {
        return sceneData.computeDiceBounds(diceOptions);
    }


} // namespace render