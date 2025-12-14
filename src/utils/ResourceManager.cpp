#include "ResourceManager.h"
#include <iostream>

ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

sf::Texture& ResourceManager::getTexture(const std::string& path) {
    auto it = textures.find(path);
    if (it != textures.end()) {
        return *it->second;
    }
    
    // 加载新纹理
    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(path)) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        // 返回一个默认纹理（这里简单处理，实际项目应该创建错误纹理）
        return *texture;
    }
    
    auto& ref = *texture;
    textures[path] = std::move(texture);
    return ref;
}

sf::Font& ResourceManager::getFont(const std::string& path) {
    auto it = fonts.find(path);
    if (it != fonts.end()) {
        return *it->second;
    }
    
    // 加载新字体
    auto font = std::make_unique<sf::Font>();
    if (!font->loadFromFile(path)) {
        std::cerr << "Failed to load font: " << path << std::endl;
        return *font;
    }
    
    auto& ref = *font;
    fonts[path] = std::move(font);
    return ref;
}

sf::SoundBuffer& ResourceManager::getSoundBuffer(const std::string& path) {
    auto it = soundBuffers.find(path);
    if (it != soundBuffers.end()) {
        return *it->second;
    }
    
    // 加载新音效
    auto buffer = std::make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(path)) {
        std::cerr << "Failed to load sound: " << path << std::endl;
        return *buffer;
    }
    
    auto& ref = *buffer;
    soundBuffers[path] = std::move(buffer);
    return ref;
}

void ResourceManager::clearTextures() {
    textures.clear();
}

void ResourceManager::clearFonts() {
    fonts.clear();
}

void ResourceManager::clearSoundBuffers() {
    soundBuffers.clear();
}