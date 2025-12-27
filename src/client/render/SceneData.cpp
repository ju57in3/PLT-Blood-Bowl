#include "SceneData.h"
#include "utility/Constants.h"
#include <filesystem>
#include <iostream>
#include <cmath>
#include "../../shared/state/CharacterStatus.h"

using namespace utility;

namespace render{

    const char* statusToFile(state::CharacterStatus status) {
        switch (status) {
            case state::stunned:     return "human_stu";
            case state::ko:          return "human_ko";
            //case state::played:      return "human_up";
            case state::bench:       return "human_up";
            default:          return "unknown";
        }
    }

    std::string buildCharacterTexturePath(const std::string& dir,
                                      const state::Character& character,
                                      const std::string& color)
    {
        std::string base = dir + character.getType() + "_" + color + ".png";

        std::string alt = dir + statusToFile(character.getStatus()) + ".png";

        if (std::filesystem::exists(alt))
            return alt;

        return base;
    }

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
        if (!defaultFont.loadFromFile("../res/fonts/bloodbowl_italic.ttf")) {
            std::cout << "Fonts not loaded\n";
        }
    }

    SceneData::~SceneData() = default;

    bool SceneData::isBoardLoaded() const {
        return boardTexture.getSize().x > 0 && boardTexture.getSize().y > 0;
    }

    void SceneData::init(const std::shared_ptr<state::BloodBowlGame>& game)
    {
        this->game = game;

        playersTextures_TeamA.resize(Constants::MAX_PLAYERS_PER_TEAM);
        playersTextures_TeamB.resize(Constants::MAX_PLAYERS_PER_TEAM);
        playersSprites_TeamA.resize(Constants::MAX_PLAYERS_PER_TEAM);
        playersSprites_TeamB.resize(Constants::MAX_PLAYERS_PER_TEAM);

        if (loadTextureFromFile("../res/board.png", boardTexture)) {
            board.setTexture(boardTexture);
        } else {
            std::cerr << "Error loading res/board.png" << std::endl;
        }

        const std::string charactersDir = "../res/characters/TODO/";
        if (!std::filesystem::exists(charactersDir) || !std::filesystem::is_directory(charactersDir)) {
            std::cerr << "Error: characters directory does not exist: " << charactersDir << std::endl;
        }

        for (const auto& character : game->getTeamA().getCharacters()) {
            const auto index = static_cast<size_t>(character->getId());
            if (index >= Constants::MAX_PLAYERS_PER_TEAM) continue;

            const std::string texturePath =
                buildCharacterTexturePath(charactersDir, *character, colorA);

            if (loadTextureFromFile(texturePath, playersTextures_TeamA.at(index))) {
                playersSprites_TeamA.at(index).setTexture(playersTextures_TeamA.at(index));
                playersSprites_TeamA.at(index).setPosition(pos2Coords(character->getPosition()));
            } else {
                std::cerr << "Error loading " << texturePath << std::endl;
            }
        }

        for (const auto& character : game->getTeamB().getCharacters()) {
            const auto index = static_cast<size_t>(character->getId());
            if (index >= Constants::MAX_PLAYERS_PER_TEAM) continue;

            const std::string texturePath =
                buildCharacterTexturePath(charactersDir, *character, colorB);

            if (loadTextureFromFile(texturePath, playersTextures_TeamB.at(index))) {
                playersSprites_TeamB.at(index).setTexture(playersTextures_TeamB.at(index));
                playersSprites_TeamB.at(index).setPosition(pos2Coords(character->getPosition()));
            } else {
                std::cerr << "Error loading " << texturePath << std::endl;
            }
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

    void SceneData::draw(sf::RenderWindow& window, const std::shared_ptr<state::Character>& highlighted, const std::pair<int,int>& previewPos, bool previewExists, bool legal, const std::vector<int>& diceOptions, bool showDice, const std::vector<std::pair<int,int>>& playablePositions,const std::vector<std::pair<int,int>>& movePath, const std::string& stateName, int currentTeamId)
    {
        window.draw(board);

        if (previewExists) {
            drawPreview(window, previewPos, previewExists, legal);
        }

        if (highlighted) {
            sf::RectangleShape highlightRect;
            highlightRect.setFillColor(sf::Color(255, 255, 0, 80));
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

        const float tileSizeF = static_cast<float>(Constants::BOARD_TILE_PIXEL_SIZE);

        for (const auto& coord : playableCoords) {
            const float radius = tileSizeF * 0.45f;
            sf::CircleShape outline(radius);
            outline.setOrigin(radius, radius);
            outline.setPosition(coord.x + tileSizeF * 0.5f, coord.y + tileSizeF * 0.5f);
            outline.setFillColor(sf::Color::Transparent);
            outline.setOutlineColor(sf::Color(0, 200, 0, 210));
            outline.setOutlineThickness(2.0f);
            window.draw(outline);
        }




        if (!movePath.empty()) {
            sf::VertexArray va(sf::LinesStrip);
            for (const auto& p : movePath) {
                auto c = pos2Coords(p);
                // center
                c.x += Constants::BOARD_TILE_PIXEL_SIZE * 0.5f;
                c.y += Constants::BOARD_TILE_PIXEL_SIZE * 0.5f;
                va.append(sf::Vertex(c, sf::Color::Yellow));
            }
            if (va.getVertexCount() > 1) window.draw(va);
            for (const auto& p : movePath) {
                auto c = pos2Coords(p);
                c.x += Constants::BOARD_TILE_PIXEL_SIZE * 0.5f;
                c.y += Constants::BOARD_TILE_PIXEL_SIZE * 0.5f;
                sf::CircleShape step(6.0f);
                step.setOrigin(6.0f, 6.0f);
                step.setPosition(c);
                step.setFillColor(sf::Color(255, 200, 0, 200));
                window.draw(step);
            }
        }

        for (auto& s : playersSprites_TeamA) {
            window.draw(s);
        }
        for (auto& s : playersSprites_TeamB) {
            window.draw(s);
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

        if (!defaultFont.getInfo().family.empty()) {
            sf::Text header(stateName + " - Team: " + (currentTeamId>=0?std::to_string(currentTeamId):std::string("-")), defaultFont, 16);
            header.setFillColor(sf::Color::White);
            header.setPosition(8.0f, 4.0f);
            window.draw(header);
        }


        sf::Vector2f ballCoords = pos2Coords(game->getBallPosition());

        sf::ConvexShape star;
        const int points = 10;
        star.setPointCount(points);

        float outerRadius = tileSizeF * 0.15f;
        float innerRadius = outerRadius * 0.45f;
        const float twoPi = 2.0f * static_cast<float>(M_PI);
        const float startAngle = -static_cast<float>(M_PI) / 2.0f; // point up

        for (int i = 0; i < points; ++i) {
            float angle = startAngle + (twoPi * i) / points;
            float r = (i % 2 == 0) ? outerRadius : innerRadius;
            float x = std::cos(angle) * r;
            float y = std::sin(angle) * r;
            star.setPoint(static_cast<size_t>(i), sf::Vector2f(x, y));
        }
        star.setOrigin(0.f, 0.f);
        star.setPosition(ballCoords.x + tileSizeF * 0.5f, ballCoords.y + tileSizeF * 0.5f);
        star.setFillColor(sf::Color::Transparent);
        star.setOutlineColor(sf::Color(255, 210, 0, 210));
        star.setOutlineThickness(2.0f);
        window.draw(star);

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
            rect.setFillColor(sf::Color(0, 255, 0, 80));
        } else {
            rect.setFillColor(sf::Color(255, 0, 0, 80));
        }
        window.draw(rect);
    }

    void SceneData::updatePositions(const std::shared_ptr<state::BloodBowlGame>& game) {
        if (!game) {
            return;
        }

        const std::string charactersDir = "../res/characters/";

        for (const auto& character : game->getTeamA().getCharacters()) {

            const size_t index = static_cast<size_t>(character->getId());
            if (index >= Constants::MAX_PLAYERS_PER_TEAM) continue;

            const std::string texturePath =
                buildCharacterTexturePath(charactersDir, *character, colorA);

            if (loadTextureFromFile(texturePath, playersTextures_TeamA.at(index))) {
                auto& sprite = playersSprites_TeamA.at(index);
                sprite.setTexture(playersTextures_TeamA.at(index));
                sprite.setPosition(pos2Coords(character->getPosition()));
            } else {
                std::cerr << "[Render] Error loading " << texturePath << std::endl;
            }
        }

        for (const auto& character : game->getTeamB().getCharacters()) {

            const size_t index = static_cast<size_t>(character->getId());
            if (index >= Constants::MAX_PLAYERS_PER_TEAM) continue;

            const std::string texturePath =
                buildCharacterTexturePath(charactersDir, *character, colorB);

            if (loadTextureFromFile(texturePath, playersTextures_TeamB.at(index))) {
                auto& sprite = playersSprites_TeamB.at(index);
                sprite.setTexture(playersTextures_TeamB.at(index));
                sprite.setPosition(pos2Coords(character->getPosition()));
            } else {
                std::cerr << "[Render] Error loading " << texturePath << std::endl;
            }
        }
    }

    const std::vector<sf::FloatRect>& SceneData::getDiceOptionBounds() const {
        return diceOptionBounds;
    }

}
