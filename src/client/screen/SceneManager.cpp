#include "SceneManager.h"
#include "ResourceManager.h"
#include <iostream>

namespace screen {

SceneManager::SceneManager(sf::RenderWindow* window, ResourceManager* resources) : window(window), resources(resources) {}

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

Screen* SceneManager::getCurrent() {
  if (stack.empty()) return nullptr;
  return screens[stack.back()].get();
}

void SceneManager::handleEvent(const sf::Event& event) {
  Screen* cur = getCurrent();
  if (cur) cur->handleEvent(event, *window);
}

void SceneManager::update(float dt) {
  Screen* cur = getCurrent();
  if (cur) cur->update(dt);
}

void SceneManager::draw() {
  if (!window) return;
  window->clear(sf::Color::Black);
  Screen* cur = getCurrent();
  if (cur) cur->draw(*window);
  window->display();
}

} // namespace screen
