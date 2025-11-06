#include "SceneData.h"
#include <filesystem>
#include <iostream>

#define TILE_SIZE 52
#define OFFSET_BEGIN_BOARD_X 50
#define OFFSET_BEGIN_BOARD_Y 50

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
        const int x = OFFSET_BEGIN_BOARD_X + (TILE_SIZE+1) * pos.first;
        const int y = OFFSET_BEGIN_BOARD_Y + (TILE_SIZE+1) * pos.second;
        return sf::Vector2f(static_cast<float>(x), static_cast<float>(y));
    }

    SceneData::SceneData(std::string colorA, std::string colorB)
    : colorA(std::move(colorA)), colorB(std::move(colorB))
    {

    }

    SceneData::~SceneData() {}

    void SceneData::init(sf::RenderWindow& window, std::shared_ptr<state::BloodBowlGame> game)
    {
        if (loadTextureFromFile("res/board.png", boardTexture)) {
            board.setTexture(boardTexture);
        } else {
            std::cerr << "Error loading res/board.png" << std::endl;
        }

        // load Characters textures
        const std::string charactersDir = "res/characters/";
        if (!std::filesystem::exists(charactersDir) && !std::filesystem::is_directory(charactersDir)) {
            std::cerr << "Error: characters directory does not exist: " << charactersDir << std::endl;
            return;
        }
        for ( const auto& character : game->getTeamA().getCharacters()) {
            const std::string texturePath = charactersDir + character->getType() + "_"+ colorA + ".png";
            const int index = character->getId();
            if (std::filesystem::exists(texturePath)) {
                loadTextureFromFile(texturePath, playersTextures_TeamA.at(index));
                playersSprites_TeamA.at(index).setTexture(playersTextures_TeamA.at(index));
                playersSprites_TeamA.at(index).setPosition(pos2Coords(character->getPosition()));
            }
        }

        for ( const auto& character : game->getTeamB().getCharacters()) {
            const std::string texturePath = charactersDir + character->getType() + "_"+ colorB + ".png";
            const int index = character->getId();
            if (std::filesystem::exists(texturePath)) {
                loadTextureFromFile(texturePath, playersTextures_TeamB.at(index));
                PlayersSprites_TeamB.at(index).setTexture(playersTextures_TeamB.at(index));
                PlayersSprites_TeamB.at(index).setPosition(pos2Coords(character->getPosition()));
            }
        }
    }

    void SceneData::draw(sf::RenderTarget& target)
    {
        // Draw board
        target.draw(board);

        // Draw players
        std::vector<sf::Sprite> playersSprites;
        playersSprites.reserve(playersSprites_TeamA.size() + PlayersSprites_TeamB.size());
        playersSprites.insert(playersSprites.end(), playersSprites_TeamA.begin(), playersSprites_TeamA.end());
        playersSprites.insert(playersSprites.end(), PlayersSprites_TeamB.begin(), PlayersSprites_TeamB.end());
        for (auto& s : playersSprites) {
            target.draw(s);
        }
    }

}
