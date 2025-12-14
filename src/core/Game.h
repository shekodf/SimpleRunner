#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Config.h"
#include "../entities/Player.h"
#include "../entities/ObstacleParticle.h"  // 使用粒子障碍物
#include "../systems/ScoreSystem.h"

class Game {
public:
    Game();
    void run();
    
private:
    void processEvents();
    void update(float deltaTime);
    void render();
    
    // 游戏状态
    enum class GameState {
        Playing,
        GameOver
    };
    
    GameState currentState;
    sf::Clock gameClock;
    float obstacleSpawnTimer;
    
    // SFML窗口和渲染
    sf::RenderWindow window;
    
    // 游戏实体
    Player player;
    std::vector<std::unique_ptr<ObstacleParticle>> obstacles;  // 粒子障碍物
    
    // 游戏系统
    ScoreSystem scoreSystem;
    
    // 游戏字体
    sf::Font font;
    
    // 生成障碍物
    void spawnObstacle();
    
    // 碰撞检测
    bool checkCollisions();
    
    // 绘制UI
    void drawUI();
    
    // 绘制调试信息
    void drawDebugInfo();
    
    // 绘制游戏说明
    void drawInstructions();
};

#endif