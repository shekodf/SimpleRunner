#include "Particle.h"
#include <iostream>

Particle::Particle()
    : size(5.0f), lifetime(0.0f), maxLifetime(1.0f),
      rotation(0.0f), rotationSpeed(0.0f), state(State::Dead) {
}

void Particle::init(const sf::Vector2f& position,
                   const sf::Vector2f& velocity,
                   float lifetime,
                   const sf::Color& color,
                   float size) {
    this->position = position;
    this->velocity = velocity;
    this->color = color;
    this->originalColor = color;
    this->size = size;
    this->originalSize = size;
    this->lifetime = lifetime;
    this->maxLifetime = lifetime;
    this->rotation = 0.0f;
    this->rotationSpeed = 0.0f;
    this->state = State::Active;
}

void Particle::update(float deltaTime) {
    if (!isAlive()) return;
    
    // 更新生命周期
    lifetime -= deltaTime;
    
    // 检查状态
    if (lifetime <= 0) {
        state = State::Dead;
        return;
    }
    
    // 如果剩余时间少于0.3秒，进入淡出状态
    if (lifetime < 0.3f && state == State::Active) {
        state = State::Fading;
    }
    
    // 如果有自定义更新器，使用它
    if (customUpdater) {
        customUpdater(*this, deltaTime);
        return;
    }
    
    // 默认物理更新
    position += velocity * deltaTime;
    
    // 更新旋转
    rotation += rotationSpeed * deltaTime;
    
    // 更新颜色和大小
    updateColor();
    updateSize();
}

void Particle::draw(sf::RenderWindow& window) const {
    if (!isAlive()) return;
    
    // 创建圆形粒子
    sf::CircleShape particleShape(size);
    
    // 根据状态设置颜色
    sf::Color drawColor = color;
    if (state == State::Fading) {
        // 淡出效果：降低透明度
        float alpha = (lifetime / 0.3f) * 255.0f;
        drawColor.a = static_cast<sf::Uint8>(alpha);
    }
    
    particleShape.setFillColor(drawColor);
    particleShape.setPosition(position);
    particleShape.setOrigin(size, size); // 中心点作为原点
    particleShape.setRotation(rotation);
    
    window.draw(particleShape);
}

bool Particle::isAlive() const {
    return state != State::Dead;
}

float Particle::getLifeRatio() const {
    if (maxLifetime <= 0) return 0.0f;
    return lifetime / maxLifetime;
}

void Particle::setUpdater(std::function<void(Particle&, float)> updater) {
    customUpdater = updater;
}

void Particle::updateColor() {
    float lifeRatio = getLifeRatio();
    
    // 根据生命周期改变颜色
    color.r = static_cast<sf::Uint8>(originalColor.r * lifeRatio);
    color.g = static_cast<sf::Uint8>(originalColor.g * lifeRatio);
    color.b = static_cast<sf::Uint8>(originalColor.b * lifeRatio);
    color.a = static_cast<sf::Uint8>(originalColor.a * lifeRatio);
}

void Particle::updateSize() {
    float lifeRatio = getLifeRatio();
    size = originalSize * (0.5f + 0.5f * lifeRatio); // 逐渐缩小
}