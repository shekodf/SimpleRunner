#ifndef CONFIG_H
#define CONFIG_H

#include <SFML/Graphics.hpp>

namespace Config {
    // 窗口设置
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    const std::string WINDOW_TITLE = "Simple Runner with Particles";
    
    // 游戏设置
    const float GRAVITY = 500.0f;
    const float PLAYER_SPEED = 300.0f;
    
    // 颜色定义
    const sf::Color BACKGROUND_COLOR = sf::Color(30, 30, 46);
    const sf::Color PLAYER_COLOR = sf::Color(88, 199, 250);
    const sf::Color OBSTACLE_COLOR = sf::Color(250, 82, 82);
    
    // 玩家设置
    const float PLAYER_WIDTH = 50.0f;
    const float PLAYER_HEIGHT = 50.0f;
    const float PLAYER_START_X = 400.0f;
    const float PLAYER_START_Y = 500.0f;
    
    // 障碍物设置
    const float OBSTACLE_WIDTH = 40.0f;
    const float OBSTACLE_HEIGHT = 40.0f;
    const float OBSTACLE_SPAWN_TIME = 1.5f;  // 减慢生成速度
    const float OBSTACLE_SPEED_MIN = 100.0f;
    const float OBSTACLE_SPEED_MAX = 250.0f;
    
    // 粒子障碍物设置
    const float PARTICLE_OBSTACLE_RADIUS = 20.0f;
    const float PARTICLE_OBSTACLE_SPAWN_TIME = 1.2f;
    
    // 速度增长设置
    const float SPEED_INCREASE_INTERVAL = 10.0f;  // 每10秒增加一次速度
    const float SPEED_INCREASE_AMOUNT = 20.0f;    // 每次增加20速度单位
    const float MAX_OBSTACLE_SPEED = 500.0f;      // 最大速度限制
}
#endif
