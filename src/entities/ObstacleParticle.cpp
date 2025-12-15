#include "ObstacleParticle.h"
#include <random>
#include <cmath>
#include <iostream>

ObstacleParticle::ObstacleParticle(float x, float y, float speed, Type type)
    : position(x, y), speed(speed), rotation(0.0f), rotationSpeed(0.0f),
      pulseScale(1.0f), pulseSpeed(2.0f), pulseTime(0.0f),
      isActive(true), isDestroying(false), hitByBullet(false), destroyTimer(0.0f), maxDestroyTime(0.3f) {  // 减少销毁时间为0.3秒
    
    // 确定类型
    if (type == Type::Random) {
        int typeIndex = randomInt(0, 3);
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
    
    // 随机旋转速度
    rotationSpeed = randomFloat(-180.0f, 180.0f);
    
    // 随机脉冲速度
    pulseSpeed = randomFloat(1.0f, 3.0f);
    
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
    float coreRadius = 20.0f;
    coreShape.setRadius(coreRadius);
    coreShape.setOrigin(coreRadius, coreRadius);
    coreShape.setPosition(position);
    
    // 外框形状（方形，用于旋转效果）
    float outlineSize = 40.0f;
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

void ObstacleParticle::setupFireEffect() {
    // 核心颜色：红色到橙色
    setCoreColors(sf::Color(255, 100, 50, 200), sf::Color(255, 200, 100, 100));
    
    // 轨迹粒子：火焰拖尾
    ParticleSystem::EmitterConfig trailConfig;
    trailConfig.position = position;
    trailConfig.positionVariance = sf::Vector2f(5, 5);
    trailConfig.velocity = sf::Vector2f(0, -speed * 0.3f); // 向上飘散
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
    auraConfig.emissionRate = 40.0f;
    auraConfig.maxParticles = 150;
    auraConfig.continuous = true;
    
    auraSystem->setEmitter(auraConfig);
    auraSystem->start();
    
    // 设置火焰粒子更新器
    trailSystem->setParticleUpdater([this](Particle& p, float dt) {
        // 火焰向上飘，轻微左右摆动
        auto pos = p.getPosition();
        pos.x += std::sin(p.getLifeRatio() * 10) * 10 * dt;
        pos.y -= 50 * dt;
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
    
    // 设置冰霜粒子更新器
    trailSystem->setParticleUpdater([](Particle& p, float dt) {
        // 冰霜粒子缓慢飘落
        auto pos = p.getPosition();
        pos.y += 20 * dt;
        p.setPosition(pos);
    });
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
    auraConfig.emissionRate = 80.0f;
    auraConfig.maxParticles = 200;
    auraConfig.continuous = true;
    
    auraSystem->setEmitter(auraConfig);
    auraSystem->start();
    
    // 设置电弧粒子更新器
    auraSystem->setParticleUpdater([this](Particle& p, float dt) {
        // 电弧粒子快速闪烁移动
        auto pos = p.getPosition();
        float time = pulseTime * 5;
        pos.x += std::sin(time) * 50 * dt;
        pos.y += std::cos(time) * 50 * dt;
        p.setPosition(pos);
    });
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

void ObstacleParticle::update(float deltaTime) {
    if (!isActive) return;
    
    // 如果是被子弹击中的状态，立即销毁
    if (hitByBullet) {
        isActive = false;
        isDestroying = true;
        // 停止所有粒子发射
        if (trailSystem) trailSystem->stop();
        if (auraSystem) auraSystem->stop();
        return;
    }
    
    if (isDestroying) {
        destroyTimer += deltaTime;
        if (destroyTimer >= maxDestroyTime) {
            isActive = false;
        }
        // 销毁时停止发射新粒子
        if (trailSystem) trailSystem->stop();
        if (auraSystem) auraSystem->stop();
    }
    
    updatePosition(deltaTime);
    updateRotation(deltaTime);
    updatePulse(deltaTime);
    updateParticleSystems(deltaTime);
    
    // 更新粒子系统位置
    if (trailSystem) {
        trailSystem->setEmitterPosition(position);
    }
    
    if (auraSystem) {
        auraSystem->setEmitterPosition(position);
    }
}

void ObstacleParticle::draw(sf::RenderWindow& window) const {
    if (!isActive) return;
    
    // 如果是被子弹击中的状态，不绘制任何东西
    if (hitByBullet) return;
    
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

sf::Vector2f ObstacleParticle::getPosition() const {
    return position;
}

float ObstacleParticle::getSpeed() const {
    return speed;
}

void ObstacleParticle::setSpeed(float newSpeed) {
    speed = newSpeed;
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

void ObstacleParticle::destroyImmediately() {
    hitByBullet = true;  // 标记为被子弹击中
    isActive = false;    // 立即变为不活跃
    isDestroying = true; // 标记为正在销毁
    
    // 停止所有粒子发射
    if (trailSystem) trailSystem->stop();
    if (auraSystem) auraSystem->stop();
    
    // 可以创建一个快速的爆炸效果
    triggerCollisionEffect();
}

bool ObstacleParticle::shouldRemove() const {
    // 如果被子弹击中，立即移除
    if (hitByBullet) return true;
    
    // 否则使用原来的逻辑
    return !isActive && (!collisionSystem || collisionSystem->getActiveParticleCount() == 0);
}

// ... [保留原有的 updatePosition、updateRotation、updatePulse、updateParticleSystems 函数] ...

// ... [保留原有的 setupFireEffect、setupIceEffect、setupElectricEffect、setupPoisonEffect 函数] ...

void ObstacleParticle::setCoreColors(const sf::Color& core, const sf::Color& outline) {
    coreColor = core;
    outlineColor = outline;
    coreShape.setFillColor(core);
    outlineShape.setOutlineColor(outline);
}

void ObstacleParticle::createDestroyParticles() {
    // 创建销毁粒子效果（只有在玩家碰撞时才使用）
    if (!hitByBullet) {  // 被子弹击中时不创建粒子效果
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
}

float ObstacleParticle::randomFloat(float min, float max) const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

int ObstacleParticle::randomInt(int min, int max) const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

void ObstacleParticle::adjustSpeed(float multiplier) {
    speed *= multiplier;
    
    // 如果需要，也可以调整粒子系统的速度
    // 例如：更新轨迹粒子速度等
}

// ... [已有的代码] ...

// 私有函数实现

void ObstacleParticle::updatePosition(float deltaTime) {
    position.y += speed * deltaTime;
    
    // 轻微水平摆动（根据类型不同）
    float swingAmount = 0.0f;
    switch (currentType) {
        case Type::Fire: swingAmount = 30.0f; break;
        case Type::Electric: swingAmount = 20.0f; break;
        default: swingAmount = 0.0f; break;
    }
    
    if (swingAmount > 0) {
        position.x += std::sin(pulseTime * 2) * swingAmount * deltaTime;
    }
    
    // 更新形状位置
    coreShape.setPosition(position);
    outlineShape.setPosition(position);
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
