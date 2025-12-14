#include "Obstacle.h"
#include <random>
#include <cmath>

Obstacle::Obstacle(float x, float y, float speed) 
    : speed(speed), radius(Config::OBSTACLE_WIDTH / 2.0f),
      rotationSpeed(0), currentRotation(0) {
    
    // 随机生成旋转速度
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> rotationDist(-180.0f, 180.0f);
    rotationSpeed = rotationDist(gen);
    
    // 创建圆形
    shape.setRadius(radius);
    shape.setFillColor(Config::OBSTACLE_COLOR);
    shape.setOutlineColor(sf::Color(255, 255, 255, 128)); // 半透明白色边框
    shape.setOutlineThickness(2.0f);
    shape.setPosition(x, y);
    shape.setOrigin(radius, radius); // 设置中心点为原点，方便旋转
}

void Obstacle::update(float deltaTime) {
    // 向下移动
    shape.move(0, speed * deltaTime);
    
    // 旋转
    currentRotation += rotationSpeed * deltaTime;
    shape.setRotation(currentRotation);
}

void Obstacle::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

bool Obstacle::isOffScreen() const {
    return shape.getPosition().y > Config::WINDOW_HEIGHT + radius * 2;
}

sf::FloatRect Obstacle::getBounds() const {
    return shape.getGlobalBounds();
}

void Obstacle::setColor(const sf::Color& color) {
    shape.setFillColor(color);
}

void Obstacle::setRandomColor() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> colorDist(50, 255);
    
    sf::Color randomColor(
        colorDist(gen),  // R
        colorDist(gen),  // G
        colorDist(gen),  // B
        255              // A
    );
    
    shape.setFillColor(randomColor);
}