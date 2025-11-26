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

        if (loadTextureFromFile("../res/ball.png", ballTexture)) {
            ballSprite.setTexture(ballTexture);
            ballSprite.setPosition(pos2Coords(game->getBallPosition()));
        }

        const std::string diceDir = "../res/BlockDice/";
        diceTextures.resize(6);
        diceSprites.resize(6);

        // TODO : Peut mieux faire ?
        const std::vector<std::string> id2DiceName = {
            "AttackerDown.png", // face 1
            "BothDown.png",     // face 2
            "Pushed.png",       // face 3
            "Pushed.png",       // face 4 (same as 3)
            "DefenderStumbles.png", // face 5
            "DefenderDown.png"  // face 6
        };

        for (int face = 1; face <= 6; ++face) {
            const int idx = face - 1;
            std::string fileName = diceDir + id2DiceName[idx];
            if (loadTextureFromFile(fileName, diceTextures[idx])) {
                diceSprites[idx].setTexture(diceTextures[idx]);
            }
        }
    }

    void SceneData::draw(sf::RenderWindow& window, const std::shared_ptr<state::Character>& highlighted, const std::pair<int,int>& previewPos, bool previewExists, bool legal, const std::vector<int>& diceOptions, bool showDice, const std::vector<std::pair<int,int>>& playablePositions)
    {
        window.draw(board);

        if (previewExists) {
            drawPreview(window, previewPos, previewExists, legal);
        }

        if (highlighted) {
            sf::RectangleShape highlightRect;
            highlightRect.setFillColor(sf::Color(255, 255, 0, 80)); // semi-transparent yellow
            const auto pos = pos2Coords(highlighted->getPosition());
            highlightRect.setSize(sf::Vector2f(Constants::BOARD_TILE_PIXEL_SIZE, Constants::BOARD_TILE_PIXEL_SIZE));
            highlightRect.setPosition(pos);
            window.draw(highlightRect);
        }

        std::vector<sf::Vector2f> playableCoords;
        playableCoords.reserve(playablePositions.size());
        for (const auto& p : playablePositions) {
            playableCoords.push_back(pos2Coords(p));
        }

        // draw a circular outline centered on each playable tile
        for (const auto& coord : playableCoords) {
            const float tileSizeF = static_cast<float>(Constants::BOARD_TILE_PIXEL_SIZE);
            const float radius = tileSizeF * 0.45f;
            sf::CircleShape outline(radius);
            outline.setOrigin(radius, radius);
            outline.setPosition(coord.x + tileSizeF * 0.5f, coord.y + tileSizeF * 0.5f);
            outline.setFillColor(sf::Color::Transparent);
            outline.setOutlineColor(sf::Color(0, 200, 0, 210));
            outline.setOutlineThickness(2.0f);
            window.draw(outline);
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

        if (showDice && !diceOptions.empty()) {
            // TODO : MAGIC NUMBER
            const float padding = 8.0f;
            const float size = static_cast<float>(Constants::BOARD_TILE_PIXEL_SIZE);
            const size_t n = diceOptions.size();

            float firstX = padding;
            float y = padding;
            diceOptionBounds.clear();

            for (size_t i = 0; i < n; ++i) {
                float x = firstX + i * (size + padding);
                int faceIndex = diceOptions[i] - 1;
                if (faceIndex >= 0 && static_cast<size_t>(faceIndex) < diceSprites.size() && diceSprites[faceIndex].getTexture()) {
                    const auto bounds = diceSprites[faceIndex].getLocalBounds();
                    float sx = size / bounds.width;
                    float sy = size / bounds.height;
                    diceSprites[faceIndex].setScale(sx, sy);
                    diceSprites[faceIndex].setPosition(x, y);
                    window.draw(diceSprites[faceIndex]);
                }
                diceOptionBounds.emplace_back(x, y, size, size);
            }

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            for (size_t i = 0; i < diceOptionBounds.size(); ++i) {
                const auto& r = diceOptionBounds[i];
                if (r.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    sf::RectangleShape outline(sf::Vector2f(r.width, r.height));
                    outline.setPosition(r.left, r.top);
                    outline.setFillColor(sf::Color(0,0,0,0));
                    outline.setOutlineThickness(3.0f);
                    outline.setOutlineColor(sf::Color(255, 255, 0, 160));
                    window.draw(outline);
                    break;
                }
            }
         } else {
             diceOptionBounds.clear();
         }
    }

    void SceneData::drawPreview(sf::RenderWindow& window, const std::pair<int,int>& previewPos, bool previewExists, bool legal)
    {
        if (!previewExists) {
            return;
        }

        const auto coord = pos2Coords(previewPos);
        sf::RectangleShape rect;
        rect.setSize(sf::Vector2f(Constants::BOARD_TILE_PIXEL_SIZE, Constants::BOARD_TILE_PIXEL_SIZE));
        rect.setPosition(coord);
        if (legal) {
            rect.setFillColor(sf::Color(0, 255, 0, 80)); // semi-transparent green
        } else {
            rect.setFillColor(sf::Color(255, 0, 0, 80)); // semi-transparent red
        }
        window.draw(rect);
    }

    void SceneData::updatePositions(const std::shared_ptr<state::BloodBowlGame>& game) {
        if (!game) {
            return;
        }

        for (const auto& character : game->getTeamA().getCharacters()) {
            const auto index = static_cast<size_t>(character->getId());
            if (index < Constants::MAX_PLAYERS_PER_TEAM) {
                playersSprites_TeamA.at(index).setPosition(pos2Coords(character->getPosition()));
            }
        }

        for (const auto& character : game->getTeamB().getCharacters()) {
            const auto index = static_cast<size_t>(character->getId());
            if (index < Constants::MAX_PLAYERS_PER_TEAM) {
                playersSprites_TeamB.at(index).setPosition(pos2Coords(character->getPosition()));
            }
        }

        ballSprite.setPosition(pos2Coords(game->getBallPosition()));
    }

    const std::vector<sf::FloatRect>& SceneData::getDiceOptionBounds() const {
        return diceOptionBounds;
    }

}
