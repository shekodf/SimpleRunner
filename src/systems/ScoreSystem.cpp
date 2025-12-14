#include "ScoreSystem.h"
#include <sstream>

ScoreSystem::ScoreSystem() : score(0), timeAlive(0.0f) {
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setStyle(sf::Text::Bold);
}

void ScoreSystem::update(float deltaTime) {
    timeAlive += deltaTime;
    score = static_cast<int>(timeAlive * 10); // 每0.1秒得1分
    updateText();
}

void ScoreSystem::draw(sf::RenderWindow& window) {
    window.draw(scoreText);
}

void ScoreSystem::setFont(const sf::Font& font) {
    this->font = font;
    scoreText.setFont(this->font);
    updateText();
}

void ScoreSystem::reset() {
    score = 0;
    timeAlive = 0.0f;
    updateText();
}

void ScoreSystem::updateText() {
    std::stringstream ss;
    ss << "Score: " << score << "\nTime: " << static_cast<int>(timeAlive) << "s";
    scoreText.setString(ss.str());
    scoreText.setPosition(10, 10);
}