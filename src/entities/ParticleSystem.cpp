#include "ParticleSystem.h"
#include <iostream>

ParticleSystem::ParticleSystem()
    : isEmitting(false), emissionTimer(0.0f) {
    
    // 初始化随机数生成器
    std::random_device rd;
    randomEngine.seed(rd());
    
    // 使用默认配置
}

ParticleSystem::~ParticleSystem() {
    clear();
}

void ParticleSystem::setEmitter(const EmitterConfig& config) {
    emitterConfig = config;
}

void ParticleSystem::burst(int count) {
    for (int i = 0; i < count && particles.size() < emitterConfig.maxParticles; i++) {
        auto particle = createParticle();
        
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
        
        // 随机旋转速度
        float rotationSpeed = randomFloat(-180.0f, 180.0f);
        
        particle->init(sf::Vector2f(posX, posY),
                      sf::Vector2f(velX, velY),
                      lifetime,
                      color,
                      size);
        
        particle->setRotationSpeed(rotationSpeed);
        
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
        
        // 更新粒子
        particle->update(deltaTime);
        
        // 如果粒子死亡，移除它
        if (!particle->isAlive()) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

void ParticleSystem::draw(sf::RenderWindow& window) const {
    // 绘制所有粒子
    for (const auto& particle : particles) {
        particle->draw(window);
    }
}

void ParticleSystem::clear() {
    particles.clear();
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

void ParticleSystem::setEmitterPosition(const sf::Vector2f& position) {
    emitterConfig.position = position;
}

std::unique_ptr<Particle> ParticleSystem::createParticle() {
    return std::make_unique<Particle>();
}

float ParticleSystem::randomFloat(float min, float max) const {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(randomEngine);
}

sf::Color ParticleSystem::randomColor(const sf::Color& min, const sf::Color& max) const {
    return sf::Color(
        static_cast<sf::Uint8>(randomFloat(min.r, max.r)),
        static_cast<sf::Uint8>(randomFloat(min.g, max.g)),
        static_cast<sf::Uint8>(randomFloat(min.b, max.b)),
        static_cast<sf::Uint8>(randomFloat(min.a, max.a))
    );
}