#include "SceneManager.h"
#include "ResourceManager.h"
#include <iostream>

namespace screen {
    SceneManager::SceneManager(sf::RenderWindow *window, ResourceManager *resources) : window(window),
        resources(resources) {
    }

    SceneManager::~SceneManager() {
        // Clean up the engine if it exists
        if (engine != nullptr) {
            delete engine;
            engine = nullptr;
        }
    }

    void SceneManager::registerScreen(std::unique_ptr<Screen> screen) {
        render::SceneId id = screen->getId();
        screen->setManager(this);
        screen->init(game, resources);
        screens[id] = std::move(screen);
    }

    void SceneManager::switchTo(render::SceneId id) {
        if (!screens.count(id)) {
            std::cerr << "SceneManager: unknown scene id\n";
            return;
        }
        if (!stack.empty()) {
            auto cur = screens[stack.back()].get();
            if (cur) cur->onExit();
        }
        stack.clear();
        stack.push_back(id);
        screens[id]->onEnter();
    }

    void SceneManager::push(render::SceneId id) {
        if (!screens.count(id)) return;
        if (!stack.empty()) screens[stack.back()]->onExit();
        stack.push_back(id);
        screens[id]->onEnter();
    }

    void SceneManager::pop() {
        if (stack.empty()) return;
        auto cur = screens[stack.back()].get();
        if (cur) cur->onExit();
        stack.pop_back();
        if (!stack.empty()) screens[stack.back()]->onEnter();
    }

    Screen *SceneManager::getCurrent() {
        if (stack.empty()) return nullptr;
        return screens[stack.back()].get();
    }

    void SceneManager::init(const std::shared_ptr<state::BloodBowlGame> &gamePtr, ResourceManager *res) {
        game = gamePtr;
        if (!resources) resources = res;
    }

    void SceneManager::handleEvent(const sf::Event &event, sf::RenderWindow &windowRef) {
        Screen *cur = getCurrent();
        if (cur && window) cur->handleEvent(event, *window);
    }

    void SceneManager::update() {
        Screen *cur = getCurrent();
        if (cur) cur->update();
    }

    void SceneManager::draw(sf::RenderWindow &window) {
        window.clear(sf::Color::Black);
        Screen *cur = getCurrent();
        if (cur) cur->draw(window);
        window.display();
    }

    render::SceneId SceneManager::getId() const {
        return render::SceneId::HOME;
    }

    void SceneManager::setGame(const std::shared_ptr<state::BloodBowlGame> &gamePtr) {
        game = gamePtr;
    }

    void SceneManager::setEngine(engine::Engine *eng) {
        // Delete the old engine if it exists
        if (engine != nullptr && engine != eng) {
            delete engine;
        }
        engine = eng;
    }

    engine::Engine *SceneManager::getEngine() const {
        return engine;
    }

    std::shared_ptr<state::BloodBowlGame> SceneManager::getGame() const {
        return game;
    }
} // namespace screen
