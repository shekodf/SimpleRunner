#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "../core/Config.h"

class Bullet; // 前向声明

class Player {
public:
    Player();
    ~Player();  // 添加析构函数声明
    
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    
    void reset();
    
    // 获取碰撞边界
    sf::FloatRect getBounds() const;
    
    // 子弹相关
    bool shoot();  // 返回是否成功发射
    const std::vector<std::unique_ptr<Bullet>>& getBullets() const { return bullets; }
    int getBulletCount() const { return bullets.size(); }
    int getRemainingBullets() const { return maxBulletUses - bulletsFired; }  // 新增：获取剩余子弹数
    int getTotalBulletsFired() const { return bulletsFired; }  // 新增：获取已发射子弹数
    bool hasBulletsRemaining() const { return bulletsFired < maxBulletUses; }  // 新增：检查是否有剩余子弹
    float getShootCooldown() const { return shootCooldown; }
    float getCooldownTime() const { return cooldownTime; }
    
private:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    
    // 子弹相关
    std::vector<std::unique_ptr<Bullet>> bullets;
    int bulletsFired;          // 已发射的子弹总数
    int maxBulletUses;         // 最大子弹使用次数（3次）
    float shootCooldown;
    float cooldownTime;
    
    void handleInput(float deltaTime);
    void applyConstraints();
    
    // 视觉反馈
    float shootFeedbackTimer;
    sf::Color originalColor;
};

#endif