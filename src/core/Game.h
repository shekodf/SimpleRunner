#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Config.h"
#include "../entities/Player.h"
#include "../entities/ObstacleParticle.h"
#include "../systems/ScoreSystem.h"

class Game {
public:
    Game();
    void run();
    
private:
    void processEvents();
    void update(float deltaTime);
    void render();
    
    // 游戏状态 - 添加开始界面
    enum class GameState {
        StartScreen,  // 开始界面
        Playing,      // 游戏中
        GameOver      // 游戏结束
    };
    
    GameState currentState;
    sf::Clock gameClock;
    float obstacleSpawnTimer;
    
    float speedIncreaseTimer;
    float currentObstacleSpeedMin;
    float currentObstacleSpeedMax;
    int speedLevel;
    
    sf::RenderWindow window;
    
    Player player;
    std::vector<std::unique_ptr<ObstacleParticle>> obstacles;
    
    ScoreSystem scoreSystem;
    
    sf::Font font;
    
    // 开始界面相关
    bool showInstructions;
    float blinkTimer;
    sf::Text pressAnyKeyText;
    
    void spawnObstacle();
    bool checkCollisions();
    void drawUI();
    void drawDebugInfo();
    void drawStartScreen();  // 改为绘制开始界面
    void drawGameInstructions();  // 游戏中的说明
    void drawGameOverUI();  // 新增：绘制游戏结束界面
    void updateDifficulty(float deltaTime);
    void resetDifficulty();
    void startGame();  // 开始游戏
};

#endif