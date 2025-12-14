#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <SFML/Graphics.hpp>
#include "../core/Config.h"

class Obstacle {
public:
    Obstacle(float x, float y, float speed);
    
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    
    bool isOffScreen() const;
    sf::FloatRect getBounds() const;
    
    // 新方法：设置颜色
    void setColor(const sf::Color& color);
    
    // 新方法：设置随机颜色
    void setRandomColor();
    
private:
    sf::CircleShape shape;  // 改为圆形
    float speed;
    float radius;
    
    // 旋转相关
    float rotationSpeed;
    float currentRotation;
};

#endif