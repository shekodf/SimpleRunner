#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>

class ResourceManager {
public:
    static ResourceManager& getInstance();
    
    // 禁止复制
    ResourceManager(const ResourceManager&) = delete;
    void operator=(const ResourceManager&) = delete;
    
    // 资源加载接口
    sf::Texture& getTexture(const std::string& path);
    sf::Font& getFont(const std::string& path);
    sf::SoundBuffer& getSoundBuffer(const std::string& path);
    
    // 清理资源
    void clearTextures();
    void clearFonts();
    void clearSoundBuffers();
    
private:
    ResourceManager() = default;
    
    std::map<std::string, std::unique_ptr<sf::Texture>> textures;
    std::map<std::string, std::unique_ptr<sf::Font>> fonts;
    std::map<std::string, std::unique_ptr<sf::SoundBuffer>> soundBuffers;
};

#endif