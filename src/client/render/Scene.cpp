#include "Scene.h"

namespace render {

    Scene::Scene(SceneId sceneId, std::shared_ptr<state::BloodBowlGame> game)
        : id(sceneId), game(std::move(game)), sceneData("blue", "red") {
    }

    Scene::~Scene() = default;


    void Scene::drawScene (sf::RenderWindow& window, const std::shared_ptr<state::Character>& highlighted, const std::pair<int,int>& previewPos, bool previewExists, bool previewLegal, const std::vector<int>& diceOptions, bool showDice, const std::vector<std::pair<int,int>>& playablePositions, const std::vector<std::pair<int,int>>& movePath) {
        if (!game || !window.isOpen()) {
            return;
        }

        if (!sceneData.isBoardLoaded()) {
            sceneData.init(game);
        }

        window.clear(sf::Color::Black);
        sceneData.updatePositions(game);

        int tId = -1;
        const std::string sName = (game && game->getCurrentState()) ? game->getCurrentState()->getName() : std::string();
        if (game && game->getCurrentTeam()) tId = game->getCurrentTeam()->getTeamId();

        sceneData.draw(window, highlighted, previewPos, previewExists, previewLegal, diceOptions, showDice, playablePositions, movePath, sName, tId);
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

    const std::vector<sf::FloatRect> &Scene::getDiceOptionBounds() const {
        return sceneData.getDiceOptionBounds();
    }


} // namespace render