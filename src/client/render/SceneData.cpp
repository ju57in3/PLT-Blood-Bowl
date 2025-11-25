#include "SceneData.h"
#include "utility/Constants.h"
#include <filesystem>
#include <iostream>

using namespace utility;

namespace render{

    static bool loadTextureFromFile(const std::string& path, sf::Texture& texture) {
        if (!std::filesystem::exists(path)) {
            return false;
        }
        if (!texture.loadFromFile(path)) {
            return false;
        }
        return true;
    }

    static sf::Vector2f pos2Coords(const std::pair<int,int> &pos) {
        const int stride = Constants::BOARD_TILE_PIXEL_SIZE + Constants::BOARD_TILE_SPACING;
        const int x = Constants::BOARD_OFFSET_X + stride * pos.first;
        const int y = Constants::BOARD_OFFSET_Y + stride * pos.second;
        return {static_cast<float>(x), static_cast<float>(y)};
    }

    SceneData::SceneData(std::string colorA, std::string colorB)
    : colorA(std::move(colorA)), colorB(std::move(colorB))
    {
        // prepare dice textures vector empty; will be loaded in init if available
        diceTextures.clear();
        diceSprites.clear();
    }

    SceneData::~SceneData() = default;

    bool SceneData::isBoardLoaded() const {
        return boardTexture.getSize().x > 0 && boardTexture.getSize().y > 0;
    }

    void SceneData::init(const std::shared_ptr<state::BloodBowlGame>& game)
    {
        playersTextures_TeamA.resize(Constants::MAX_PLAYERS_PER_TEAM);
        playersTextures_TeamB.resize(Constants::MAX_PLAYERS_PER_TEAM);
        playersSprites_TeamA.resize(Constants::MAX_PLAYERS_PER_TEAM);
        playersSprites_TeamB.resize(Constants::MAX_PLAYERS_PER_TEAM);

        if (loadTextureFromFile("../res/board.png", boardTexture)) {
            board.setTexture(boardTexture);
        } else {
            std::cerr << "Error loading res/board.png" << std::endl;
        }

        const std::string charactersDir = "../res/characters/";
        if (!std::filesystem::exists(charactersDir) || !std::filesystem::is_directory(charactersDir)) {
            std::cerr << "Error: characters directory does not exist: " << charactersDir << std::endl;
            // try fallback
        }

        for ( const auto& character : game->getTeamA().getCharacters()) {
            const std::string texturePath = charactersDir + character->getType() + "_"+ colorA + ".png";
            const auto index = static_cast<size_t>(character->getId());
            if (index < Constants::MAX_PLAYERS_PER_TEAM && loadTextureFromFile(texturePath, playersTextures_TeamA.at(index))) {
                playersSprites_TeamA.at(index).setTexture(playersTextures_TeamA.at(index));
                playersSprites_TeamA.at(index).setPosition(pos2Coords(character->getPosition()));
            } else {
                std::cerr << "Error loading " << texturePath << std::endl;
            }
        }

        for ( const auto& character : game->getTeamB().getCharacters()) {
            const std::string texturePath = charactersDir + character->getType() + "_"+ colorB + ".png";
            const auto index = static_cast<size_t>(character->getId());
            if (index < Constants::MAX_PLAYERS_PER_TEAM && loadTextureFromFile(texturePath, playersTextures_TeamB.at(index))) {
                playersSprites_TeamB.at(index).setTexture(playersTextures_TeamB.at(index));
                playersSprites_TeamB.at(index).setPosition(pos2Coords(character->getPosition()));
            } else {
                std::cerr << "Error loading " << texturePath << std::endl;
            }
        }

        if (loadTextureFromFile("../res/ball.png", ballTexture) || loadTextureFromFile("res/ball.png", ballTexture)) {
            ballSprite.setTexture(ballTexture);
            ballSprite.setPosition(pos2Coords(game->getBallPosition()));
        }

        // Load dice textures from res/BlockDice with names like "1.png".."6.png" or named images
        const std::string diceDir = "../res/BlockDice/";
        diceTextures.resize(6);
        diceSprites.resize(6);
        // fallback names for faces: index 1..6 -> name
        const std::vector<std::string> fallbackNames = {
            "AttackerDown.png", // face 1
            "BothDown.png",     // face 2
            "Pushed.png",       // face 3
            "Pushed.png",       // face 4 (same as 3)
            "DefenderStumbles.png", // face 5
            "DefenderDown.png"  // face 6
        };

        for (int face = 1; face <= 6; ++face) {
            const int idx = face - 1;
            std::string p1 = diceDir + std::to_string(face) + ".png";
            if (loadTextureFromFile(p1, diceTextures[idx])) {
                diceSprites[idx].setTexture(diceTextures[idx]);
            } else {
                // try descriptive fallback
                std::string pFallback = diceDir + fallbackNames[idx];
                if (loadTextureFromFile(pFallback, diceTextures[idx])) {
                    diceSprites[idx].setTexture(diceTextures[idx]);
                } else {
                    // leave texture empty; fallback drawing will render number
                }
            }
        }
        // Load default font for fallback dice labels
        if (!defaultFont.loadFromFile("../res/arial.ttf")) {
            // silent if not present, fallback drawing will skip text
        }
    }

    void SceneData::draw(sf::RenderWindow& window, const std::shared_ptr<state::Character>& highlighted, const std::pair<int,int>& previewPos, bool previewExists, bool legal, const std::vector<int>& diceOptions, bool showDice)
    {
        window.draw(board);

        // draw preview under characters so it appears as tile highlight
        if (previewExists) {
            drawPreview(window, previewPos, previewExists, legal);
        }

        // Draw highlights first (a translucent rectangle under the sprite)
        if (highlighted) {
            sf::RectangleShape highlightRect;
            highlightRect.setFillColor(sf::Color(255, 255, 0, 80)); // semi-transparent yellow
            const auto pos = pos2Coords(highlighted->getPosition());
            highlightRect.setSize(sf::Vector2f(Constants::BOARD_TILE_PIXEL_SIZE, Constants::BOARD_TILE_PIXEL_SIZE));
            highlightRect.setPosition(pos);
            window.draw(highlightRect);
        }

        for (auto& s : playersSprites_TeamA) {
            window.draw(s);
        }
        for (auto& s : playersSprites_TeamB) {
            window.draw(s);
        }
        if (ballTexture.getSize().x > 0 && ballTexture.getSize().y > 0) {
            window.draw(ballSprite);
        }

        // draw dice options overlay if requested
        if (showDice && !diceOptions.empty()) {
            // place dice icons in the top-left corner of the window (single row) with small padding
            const float padding = 8.0f;
            const float size = static_cast<float>(Constants::BOARD_TILE_PIXEL_SIZE);
            const size_t n = diceOptions.size();
            // top-left origin with a small padding from the window border
            float startX = padding;
            float y = padding;
            diceOptionBounds.clear();
            for (size_t i = 0; i < n; ++i) {
                float x = startX + i * (size + padding);
                int faceIndex = diceOptions[i] - 1;
                if (faceIndex >= 0 && static_cast<size_t>(faceIndex) < diceSprites.size() && diceSprites[faceIndex].getTexture()) {
                    // scale sprite to square 'size' while preserving aspect ratio
                    const auto bounds = diceSprites[faceIndex].getLocalBounds();
                    float sx = size / bounds.width;
                    float sy = size / bounds.height;
                    diceSprites[faceIndex].setScale(sx, sy);
                    diceSprites[faceIndex].setPosition(x, y);
                    window.draw(diceSprites[faceIndex]);
                } else {
                    // fallback: draw a simple rectangle with number
                    sf::RectangleShape r(sf::Vector2f(size, size));
                    r.setPosition(x, y);
                    r.setFillColor(sf::Color(200,200,200,200));
                    window.draw(r);
                    if (defaultFont.getInfo().family != "") {
                        sf::Text t(std::to_string(diceOptions[i]), defaultFont, 14);
                        t.setPosition(x + size/4, y + size/6);
                        t.setFillColor(sf::Color::Black);
                        window.draw(t);
                    }
                }
                diceOptionBounds.emplace_back(sf::FloatRect(x, y, size, size));
            }
            // Hover visual: check mouse position in window coords
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            for (size_t i = 0; i < diceOptionBounds.size(); ++i) {
                const auto& r = diceOptionBounds[i];
                if (r.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    // draw highlight rectangle (outline) around hovered die
                    sf::RectangleShape outline(sf::Vector2f(r.width, r.height));
                    outline.setPosition(r.left, r.top);
                    outline.setFillColor(sf::Color(0,0,0,0));
                    outline.setOutlineThickness(3.0f);
                    outline.setOutlineColor(sf::Color(255, 255, 0, 160));
                    window.draw(outline);
                    break; // only highlight first hovered
                }
            }
         } else {
             // clear bounds if not shown
             diceOptionBounds.clear();
         }
    }

    void SceneData::drawPreview(sf::RenderWindow& target, const std::pair<int,int>& previewPos, bool previewExists, bool legal)
    {
        if (!previewExists) return;
        const auto pos = pos2Coords(previewPos);
        sf::RectangleShape rect;
        rect.setSize(sf::Vector2f(Constants::BOARD_TILE_PIXEL_SIZE, Constants::BOARD_TILE_PIXEL_SIZE));
        rect.setPosition(pos);
        if (legal) {
            rect.setFillColor(sf::Color(0, 255, 0, 80)); // semi-transparent green
        } else {
            rect.setFillColor(sf::Color(255, 0, 0, 80)); // semi-transparent red
        }
        target.draw(rect);
    }

    void SceneData::updatePositions(const std::shared_ptr<state::BloodBowlGame>& game)
    {
        if (!game) return;

        // Update TeamA positions
        for (const auto& character : game->getTeamA().getCharacters()) {
            const auto index = static_cast<size_t>(character->getId());
            if (index < Constants::MAX_PLAYERS_PER_TEAM) {
                playersSprites_TeamA.at(index).setPosition(pos2Coords(character->getPosition()));
            }
        }

        // Update TeamB positions
        for (const auto& character : game->getTeamB().getCharacters()) {
            const auto index = static_cast<size_t>(character->getId());
            if (index < Constants::MAX_PLAYERS_PER_TEAM) {
                playersSprites_TeamB.at(index).setPosition(pos2Coords(character->getPosition()));
            }
        }

        // Update ball position
        ballSprite.setPosition(pos2Coords(game->getBallPosition()));
    }

    const std::vector<sf::FloatRect>& SceneData::getDiceOptionBounds() const {
        return diceOptionBounds;
    }

}

