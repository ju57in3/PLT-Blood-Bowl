#include "Scene.h"
#include "utility/Constants.h"

namespace render {

    Scene::Scene(SceneId sceneId, std::shared_ptr<state::BloodBowlGame> game)
        : id(sceneId), game(std::move(game)), sceneData("blue", "red") {
    }

    Scene::~Scene() = default;


    void Scene::drawScene (sf::RenderWindow& window, const std::shared_ptr<state::Character>& highlighted, const std::pair<int,int>& previewPos, bool previewExists, bool previewLegal, const std::vector<int>& diceOptions, bool showDice) {
        if (!game || !window.isOpen()) return;

        // Initialize sceneData textures/sprites on first draw if needed
        if (!sceneData.isBoardLoaded()) {
            sceneData.init(game);
        }

        window.clear(sf::Color::Black);
        sceneData.updatePositions(game);  // Update sprite positions from game state
        // draw preview under characters so it appears as tile highlight
        sceneData.draw(window, highlighted, previewPos, previewExists, previewLegal, diceOptions, showDice);
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

    const std::vector<sf::FloatRect> &Scene::getDiceOptionBounds() const {
        return sceneData.getDiceOptionBounds();
    }


} // namespace render