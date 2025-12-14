#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>
#include <functional>
#include <cmath>

class Particle {
public:
    // 生命周期状态
    enum class State {
        Active,    // 活跃状态
        Fading,    // 淡出状态
        Dead       // 死亡状态
    };

    Particle();
    
    // 初始化粒子
    void init(const sf::Vector2f& position, 
              const sf::Vector2f& velocity,
              float lifetime,
              const sf::Color& color,
              float size);
    
    // 更新粒子状态
    void update(float deltaTime);
    
    // 绘制粒子
    void draw(sf::RenderWindow& window) const;
    
    // 检查粒子是否还活着
    bool isAlive() const;
    
    // 获取状态
    State getState() const { return state; }
    
    // 获取剩余生命周期比例 (0.0 - 1.0)
    float getLifeRatio() const;
    
    // 设置自定义更新函数
    void setUpdater(std::function<void(Particle&, float)> updater);
    
    // 获取当前位置
    sf::Vector2f getPosition() const { return position; }
    
    // 设置位置
    void setPosition(const sf::Vector2f& pos) { position = pos; }
    
    // 设置速度
    void setVelocity(const sf::Vector2f& vel) { velocity = vel; }
    
    // 设置旋转
    void setRotation(float rot) { rotation = rot; }
    void setRotationSpeed(float speed) { rotationSpeed = speed; }
    
private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    sf::Color originalColor;
    float size;
    float originalSize;
    float lifetime;
    float maxLifetime;
    float rotation;
    float rotationSpeed;
    State state;
    
    // 自定义更新函数
    std::function<void(Particle&, float)> customUpdater;
    
    // 更新颜色（根据生命周期）
    void updateColor();
    
    // 更新大小（根据生命周期）
    void updateSize();
};

#endif