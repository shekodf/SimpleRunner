#ifndef OBSTACLE_PARTICLE_H
#define OBSTACLE_PARTICLE_H

#include <SFML/Graphics.hpp>
#include <memory>
#include "ParticleSystem.h"

class ObstacleParticle {
public:
    // 障碍物类型（不同外观）
    enum class Type {
        Fire,      // 火焰效果
        Ice,       // 冰霜效果
        Electric,  // 电击效果
        Poison,    // 毒雾效果
        Random     // 随机类型
    };
    
    ObstacleParticle(float x, float y, float speed, Type type = Type::Random);
    ~ObstacleParticle();
    
    // 更新障碍物和粒子系统
    void update(float deltaTime);
    
    // 绘制障碍物和粒子
    void draw(sf::RenderWindow& window) const;
    
    // 检查是否离开屏幕
    bool isOffScreen() const;
    
    // 获取碰撞边界（主形状）
    sf::FloatRect getBounds() const;
    
    // 获取位置
    sf::Vector2f getPosition() const;
    
    // 获取速度
    float getSpeed() const;
    
    // 设置速度
    void setSpeed(float speed);
    
    // 设置类型
    void setType(Type type);
    
    // 触发碰撞粒子效果
    void triggerCollisionEffect();
    
    // 触发销毁粒子效果
    void triggerDestroyEffect();
    
    // 检查是否应该被移除
    bool shouldRemove() const;
    
    // 获取类型
    Type getType() const { return currentType; }
    
private:
    // 主形状（核心障碍物）
    sf::CircleShape coreShape;
    sf::RectangleShape outlineShape;
    
    // 物理属性
    sf::Vector2f position;
    float speed;
    float rotation;
    float rotationSpeed;
    float pulseScale;
    float pulseSpeed;
    float pulseTime;
    
    // 类型相关属性
    Type currentType;
    sf::Color coreColor;
    sf::Color outlineColor;
    
    // 粒子系统
    std::unique_ptr<ParticleSystem> trailSystem;
    std::unique_ptr<ParticleSystem> auraSystem;
    std::unique_ptr<ParticleSystem> collisionSystem;
    
    // 状态
    bool isActive;
    bool isDestroying;
    float destroyTimer;
    float maxDestroyTime;
    
    // 初始化函数
    void initCore();
    void initParticleSystems();
    void setupByType(Type type);
    
    // 更新函数
    void updatePosition(float deltaTime);
    void updateRotation(float deltaTime);
    void updatePulse(float deltaTime);
    void updateParticleSystems(float deltaTime);
    
    // 粒子系统配置
    void setupFireEffect();
    void setupIceEffect();
    void setupElectricEffect();
    void setupPoisonEffect();
    
    // 工具函数
    void setCoreColors(const sf::Color& core, const sf::Color& outline);
    void createDestroyParticles();
    
    // 随机数生成
    float randomFloat(float min, float max) const;
    int randomInt(int min, int max) const;
};

#endif