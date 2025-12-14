#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include "../core/Config.h"

class Player {
public:
    Player();
    
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    
    void reset();
    
    // 获取碰撞边界
    sf::FloatRect getBounds() const;
    
private:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    
    void handleInput(float deltaTime);
    void applyConstraints();
};

#endif