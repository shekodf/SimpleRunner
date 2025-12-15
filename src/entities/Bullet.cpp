#include "Bullet.h"
#include <iostream>

Bullet::Bullet(float x, float y) 
    : speed(1000.0f), active(true), destroyTimer(0.0f) { // 增加速度到1000
    
    shape.setRadius(6.0f);
    shape.setFillColor(sf::Color(255, 255, 200, 255)); // 淡黄色
    shape.setOutlineColor(sf::Color(255, 255, 100, 255));
    shape.setOutlineThickness(2.0f);
    shape.setOrigin(6.0f, 6.0f);
    shape.setPosition(x, y);
    
    originalColor = shape.getFillColor();
}

void Bullet::update(float deltaTime) {
    if (!active) {
        destroyTimer += deltaTime;
        
        // 销毁时的闪烁效果
        if (destroyTimer < 0.3f) {
            float alpha = 255.0f * (1.0f - destroyTimer / 0.3f);
            sf::Color newColor = originalColor;
            newColor.a = static_cast<sf::Uint8>(alpha);
            shape.setFillColor(newColor);
            shape.setOutlineColor(sf::Color(255, 255, 100, static_cast<sf::Uint8>(alpha)));
        }
        return;
    }
    
    // 向上移动（负Y方向）
    shape.move(0, -speed * deltaTime);
}

void Bullet::draw(sf::RenderWindow& window) const {
    if (active || destroyTimer < 0.3f) {
        window.draw(shape);
    }
}

bool Bullet::isOffScreen() const {
    // 检查是否完全离开屏幕上方
    return shape.getPosition().y + shape.getRadius() < 0;
}

sf::FloatRect Bullet::getBounds() const {
    return shape.getGlobalBounds();
}

void Bullet::triggerDestroyEffect() {
    active = false;
    destroyTimer = 0.0f;
}