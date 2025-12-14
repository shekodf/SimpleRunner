#ifndef SCORESYSTEM_H
#define SCORESYSTEM_H

#include <SFML/Graphics.hpp>

class ScoreSystem {
public:
    ScoreSystem();
    
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    
    void setFont(const sf::Font& font);
    void reset();
    
    // 获取当前分数
    int getScore() const { return score; }
    
    // 获取存活时间
    float getTimeAlive() const { return timeAlive; }
    
private:
    int score;
    float timeAlive;
    sf::Font font;
    sf::Text scoreText;
    
    void updateText();
};

#endif