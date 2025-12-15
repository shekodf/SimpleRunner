#include "Player.h"
#include "Bullet.h"
#include <iostream>
#include <cstdlib> // 为了 rand() 函数

Player::Player() 
    : bulletsFired(0), maxBulletUses(3), shootCooldown(0.0f), cooldownTime(0.5f), 
      shootFeedbackTimer(0.0f), eyeAnimationTimer(0.0f), eyesClosed(false) {
    
    // 初始化主形状
    shape.setSize(sf::Vector2f(Config::PLAYER_WIDTH, Config::PLAYER_HEIGHT));
    shape.setFillColor(Config::PLAYER_COLOR);
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(2.0f);
    
    originalColor = shape.getFillColor();
    
    // 初始化左眼
    leftEye.setRadius(6.0f);
    leftEye.setFillColor(sf::Color::Black);
    leftEye.setOutlineColor(sf::Color::White);
    leftEye.setOutlineThickness(1.0f);
    
    // 初始化右眼
    rightEye.setRadius(6.0f);
    rightEye.setFillColor(sf::Color::Black);
    rightEye.setOutlineColor(sf::Color::White);
    rightEye.setOutlineThickness(1.0f);
    
    reset();
}

Player::~Player() {
    // 留空，让编译器自动处理
}

void Player::reset() {
    shape.setPosition(Config::PLAYER_START_X, Config::PLAYER_START_Y);
    velocity = sf::Vector2f(0, 0);
    bullets.clear();
    bulletsFired = 0;  // 重置已发射子弹数
    shootCooldown = 0.0f;
    shootFeedbackTimer = 0.0f;
    eyeAnimationTimer = 0.0f;
    eyesClosed = false;
    shape.setFillColor(originalColor);
    
    // 更新眼睛位置
    updateEyesPosition();
}

void Player::update(float deltaTime) {
    handleInput(deltaTime);
    applyConstraints();
    
    // 更新位置
    shape.move(velocity * deltaTime);
    
    // 更新子弹
    for (auto& bullet : bullets) {
        bullet->update(deltaTime);
    }
    
    // 移除离开屏幕或应该被移除的子弹
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const std::unique_ptr<Bullet>& b) {
                return b->isOffScreen() || b->shouldRemove();
            }),
        bullets.end()
    );
    
    // 更新射击冷却
    if (shootCooldown > 0) {
        shootCooldown -= deltaTime;
    }
    
    // 更新射击反馈效果
    if (shootFeedbackTimer > 0) {
        shootFeedbackTimer -= deltaTime;
        if (shootFeedbackTimer <= 0) {
            shape.setFillColor(originalColor);
        }
    }
    
    // 更新眼睛动画
    updateEyesAnimation(deltaTime);
    updateEyesPosition();
}

void Player::draw(sf::RenderWindow& window) {
    // 先绘制玩家主体
    window.draw(shape);
    
    // 绘制眼睛（如果眼睛没有闭合）
    if (!eyesClosed) {
        window.draw(leftEye);
        window.draw(rightEye);
    } else {
        // 绘制闭合的眼睛（两条线）
        drawClosedEyes(window);
    }
    
    // 绘制子弹（在玩家上面）
    for (auto& bullet : bullets) {
        bullet->draw(window);
    }
    
    // 绘制射击反馈（射击时发光）
    if (shootFeedbackTimer > 0) {
        float intensity = shootFeedbackTimer / 0.15f;
        sf::RectangleShape feedbackShape(shape);
        feedbackShape.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(100 * intensity)));
        feedbackShape.setOutlineColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(200 * intensity)));
        window.draw(feedbackShape);
    }
}

void Player::handleInput(float deltaTime) {
    velocity.x = 0;
    velocity.y = 0;
    
    // 左右移动
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        velocity.x = -Config::PLAYER_SPEED;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        velocity.x = Config::PLAYER_SPEED;
    }
    
    // 上下移动（新增）
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        velocity.y = -Config::PLAYER_SPEED;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        velocity.y = Config::PLAYER_SPEED;
    }
    
    // 发射子弹（空格键）- 最多只能发射3次
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && 
        shootCooldown <= 0 && 
        bulletsFired < maxBulletUses) {
        if (shoot()) {
            shootCooldown = cooldownTime;
        }
    }
}

void Player::applyConstraints() {
    sf::Vector2f position = shape.getPosition();
    
    // 左右边界
    if (position.x < 0) {
        position.x = 0;
    }
    else if (position.x + Config::PLAYER_WIDTH > Config::WINDOW_WIDTH) {
        position.x = Config::WINDOW_WIDTH - Config::PLAYER_WIDTH;
    }
    
    // 上下边界（新增）
    if (position.y < 0) {
        position.y = 0;
    }
    else if (position.y + Config::PLAYER_HEIGHT > Config::WINDOW_HEIGHT) {
        position.y = Config::WINDOW_HEIGHT - Config::PLAYER_HEIGHT;
    }
    
    shape.setPosition(position);
}

sf::FloatRect Player::getBounds() const {
    return shape.getGlobalBounds();
}

bool Player::shoot() {
    // 检查是否已达到最大发射次数
    if (bulletsFired >= maxBulletUses) {
        std::cout << "No bullets remaining!" << std::endl;
        return false;
    }
    
    // 从玩家中心发射子弹
    float bulletX = shape.getPosition().x + shape.getSize().x / 2.0f;
    float bulletY = shape.getPosition().y - 10.0f; // 从玩家上方发射
    
    bullets.push_back(std::make_unique<Bullet>(bulletX, bulletY));
    bulletsFired++;  // 增加已发射子弹计数
    
    // 射击反馈效果
    shootFeedbackTimer = 0.15f;
    shape.setFillColor(sf::Color(255, 255, 255, 200));
    
    // 射击时眨眼
    eyeAnimationTimer = 0.1f;
    eyesClosed = true;
    
    std::cout << "Bullet fired! (" << bulletsFired << "/" << maxBulletUses << " bullets used)" << std::endl;
    
    return true;
}

void Player::updateEyesPosition() {
    sf::Vector2f playerPos = shape.getPosition();
    sf::Vector2f playerSize = shape.getSize();
    
    // 左眼位置（左上角）
    leftEye.setPosition(
        playerPos.x + playerSize.x * 0.25f - leftEye.getRadius(),
        playerPos.y + playerSize.y * 0.3f - leftEye.getRadius()
    );
    
    // 右眼位置（右上角）
    rightEye.setPosition(
        playerPos.x + playerSize.x * 0.75f - rightEye.getRadius(),
        playerPos.y + playerSize.y * 0.3f - rightEye.getRadius()
    );
}

void Player::updateEyesAnimation(float deltaTime) {
    if (eyesClosed) {
        eyeAnimationTimer -= deltaTime;
        if (eyeAnimationTimer <= 0) {
            eyesClosed = false;
            eyeAnimationTimer = 3.0f + (rand() % 10) / 10.0f; // 随机3-4秒后再次眨眼
        }
    } else {
        eyeAnimationTimer -= deltaTime;
        if (eyeAnimationTimer <= 0) {
            eyesClosed = true;
            eyeAnimationTimer = 0.1f; // 眨眼持续0.1秒
        }
    }
}

void Player::drawClosedEyes(sf::RenderWindow& window) {
    sf::Vector2f playerPos = shape.getPosition();
    sf::Vector2f playerSize = shape.getSize();
    
    // 左眼闭合线
    sf::Vertex leftEyeLine[] = {
        sf::Vertex(sf::Vector2f(
            playerPos.x + playerSize.x * 0.25f - leftEye.getRadius(),
            playerPos.y + playerSize.y * 0.3f
        ), sf::Color::Black),
        sf::Vertex(sf::Vector2f(
            playerPos.x + playerSize.x * 0.25f + leftEye.getRadius(),
            playerPos.y + playerSize.y * 0.3f
        ), sf::Color::Black)
    };
    
    // 右眼闭合线
    sf::Vertex rightEyeLine[] = {
        sf::Vertex(sf::Vector2f(
            playerPos.x + playerSize.x * 0.75f - rightEye.getRadius(),
            playerPos.y + playerSize.y * 0.3f
        ), sf::Color::Black),
        sf::Vertex(sf::Vector2f(
            playerPos.x + playerSize.x * 0.75f + rightEye.getRadius(),
            playerPos.y + playerSize.y * 0.3f
        ), sf::Color::Black)
    };
    
    window.draw(leftEyeLine, 2, sf::Lines);
    window.draw(rightEyeLine, 2, sf::Lines);
}