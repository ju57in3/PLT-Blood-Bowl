#include "ResourceManager.h"
#include <stdexcept>
#include <iostream>

namespace screen {
    ResourceManager::ResourceManager(const std::string &basePath) {
        this->basePath = basePath;
    }

    ResourceManager::~ResourceManager() = default;

    const sf::Texture &ResourceManager::getTexture(const std::string &key) {
        auto it = textures.find(key);
        if (it != textures.end()) return *it->second;
        auto tex = std::make_shared<sf::Texture>();
        std::string path = basePath + key;
        if (!tex->loadFromFile(path)) {
            std::cerr << "ResourceManager: failed to load texture '" << path << "'\n";
            throw std::runtime_error("Failed to load texture: " + path);
        }
        textures[key] = tex;
        return *tex;
    }

    const sf::Font &ResourceManager::getFont(const std::string &key) {
        auto it = fonts.find(key);
        if (it != fonts.end()) return *it->second;
        auto f = std::make_shared<sf::Font>();
        std::string path = basePath + key;
        if (!f->loadFromFile(path)) {
            std::cerr << "ResourceManager: failed to load font '" << path << "'\n";
            throw std::runtime_error("Failed to load font: " + path);
        }
        fonts[key] = f;
        return *f;
    }
} // namespace screen
