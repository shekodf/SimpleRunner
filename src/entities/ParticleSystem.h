#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <functional>
#include <random>
#include <algorithm>
#include "Particle.h"

class ParticleSystem {
public:
    // 粒子发射器配置
    struct EmitterConfig {
        sf::Vector2f position;           // 发射位置
        sf::Vector2f positionVariance;   // 位置变化范围
        sf::Vector2f velocity;           // 基础速度
        sf::Vector2f velocityVariance;   // 速度变化范围
        sf::Color startColor;            // 起始颜色
        sf::Color endColor;              // 结束颜色
        float minSize;                   // 最小大小
        float maxSize;                   // 最大大小
        float minLifetime;               // 最小生命周期
        float maxLifetime;               // 最大生命周期
        float emissionRate;              // 每秒发射粒子数
        int maxParticles;                // 最大粒子数
        bool continuous;                 // 是否持续发射
        
        EmitterConfig() 
            : position(0, 0), positionVariance(10, 10),
              velocity(0, 100), velocityVariance(50, 50),
              startColor(255, 100, 50, 255), endColor(255, 200, 100, 0),
              minSize(2.0f), maxSize(8.0f),
              minLifetime(0.5f), maxLifetime(2.0f),
              emissionRate(20.0f), maxParticles(500),
              continuous(true) {}
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
    
    // 设置发射器位置
    void setEmitterPosition(const sf::Vector2f& position);
    
    // 获取发射器配置
    EmitterConfig& getEmitterConfig() { return emitterConfig; }
    const EmitterConfig& getEmitterConfig() const { return emitterConfig; }
    
private:
    // 粒子池
    std::vector<std::unique_ptr<Particle>> particles;
    
    // 发射器配置
    EmitterConfig emitterConfig;
    
    // 发射器状态
    bool isEmitting;
    float emissionTimer;
    
    // 随机数生成器
    mutable std::mt19937 randomEngine;
    
    // 自定义更新器
    std::function<void(Particle&, float)> particleUpdater;
    
    // 创建新粒子
    std::unique_ptr<Particle> createParticle();
    
    // 随机浮点数生成器
    float randomFloat(float min, float max) const;
    
    // 随机颜色生成器
    sf::Color randomColor(const sf::Color& min, const sf::Color& max) const;
};

#endif