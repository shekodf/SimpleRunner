# 高级粒子效果障碍物 - 详细步骤

我将为你提供完整、详细的粒子效果障碍物实现步骤。这个实现将创建一个视觉效果丰富的障碍物，带有拖尾粒子、旋转动画和颜色变化。

## 一、项目结构调整

首先，让我们创建更清晰的目录结构：

```
SimpleRunner/
├── src/
│   ├── entities/
│   │   ├── ObstacleParticle.h     # 粒子障碍物类
│   │   ├── ObstacleParticle.cpp   # 实现
│   │   ├── Particle.h            # 单个粒子类
│   │   ├── Particle.cpp          # 粒子实现
│   │   └── Obstacle.h           # 基础障碍物（可选保留）
│   └── core/
│       └── Config.h             # 添加粒子配置
```

## 二、创建粒子系统基础类

### 1. `src/entities/Particle.h`
```cpp
#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>
#include <functional>

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
    
private:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
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
```

### 2. `src/entities/Particle.cpp`
```cpp
#include "Particle.h"
#include <cmath>
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
    this->acceleration = sf::Vector2f(0, 0);
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
    velocity += acceleration * deltaTime;
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
```

## 三、创建粒子系统管理器

### 3. `src/entities/ParticleSystem.h`
```cpp
#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <functional>
#include <random>
#include "Particle.h"

class ParticleSystem {
public:
    // 粒子发射器配置
    struct EmitterConfig {
        sf::Vector2f position;      // 发射位置
        sf::Vector2f positionVariance; // 位置变化范围
        sf::Vector2f velocity;      // 基础速度
        sf::Vector2f velocityVariance; // 速度变化范围
        sf::Color startColor;       // 起始颜色
        sf::Color endColor;         // 结束颜色
        float minSize;              // 最小大小
        float maxSize;              // 最大大小
        float minLifetime;          // 最小生命周期
        float maxLifetime;          // 最大生命周期
        float emissionRate;         // 每秒发射粒子数
        int maxParticles;           // 最大粒子数
        bool continuous;            // 是否持续发射
    };
    
    ParticleSystem();
    ~ParticleSystem();
    
    // 设置发射器配置
    void setEmitter(const EmitterConfig& config);
    
    // 发射一批粒子
    void burst(int count);
    
    // 开始持续发射
    void start();
    
    // 停止发射
    void stop();
    
    // 更新粒子系统
    void update(float deltaTime);
    
    // 绘制所有粒子
    void draw(sf::RenderWindow& window) const;
    
    // 清除所有粒子
    void clear();
    
    // 检查是否还有活跃粒子
    bool hasActiveParticles() const;
    
    // 获取活跃粒子数量
    int getActiveParticleCount() const;
    
    // 设置自定义粒子更新函数
    void setParticleUpdater(std::function<void(Particle&, float)> updater);
    
    // 设置重力等外力
    void setGravity(const sf::Vector2f& gravity);
    
    // 设置风等水平力
    void setWind(const sf::Vector2f& wind);
    
private:
    // 粒子池
    std::vector<std::unique_ptr<Particle>> particles;
    std::vector<std::unique_ptr<Particle>> freeParticles;
    
    // 发射器配置
    EmitterConfig emitterConfig;
    
    // 发射器状态
    bool isEmitting;
    float emissionTimer;
    
    // 随机数生成器
    std::mt19937 randomEngine;
    
    // 外部力
    sf::Vector2f gravity;
    sf::Vector2f wind;
    
    // 自定义更新器
    std::function<void(Particle&, float)> particleUpdater;
    
    // 创建新粒子
    std::unique_ptr<Particle> createParticle();
    
    // 回收粒子
    void recycleParticle(Particle* particle);
    
    // 随机浮点数生成器
    float randomFloat(float min, float max);
    
    // 随机整数生成器
    int randomInt(int min, int max);
    
    // 随机颜色生成器
    sf::Color randomColor(const sf::Color& min, const sf::Color& max);
    
    // 初始化粒子池
    void initParticlePool();
};

#endif
```

### 4. `src/entities/ParticleSystem.cpp`
```cpp
#include "ParticleSystem.h"
#include <cmath>
#include <algorithm>
#include <iostream>

ParticleSystem::ParticleSystem()
    : isEmitting(false), emissionTimer(0.0f),
      gravity(0, 98.0f), wind(0, 0) {
    
    // 初始化随机数生成器
    std::random_device rd;
    randomEngine.seed(rd());
    
    // 默认发射器配置
    emitterConfig = {
        sf::Vector2f(0, 0),     // position
        sf::Vector2f(10, 10),   // positionVariance
        sf::Vector2f(0, 100),   // velocity
        sf::Vector2f(50, 50),   // velocityVariance
        sf::Color(255, 100, 50, 255),  // startColor
        sf::Color(255, 200, 100, 0),   // endColor
        2.0f,                   // minSize
        8.0f,                   // maxSize
        0.5f,                   // minLifetime
        2.0f,                   // maxLifetime
        20.0f,                  // emissionRate
        500,                    // maxParticles
        true                    // continuous
    };
    
    initParticlePool();
}

ParticleSystem::~ParticleSystem() {
    clear();
}

void ParticleSystem::setEmitter(const EmitterConfig& config) {
    emitterConfig = config;
    // 如果最大粒子数改变，重新初始化粒子池
    if (particles.size() + freeParticles.size() < config.maxParticles) {
        initParticlePool();
    }
}

void ParticleSystem::burst(int count) {
    for (int i = 0; i < count && !freeParticles.empty(); i++) {
        auto particle = std::move(freeParticles.back());
        freeParticles.pop_back();
        
        // 随机位置
        float posX = emitterConfig.position.x + 
                     randomFloat(-emitterConfig.positionVariance.x, 
                                 emitterConfig.positionVariance.x);
        float posY = emitterConfig.position.y + 
                     randomFloat(-emitterConfig.positionVariance.y, 
                                 emitterConfig.positionVariance.y);
        
        // 随机速度
        float velX = emitterConfig.velocity.x + 
                     randomFloat(-emitterConfig.velocityVariance.x, 
                                 emitterConfig.velocityVariance.x);
        float velY = emitterConfig.velocity.y + 
                     randomFloat(-emitterConfig.velocityVariance.y, 
                                 emitterConfig.velocityVariance.y);
        
        // 随机生命周期
        float lifetime = randomFloat(emitterConfig.minLifetime, 
                                     emitterConfig.maxLifetime);
        
        // 随机大小
        float size = randomFloat(emitterConfig.minSize, 
                                 emitterConfig.maxSize);
        
        // 随机颜色（在起始颜色和结束颜色之间）
        sf::Color color = randomColor(emitterConfig.startColor, 
                                      emitterConfig.endColor);
        
        particle->init(sf::Vector2f(posX, posY),
                      sf::Vector2f(velX, velY),
                      lifetime,
                      color,
                      size);
        
        // 设置自定义更新器
        if (particleUpdater) {
            particle->setUpdater(particleUpdater);
        }
        
        particles.push_back(std::move(particle));
    }
}

void ParticleSystem::start() {
    isEmitting = true;
}

void ParticleSystem::stop() {
    isEmitting = false;
}

void ParticleSystem::update(float deltaTime) {
    // 处理持续发射
    if (isEmitting && emitterConfig.continuous) {
        emissionTimer += deltaTime;
        float timePerParticle = 1.0f / emitterConfig.emissionRate;
        
        while (emissionTimer >= timePerParticle && 
               particles.size() < emitterConfig.maxParticles) {
            burst(1);
            emissionTimer -= timePerParticle;
        }
    }
    
    // 更新所有活跃粒子
    for (auto it = particles.begin(); it != particles.end(); ) {
        auto& particle = *it;
        
        // 应用外部力
        auto velocity = sf::Vector2f(0, 0); // 需要从粒子获取，这里简化
        
        // 更新粒子
        particle->update(deltaTime);
        
        // 如果粒子死亡，回收它
        if (!particle->isAlive()) {
            recycleParticle(particle.get());
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

void ParticleSystem::draw(sf::RenderWindow& window) const {
    // 按深度排序（y坐标大的先绘制）
    std::vector<const Particle*> sortedParticles;
    for (const auto& particle : particles) {
        sortedParticles.push_back(particle.get());
    }
    
    std::sort(sortedParticles.begin(), sortedParticles.end(),
              [](const Particle* a, const Particle* b) {
                  return a->getPosition().y < b->getPosition().y;
              });
    
    // 绘制粒子
    for (const auto& particle : sortedParticles) {
        particle->draw(window);
    }
}

void ParticleSystem::clear() {
    particles.clear();
    freeParticles.clear();
}

bool ParticleSystem::hasActiveParticles() const {
    return !particles.empty();
}

int ParticleSystem::getActiveParticleCount() const {
    return static_cast<int>(particles.size());
}

void ParticleSystem::setParticleUpdater(std::function<void(Particle&, float)> updater) {
    particleUpdater = updater;
}

void ParticleSystem::setGravity(const sf::Vector2f& g) {
    gravity = g;
}

void ParticleSystem::setWind(const sf::Vector2f& w) {
    wind = w;
}

std::unique_ptr<Particle> ParticleSystem::createParticle() {
    return std::make_unique<Particle>();
}

void ParticleSystem::recycleParticle(Particle* particle) {
    // 重置粒子状态
    // 注意：这里只是简单地将指针移回空闲列表
    // 实际使用时需要更复杂的池管理
    freeParticles.push_back(std::unique_ptr<Particle>(particle));
}

float ParticleSystem::randomFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(randomEngine);
}

int ParticleSystem::randomInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(randomEngine);
}

sf::Color ParticleSystem::randomColor(const sf::Color& min, const sf::Color& max) {
    return sf::Color(
        randomInt(min.r, max.r),
        randomInt(min.g, max.g),
        randomInt(min.b, max.b),
        randomInt(min.a, max.a)
    );
}

void ParticleSystem::initParticlePool() {
    int totalParticles = emitterConfig.maxParticles;
    
    // 清空现有粒子
    particles.clear();
    freeParticles.clear();
    
    // 预创建粒子
    for (int i = 0; i < totalParticles; i++) {
        freeParticles.push_back(createParticle());
    }
}
```

## 四、创建高级粒子障碍物

### 5. `src/entities/ObstacleParticle.h`
```cpp
#ifndef OBSTACLE_PARTICLE_H
#define OBSTACLE_PARTICLE_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
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
    
    // 设置位置
    void setPosition(float x, float y);
    
    // 获取位置
    sf::Vector2f getPosition() const;
    
    // 设置速度
    void setSpeed(float speed);
    
    // 获取速度
    float getSpeed() const;
    
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
};

#endif
```

### 6. `src/entities/ObstacleParticle.cpp` (第一部分)
```cpp
#include "ObstacleParticle.h"
#include <random>
#include <cmath>
#include <iostream>

ObstacleParticle::ObstacleParticle(float x, float y, float speed, Type type)
    : position(x, y), speed(speed), rotation(0.0f), rotationSpeed(0.0f),
      pulseScale(1.0f), pulseSpeed(2.0f), pulseTime(0.0f),
      isActive(true), isDestroying(false), destroyTimer(0.0f), maxDestroyTime(1.0f) {
    
    // 随机数生成器
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    // 随机旋转速度
    std::uniform_real_distribution<float> rotationDist(-180.0f, 180.0f);
    rotationSpeed = rotationDist(gen);
    
    // 随机脉冲速度
    std::uniform_real_distribution<float> pulseDist(1.0f, 3.0f);
    pulseSpeed = pulseDist(gen);
    
    // 确定类型
    if (type == Type::Random) {
        std::uniform_int_distribution<int> typeDist(0, 3);
        int typeIndex = typeDist(gen);
        switch (typeIndex) {
            case 0: currentType = Type::Fire; break;
            case 1: currentType = Type::Ice; break;
            case 2: currentType = Type::Electric; break;
            case 3: currentType = Type::Poison; break;
            default: currentType = Type::Fire; break;
        }
    } else {
        currentType = type;
    }
    
    // 初始化
    initCore();
    initParticleSystems();
    setupByType(currentType);
}

ObstacleParticle::~ObstacleParticle() {
    // 智能指针自动清理
}

void ObstacleParticle::initCore() {
    // 核心形状（圆形）
    float coreRadius = 15.0f;
    coreShape.setRadius(coreRadius);
    coreShape.setOrigin(coreRadius, coreRadius);
    coreShape.setPosition(position);
    
    // 外框形状（方形，用于旋转效果）
    float outlineSize = 30.0f;
    outlineShape.setSize(sf::Vector2f(outlineSize, outlineSize));
    outlineShape.setOrigin(outlineSize / 2, outlineSize / 2);
    outlineShape.setPosition(position);
    outlineShape.setFillColor(sf::Color::Transparent);
    outlineShape.setOutlineThickness(3.0f);
}

void ObstacleParticle::initParticleSystems() {
    // 轨迹粒子系统（拖尾效果）
    trailSystem = std::make_unique<ParticleSystem>();
    
    // 光环粒子系统（围绕核心的粒子）
    auraSystem = std::make_unique<ParticleSystem>();
    
    // 碰撞粒子系统（碰撞时触发的效果）
    collisionSystem = std::make_unique<ParticleSystem>();
}

void ObstacleParticle::setupByType(Type type) {
    switch (type) {
        case Type::Fire:
            setupFireEffect();
            break;
        case Type::Ice:
            setupIceEffect();
            break;
        case Type::Electric:
            setupElectricEffect();
            break;
        case Type::Poison:
            setupPoisonEffect();
            break;
        default:
            setupFireEffect();
            break;
    }
}
```

### 7. `src/entities/ObstacleParticle.cpp` (第二部分 - 效果设置)
```cpp
void ObstacleParticle::setupFireEffect() {
    // 核心颜色：红色到橙色
    setCoreColors(sf::Color(255, 100, 50, 200), sf::Color(255, 200, 100, 100));
    
    // 轨迹粒子：火焰拖尾
    ParticleSystem::EmitterConfig trailConfig;
    trailConfig.position = position;
    trailConfig.positionVariance = sf::Vector2f(5, 5);
    trailConfig.velocity = sf::Vector2f(0, -speed * 0.5f); // 向上飘散
    trailConfig.velocityVariance = sf::Vector2f(20, 10);
    trailConfig.startColor = sf::Color(255, 150, 50, 255);
    trailConfig.endColor = sf::Color(255, 50, 0, 0);
    trailConfig.minSize = 3.0f;
    trailConfig.maxSize = 8.0f;
    trailConfig.minLifetime = 0.3f;
    trailConfig.maxLifetime = 0.8f;
    trailConfig.emissionRate = 30.0f;
    trailConfig.maxParticles = 100;
    trailConfig.continuous = true;
    
    trailSystem->setEmitter(trailConfig);
    trailSystem->start();
    
    // 光环粒子：火焰边缘
    ParticleSystem::EmitterConfig auraConfig;
    auraConfig.position = position;
    auraConfig.positionVariance = sf::Vector2f(25, 25);
    auraConfig.velocity = sf::Vector2f(0, 0);
    auraConfig.velocityVariance = sf::Vector2f(10, 10);
    auraConfig.startColor = sf::Color(255, 200, 100, 100);
    auraConfig.endColor = sf::Color(255, 100, 0, 0);
    auraConfig.minSize = 1.0f;
    auraConfig.maxSize = 4.0f;
    auraConfig.minLifetime = 0.5f;
    auraConfig.maxLifetime = 1.0f;
    auraConfig.emissionRate = 50.0f;
    auraConfig.maxParticles = 150;
    auraConfig.continuous = true;
    
    auraSystem->setEmitter(auraConfig);
    auraSystem->start();
    
    // 设置火焰粒子更新器
    trailSystem->setParticleUpdater([](Particle& p, float dt) {
        // 火焰向上飘，轻微左右摆动
        auto pos = p.getPosition();
        pos.x += std::sin(p.getLifeRatio() * 10) * 20 * dt;
        pos.y -= 100 * dt;
        p.setPosition(pos);
    });
}

void ObstacleParticle::setupIceEffect() {
    // 核心颜色：蓝色到青色
    setCoreColors(sf::Color(100, 200, 255, 200), sf::Color(150, 230, 255, 100));
    
    // 轨迹粒子：冰晶拖尾
    ParticleSystem::EmitterConfig trailConfig;
    trailConfig.position = position;
    trailConfig.positionVariance = sf::Vector2f(3, 3);
    trailConfig.velocity = sf::Vector2f(0, -10);
    trailConfig.velocityVariance = sf::Vector2f(5, 5);
    trailConfig.startColor = sf::Color(150, 230, 255, 200);
    trailConfig.endColor = sf::Color(100, 180, 255, 0);
    trailConfig.minSize = 2.0f;
    trailConfig.maxSize = 6.0f;
    trailConfig.minLifetime = 0.5f;
    trailConfig.maxLifetime = 1.5f;
    trailConfig.emissionRate = 20.0f;
    trailConfig.maxParticles = 80;
    trailConfig.continuous = true;
    
    trailSystem->setEmitter(trailConfig);
    trailSystem->start();
}

void ObstacleParticle::setupElectricEffect() {
    // 核心颜色：紫色到蓝色
    setCoreColors(sf::Color(150, 100, 255, 200), sf::Color(200, 150, 255, 100));
    
    // 光环粒子：电弧效果
    ParticleSystem::EmitterConfig auraConfig;
    auraConfig.position = position;
    auraConfig.positionVariance = sf::Vector2f(20, 20);
    auraConfig.velocity = sf::Vector2f(0, 0);
    auraConfig.velocityVariance = sf::Vector2f(30, 30);
    auraConfig.startColor = sf::Color(200, 150, 255, 150);
    auraConfig.endColor = sf::Color(100, 50, 200, 0);
    auraConfig.minSize = 1.0f;
    auraConfig.maxSize = 3.0f;
    auraConfig.minLifetime = 0.2f;
    auraConfig.maxLifetime = 0.5f;
    auraConfig.emissionRate = 100.0f;
    auraConfig.maxParticles = 200;
    auraConfig.continuous = true;
    
    auraSystem->setEmitter(auraConfig);
    auraSystem->start();
}

void ObstacleParticle::setupPoisonEffect() {
    // 核心颜色：绿色到黄色
    setCoreColors(sf::Color(100, 255, 100, 200), sf::Color(200, 255, 100, 100));
    
    // 轨迹粒子：毒雾效果
    ParticleSystem::EmitterConfig trailConfig;
    trailConfig.position = position;
    trailConfig.positionVariance = sf::Vector2f(8, 8);
    trailConfig.velocity = sf::Vector2f(0, -5);
    trailConfig.velocityVariance = sf::Vector2f(15, 5);
    trailConfig.startColor = sf::Color(100, 255, 100, 150);
    trailConfig.endColor = sf::Color(50, 150, 50, 0);
    trailConfig.minSize = 4.0f;
    trailConfig.maxSize = 10.0f;
    trailConfig.minLifetime = 0.8f;
    trailConfig.maxLifetime = 1.5f;
    trailConfig.emissionRate = 15.0f;
    trailConfig.maxParticles = 60;
    trailConfig.continuous = true;
    
    trailSystem->setEmitter(trailConfig);
    trailSystem->start();
}
```

### 8. `src/entities/ObstacleParticle.cpp` (第三部分 - 更新和绘制)
```cpp
void ObstacleParticle::update(float deltaTime) {
    if (!isActive) return;
    
    if (isDestroying) {
        destroyTimer += deltaTime;
        if (destroyTimer >= maxDestroyTime) {
            isActive = false;
        }
        // 销毁时停止发射新粒子
        trailSystem->stop();
        auraSystem->stop();
    }
    
    updatePosition(deltaTime);
    updateRotation(deltaTime);
    updatePulse(deltaTime);
    updateParticleSystems(deltaTime);
    
    // 更新粒子系统位置
    if (trailSystem) {
        auto& emitter = trailSystem->getEmitter();
        emitter.position = position;
        trailSystem->setEmitter(emitter);
    }
    
    if (auraSystem) {
        auto& emitter = auraSystem->getEmitter();
        emitter.position = position;
        auraSystem->setEmitter(emitter);
    }
}

void ObstacleParticle::updatePosition(float deltaTime) {
    position.y += speed * deltaTime;
    
    // 轻微水平摆动（根据类型不同）
    float swingAmount = 0.0f;
    switch (currentType) {
        case Type::Fire: swingAmount = 50.0f; break;
        case Type::Electric: swingAmount = 30.0f; break;
        default: swingAmount = 0.0f; break;
    }
    
    if (swingAmount > 0) {
        float time = pulseTime * 2;
        position.x += std::sin(time) * swingAmount * deltaTime;
    }
}

void ObstacleParticle::updateRotation(float deltaTime) {
    rotation += rotationSpeed * deltaTime;
    
    // 更新形状旋转
    coreShape.setRotation(rotation);
    outlineShape.setRotation(-rotation * 0.5f); // 反向慢速旋转
}

void ObstacleParticle::updatePulse(float deltaTime) {
    pulseTime += deltaTime * pulseSpeed;
    float pulse = 0.1f * std::sin(pulseTime) + 1.0f;
    
    // 应用脉冲缩放
    coreShape.setScale(pulse, pulse);
    outlineShape.setScale(pulse, pulse);
}

void ObstacleParticle::updateParticleSystems(float deltaTime) {
    if (trailSystem) trailSystem->update(deltaTime);
    if (auraSystem) auraSystem->update(deltaTime);
    if (collisionSystem) collisionSystem->update(deltaTime);
}

void ObstacleParticle::draw(sf::RenderWindow& window) const {
    if (!isActive) return;
    
    // 先绘制粒子（在底部）
    if (auraSystem) auraSystem->draw(window);
    if (trailSystem) trailSystem->draw(window);
    
    // 再绘制主形状
    window.draw(outlineShape);
    window.draw(coreShape);
    
    // 最后绘制碰撞粒子（在最上层）
    if (collisionSystem) collisionSystem->draw(window);
}

bool ObstacleParticle::isOffScreen() const {
    return position.y > 800; // 假设屏幕高度为800
}

sf::FloatRect ObstacleParticle::getBounds() const {
    return coreShape.getGlobalBounds();
}

void ObstacleParticle::setPosition(float x, float y) {
    position.x = x;
    position.y = y;
    coreShape.setPosition(position);
    outlineShape.setPosition(position);
}

sf::Vector2f ObstacleParticle::getPosition() const {
    return position;
}

void ObstacleParticle::setSpeed(float newSpeed) {
    speed = newSpeed;
}

float ObstacleParticle::getSpeed() const {
    return speed;
}

void ObstacleParticle::setType(Type type) {
    currentType = type;
    setupByType(type);
}

void ObstacleParticle::triggerCollisionEffect() {
    // 创建碰撞粒子效果
    ParticleSystem::EmitterConfig collisionConfig;
    collisionConfig.position = position;
    collisionConfig.positionVariance = sf::Vector2f(20, 20);
    collisionConfig.velocity = sf::Vector2f(0, 0);
    collisionConfig.velocityVariance = sf::Vector2f(200, 200);
    collisionConfig.startColor = coreColor;
    collisionConfig.endColor = sf::Color(coreColor.r, coreColor.g, coreColor.b, 0);
    collisionConfig.minSize = 3.0f;
    collisionConfig.maxSize = 10.0f;
    collisionConfig.minLifetime = 0.2f;
    collisionConfig.maxLifetime = 0.8f;
    collisionConfig.emissionRate = 0; // 一次性发射
    collisionConfig.maxParticles = 50;
    collisionConfig.continuous = false;
    
    if (!collisionSystem) {
        collisionSystem = std::make_unique<ParticleSystem>();
    }
    
    collisionSystem->setEmitter(collisionConfig);
    collisionSystem->burst(30);
}

void ObstacleParticle::triggerDestroyEffect() {
    isDestroying = true;
    createDestroyParticles();
}

bool ObstacleParticle::shouldRemove() const {
    return !isActive && (!collisionSystem || collisionSystem->getActiveParticleCount() == 0);
}

void ObstacleParticle::setCoreColors(const sf::Color& core, const sf::Color& outline) {
    coreColor = core;
    outlineColor = outline;
    coreShape.setFillColor(core);
    outlineShape.setOutlineColor(outline);
}

void ObstacleParticle::createDestroyParticles() {
    // 创建销毁粒子效果
    ParticleSystem::EmitterConfig destroyConfig;
    destroyConfig.position = position;
    destroyConfig.positionVariance = sf::Vector2f(30, 30);
    destroyConfig.velocity = sf::Vector2f(0, 0);
    destroyConfig.velocityVariance = sf::Vector2f(300, 300);
    destroyConfig.startColor = coreColor;
    destroyConfig.endColor = sf::Color(coreColor.r, coreColor.g, coreColor.b, 0);
    destroyConfig.minSize = 5.0f;
    destroyConfig.maxSize = 15.0f;
    destroyConfig.minLifetime = 0.5f;
    destroyConfig.maxLifetime = 1.0f;
    destroyConfig.emissionRate = 0; // 一次性发射
    destroyConfig.maxParticles = 100;
    destroyConfig.continuous = false;
    
    if (!collisionSystem) {
        collisionSystem = std::make_unique<ParticleSystem>();
    }
    
    collisionSystem->setEmitter(destroyConfig);
    collisionSystem->burst(80);
}
```

## 五、集成到游戏中

### 9. 修改游戏主类以使用粒子障碍物

#### `src/core/Game.cpp` (修改部分)
```cpp
// 在头文件包含部分添加
#include "../entities/ObstacleParticle.h"

// 修改障碍物容器类型
// 将原来的：std::vector<Obstacle> obstacles;
// 改为：
std::vector<std::unique_ptr<ObstacleParticle>> obstacles;

// 修改 spawnObstacle 函数
void Game::spawnObstacle() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    // 随机x位置
    std::uniform_real_distribution<float> xDist(0, Config::WINDOW_WIDTH - 60);
    
    // 随机速度
    std::uniform_real_distribution<float> speedDist(
        Config::OBSTACLE_SPEED_MIN, Config::OBSTACLE_SPEED_MAX);
    
    // 随机类型
    std::uniform_int_distribution<int> typeDist(0, 3);
    
    float x = xDist(gen);
    float speed = speedDist(gen);
    int typeIndex = typeDist(gen);
    
    ObstacleParticle::Type type;
    switch (typeIndex) {
        case 0: type = ObstacleParticle::Type::Fire; break;
        case 1: type = ObstacleParticle::Type::Ice; break;
        case 2: type = ObstacleParticle::Type::Electric; break;
        case 3: type = ObstacleParticle::Type::Poison; break;
        default: type = ObstacleParticle::Type::Fire; break;
    }
    
    obstacles.push_back(std::make_unique<ObstacleParticle>(x, 0, speed, type));
}

// 修改更新函数中的障碍物循环
void Game::update(float deltaTime) {
    // ... 其他代码 ...
    
    // 更新障碍物
    for (auto& obstacle : obstacles) {
        obstacle->update(deltaTime);
    }
    
    // 移除离开屏幕或应该被移除的障碍物
    obstacles.erase(
        std::remove_if(obstacles.begin(), obstacles.end(),
            [](const std::unique_ptr<ObstacleParticle>& o) {
                return o->isOffScreen() || o->shouldRemove();
            }),
        obstacles.end()
    );
    
    // ... 其他代码 ...
}

// 修改渲染函数
void Game::render() {
    window.clear(Config::BACKGROUND_COLOR);
    
    // 绘制玩家
    player.draw(window);
    
    // 绘制障碍物
    for (auto& obstacle : obstacles) {
        obstacle->draw(window);
    }
    
    // ... 其他代码 ...
    
    window.display();
}

// 修改碰撞检测
bool Game::checkCollisions() {
    for (const auto& obstacle : obstacles) {
        if (player.getBounds().intersects(obstacle->getBounds())) {
            // 触发碰撞效果
            obstacle->triggerCollisionEffect();
            
            // 如果碰撞后应该销毁障碍物
            obstacle->triggerDestroyEffect();
            
            return true;
        }
    }
    return false;
}
```

### 10. 更新 CMakeLists.txt
```cmake
# 确保包含新文件
file(GLOB_RECURSE SOURCES 
    src/*.cpp
    src/entities/*.cpp
    src/core/*.cpp
    src/systems/*.cpp
    src/utils/*.cpp
)
```

## 六、测试和优化

### 11. 创建测试程序
```cpp
// test_particles.cpp
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "src/entities/ObstacleParticle.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Particle Obstacle Test");
    
    std::vector<std::unique_ptr<ObstacleParticle>> obstacles;
    
    // 创建不同种类的粒子障碍物
    obstacles.push_back(std::make_unique<ObstacleParticle>(100, 100, 50, 
        ObstacleParticle::Type::Fire));
    obstacles.push_back(std::make_unique<ObstacleParticle>(300, 100, 50, 
        ObstacleParticle::Type::Ice));
    obstacles.push_back(std::make_unique<ObstacleParticle>(500, 100, 50, 
        ObstacleParticle::Type::Electric));
    obstacles.push_back(std::make_unique<ObstacleParticle>(700, 100, 50, 
        ObstacleParticle::Type::Poison));
    
    sf::Clock clock;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            // 空格键触发碰撞效果
            if (event.type == sf::Event::KeyPressed && 
                event.key.code == sf::Keyboard::Space) {
                for (auto& obstacle : obstacles) {
                    obstacle->triggerCollisionEffect();
                }
            }
            
            // D键触发销毁效果
            if (event.type == sf::Event::KeyPressed && 
                event.key.code == sf::Keyboard::D) {
                for (auto& obstacle : obstacles) {
                    obstacle->triggerDestroyEffect();
                }
            }
        }
        
        float deltaTime = clock.restart().asSeconds();
        
        // 更新所有障碍物
        for (auto& obstacle : obstacles) {
            obstacle->update(deltaTime);
        }
        
        // 移除销毁完成的障碍物
        obstacles.erase(
            std::remove_if(obstacles.begin(), obstacles.end(),
                [](const std::unique_ptr<ObstacleParticle>& o) {
                    return o->shouldRemove();
                }),
            obstacles.end()
        );
        
        // 渲染
        window.clear(sf::Color(30, 30, 50)); // 深蓝色背景
        
        for (auto& obstacle : obstacles) {
            obstacle->draw(window);
        }
        
        // 显示说明文字
        sf::Font font;
        if (font.loadFromFile("arial.ttf")) {
            sf::Text text("Press SPACE for collision effect, D for destroy effect", 
                          font, 20);
            text.setPosition(50, 550);
            text.setFillColor(sf::Color::White);
            window.draw(text);
        }
        
        window.display();
    }
    
    return 0;
}
```

## 七、性能优化建议

### 12. 性能优化技巧

#### 1. 粒子池优化
```cpp
// 在ParticleSystem中添加对象池优化
class ParticlePool {
private:
    std::vector<Particle*> activeParticles;
    std::vector<Particle*> inactiveParticles;
    std::vector<Particle> particleStorage;
    
public:
    ParticlePool(size_t capacity) {
        particleStorage.resize(capacity);
        inactiveParticles.reserve(capacity);
        for (auto& particle : particleStorage) {
            inactiveParticles.push_back(&particle);
        }
    }
    
    Particle* acquire() {
        if (inactiveParticles.empty()) return nullptr;
        Particle* p = inactiveParticles.back();
        inactiveParticles.pop_back();
        activeParticles.push_back(p);
        return p;
    }
    
    void release(Particle* particle) {
        // 从活跃列表移除
        auto it = std::find(activeParticles.begin(), activeParticles.end(), particle);
        if (it != activeParticles.end()) {
            activeParticles.erase(it);
            inactiveParticles.push_back(particle);
        }
    }
};
```

#### 2. 批处理渲染
```cpp
// 使用VertexArray进行批处理渲染
class BatchRenderer {
private:
    sf::VertexArray vertices;
    sf::Texture particleTexture;
    
public:
    BatchRenderer() : vertices(sf::Quads) {
        // 创建粒子纹理（一个白色圆形）
        createParticleTexture();
    }
    
    void addParticle(const sf::Vector2f& position, float size, const sf::Color& color) {
        // 为每个粒子添加4个顶点（一个四边形）
        sf::Vector2f halfSize(size, size);
        
        vertices.append(sf::Vertex(position - halfSize, color, sf::Vector2f(0, 0)));
        vertices.append(sf::Vertex(position + sf::Vector2f(halfSize.x, -halfSize.y), 
                                 color, sf::Vector2f(1, 0)));
        vertices.append(sf::Vertex(position + halfSize, color, sf::Vector2f(1, 1)));
        vertices.append(sf::Vertex(position + sf::Vector2f(-halfSize.x, halfSize.y), 
                                 color, sf::Vector2f(0, 1)));
    }
    
    void draw(sf::RenderWindow& window) {
        window.draw(vertices, &particleTexture);
        vertices.clear();
    }
};
```

## 八、编译和运行

### 编译命令：
```bash
cd build
cmake .. -G "MinGW Makefiles"
cmake --build . --config Release
```

### 运行前准备：
1. 确保所有源文件正确放置
2. 如果需要字体，确保 `arial.ttf` 在正确位置
3. 编译成功后，复制 SFML DLL 文件到可执行文件目录

## 九、扩展功能

### 可以进一步添加的功能：

1. **粒子物理**：添加重力、碰撞、反弹等物理效果
2. **更多效果**：爆炸、烟雾、水流等粒子效果
3. **交互效果**：鼠标/触摸与粒子的交互
4. **声音效果**：为不同粒子效果添加音效
5. **性能监控**：显示当前粒子数量和帧率

## 十、故障排除

### 常见问题：

1. **编译错误**：确保包含正确的头文件和链接 SFML 库
2. **运行崩溃**：检查内存管理，确保没有空指针
3. **性能问题**：减少粒子数量或优化渲染
4. **效果不显示**：检查粒子系统初始化和更新逻辑

通过以上步骤，你将拥有一个功能丰富的粒子效果障碍物系统，可以在游戏中创建各种视觉效果。根据你的硬件性能，可以调整粒子数量以获得最佳性能。