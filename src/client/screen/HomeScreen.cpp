#include "HomeScreen.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "../../shared/utility/Constants.h"

namespace screen {

HomeScreen::HomeScreen() = default;

void HomeScreen::init(const std::shared_ptr<state::BloodBowlGame>& gamePtr, ResourceManager* res) {
  this->resources = res;
  this->game = gamePtr;
  // try to load default font from resources; fall back to system font if not found
  try {
    const sf::Font& f = resources->getFont("fonts/bloodbowl.ttf");
    font = f;
  } catch (...) {
    // leave font default if load fails
  }
  setupUI();
}

void HomeScreen::setManager(SceneManager* mgr) { this->manager = mgr; }

void HomeScreen::setupUI() {
  title.setFont(font);
  title.setString("BloodBowl");
  title.setCharacterSize(48);
  title.setFillColor(sf::Color::White);
  title.setPosition(utility::Constants::WINDOW_WIDTH * 0.5f - 120.0f, 30.0f);

  subtitle.setFont(font);
  subtitle.setString("Creez vos equipes et lancez un match !");
  subtitle.setCharacterSize(18);
  subtitle.setFillColor(sf::Color(200,200,200));
  subtitle.setPosition(utility::Constants::WINDOW_WIDTH * 0.5f - 220.0f, 90.0f);

  std::vector<std::string> labels = {"Nouvelle equipe", "Creer match", "Quitter"};
  int y = 160;
  for (const auto& lbl : labels) {
    Button b;
    b.rect.setSize({300,50});
    b.rect.setFillColor(sf::Color(70,70,70));
    b.rect.setPosition(utility::Constants::WINDOW_WIDTH * 0.5f - 150.0f, static_cast<float>(y));
    b.text.setFont(font);
    b.text.setString(lbl);
    b.text.setCharacterSize(20);
    b.text.setFillColor(sf::Color::White);
    b.text.setPosition(b.rect.getPosition().x + 20, b.rect.getPosition().y + 10);
    buttons.push_back(b);
    y += 80;
  }
}

void HomeScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
  if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
    sf::Vector2f mpos = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
    for (size_t i = 0; i < buttons.size(); ++i) {
      if (buttons[i].rect.getGlobalBounds().contains(mpos)) {
        switch(i) {
          case 0: // Nouvelle équipe
            if (manager) manager->switchTo(render::SceneId::TEAM_CREATION);
            break;
          case 1: // Créer match
            if (manager) manager->switchTo(render::SceneId::MATCH_CREATION);
            break;
          case 2: // Quitter
            window.close();
            break;
          default:
            // no-op for safety
            break;
         }
      }
    }
  }
}

void HomeScreen::update(float dt) {
  // hover effect
  if (!resources) return;
}

void HomeScreen::draw(sf::RenderWindow& window) {
  // background
  window.clear(sf::Color(20,20,20));
  window.draw(title);
  window.draw(subtitle);
  for (auto &b : buttons) {
    window.draw(b.rect);
    window.draw(b.text);
  }
}

} // namespace screen
