#include "Scene.h"

namespace render{

Scene::Scene(SceneId sceneId, std::shared_ptr<state::BloodBowlGame> game)
: id(sceneId), game(game), sceneData()
{
    // default window constructed by sf::RenderWindow default ctor is not usable for real rendering
    // leave it to the caller to create/open a real window if needed
}

Scene::~Scene() {}

void Scene::init(SceneId sceneId, std::shared_ptr<state::BloodBowlGame> game)
{
    this->id = sceneId;
    this->game = game;
    // If we had a valid window, initialize sceneData with it. We don't own the window here,
    // so only initialize sceneData when a proper window is available.
}

void Scene::drawScene()
{
    // If the window is open, draw the sceneData onto it
    // Note: window is a public member in the class; check if it's valid
    // SFML RenderWindow has a valid default state but isOpen() will be false until created/opened
    if (game && window.isOpen()) {
        window.clear(sf::Color::Black);
        sceneData.draw(window);
        window.display();
    }
}

SceneId Scene::getId() const { return id; }
void Scene::setId(SceneId newId) { id = newId; }

const std::shared_ptr<state::BloodBowlGame>& Scene::getGame() const { return game; }
void Scene::setGame(const std::shared_ptr<state::BloodBowlGame>& newGame) { game = newGame; }

const SceneData& Scene::getSceneData() const { return sceneData; }
void Scene::setSceneData(const SceneData& newSceneData) { sceneData = newSceneData; }

}
