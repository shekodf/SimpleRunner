#include "Player.h"
#include <iostream>

Player::Player() {
    shape.setSize(sf::Vector2f(Config::PLAYER_WIDTH, Config::PLAYER_HEIGHT));
    shape.setFillColor(Config::PLAYER_COLOR);
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(2.0f);
    
    reset();
}

void Player::reset() {
    shape.setPosition(Config::PLAYER_START_X, Config::PLAYER_START_Y);
    velocity = sf::Vector2f(0, 0);
}

void Player::update(float deltaTime) {
    handleInput(deltaTime);
    applyConstraints();
    
    // 更新位置
    shape.move(velocity * deltaTime);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

void Player::handleInput(float deltaTime) {
    velocity.x = 0;
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        velocity.x = -Config::PLAYER_SPEED;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        velocity.x = Config::PLAYER_SPEED;
    }
}

void Player::applyConstraints() {
    // 限制在窗口内
    sf::Vector2f position = shape.getPosition();
    
    // 左右边界
    if (position.x < 0) {
        position.x = 0;
    }
    else if (position.x + Config::PLAYER_WIDTH > Config::WINDOW_WIDTH) {
        position.x = Config::WINDOW_WIDTH - Config::PLAYER_WIDTH;
    }
    
    // 上下边界
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