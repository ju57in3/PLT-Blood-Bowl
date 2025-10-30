#include "SceneData.h"

namespace render{

    SceneData::SceneData()
        : playersSprites(), ballSprite(), board() {

    }

    SceneData::~SceneData() {}

    void SceneData::init(sf::RenderWindow& window, std::shared_ptr<state::BloodBowlGame> game)
    {
        // Initialize board size or sprite positions based on game dimensions if available
        // As we don't have textures/assets here, we'll position elements with simple defaults
        // Resize or setup shapes according to window
        (void)game; // currently unused
        sf::Vector2u size = window.getSize();
        // place ball at center by default
        ballSprite.setPosition(static_cast<float>(size.x) / 2.f, static_cast<float>(size.y) / 2.f);

        // Ensure playersSprites vector matches expected number (if game provides teams later)
        // For now leave empty; callers can populate playersSprites with actual sprites/textures
    }

    void SceneData::draw(sf::RenderTarget& target)
    {
        // Draw board first if set (if it has a texture)
        target.draw(board);

        // Draw players
        for (auto& s : playersSprites) {
            target.draw(s);
        }

        // Draw ball (CircleShape)
        target.draw(ballSprite);

    }

}
