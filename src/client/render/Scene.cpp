#include "Scene.h"
#include "utility/Constants.h"

namespace render {

    Scene::Scene(SceneId sceneId, std::shared_ptr<state::BloodBowlGame> game)
        : id(sceneId), game(std::move(game)), sceneData("blue", "red") {
    }

    Scene::~Scene() = default;

    void Scene::drawScene(sf::RenderWindow& window, std::shared_ptr<state::Character> highlighted) {
        if (!game || !window.isOpen()) return;

        // Initialize sceneData textures/sprites on first draw if needed
        if (!sceneData.isBoardLoaded()) {
            sceneData.init(window, game);
        }

        window.clear(sf::Color::Black);
        sceneData.updatePositions(game);  // Update sprite positions from game state
        sceneData.draw(window, highlighted);
        window.display();
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
    }

    SceneData Scene::getSceneData() const {
        return sceneData;
    }

    void Scene::setSceneData(SceneData& newSceneData) {
        sceneData = newSceneData;
    }

} // namespace render
