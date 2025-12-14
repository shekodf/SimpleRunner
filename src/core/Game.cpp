#include "Game.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <sstream>

Game::Game() 
    : window(sf::VideoMode(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT), 
             Config::WINDOW_TITLE),
      currentState(GameState::Playing),
      obstacleSpawnTimer(0.0f) {
    
    // 设置窗口属性
    window.setFramerateLimit(60);
    
    // 初始化字体
    if (!font.loadFromFile("../assets/fonts/arial.ttf")) {
        std::cerr << "Failed to load font. Using default." << std::endl;
        // 如果找不到字体，创建一个默认的空字体
        font.loadFromMemory(nullptr, 0);
    }
    
    scoreSystem.setFont(font);
    
    std::cout << "===========================================" << std::endl;
    std::cout << "Simple Runner with Particle Obstacles" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "- A/Left Arrow: Move left" << std::endl;
    std::cout << "- D/Right Arrow: Move right" << std::endl;
    std::cout << "- R: Restart game" << std::endl;
    std::cout << "- ESC: Exit game" << std::endl;
    std::cout << "===========================================" << std::endl;
}

void Game::run() {
    sf::Clock clock;
    
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        
        processEvents();
        update(deltaTime);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        // 重新开始游戏
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::R && currentState == GameState::GameOver) {
                // 重置游戏
                currentState = GameState::Playing;
                player.reset();
                obstacles.clear();
                scoreSystem.reset();
                obstacleSpawnTimer = 0.0f;
                std::cout << "Game restarted!" << std::endl;
            }
            
            // 退出游戏
            if (event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }
    }
}

void Game::update(float deltaTime) {
    if (currentState != GameState::Playing) {
        return;
    }
    
    // 更新玩家
    player.update(deltaTime);
    
    // 更新障碍物
    for (auto& obstacle : obstacles) {
        obstacle->update(deltaTime);
    }
    
    // 移除离开屏幕或应该被移除的障碍物
    obstacles.erase(
        std::remove_if(obstacles.begin(), obstacles.end(),
            [](const std::unique_ptr<ObstacleParticle>& o) {
                return o->isOffScreen() || o->shouldRemove();
            }),
        obstacles.end()
    );
    
    // 生成新障碍物
    obstacleSpawnTimer += deltaTime;
    if (obstacleSpawnTimer >= Config::PARTICLE_OBSTACLE_SPAWN_TIME) {
        spawnObstacle();
        obstacleSpawnTimer = 0.0f;
    }
    
    // 更新分数
    scoreSystem.update(deltaTime);
    
    // 检测碰撞
    if (checkCollisions()) {
        currentState = GameState::GameOver;
        std::cout << "Game Over! Final score: " << scoreSystem.getScore() << std::endl;
    }
}

void Game::render() {
    window.clear(Config::BACKGROUND_COLOR);
    
    // 绘制所有实体
    player.draw(window);
    
    for (auto& obstacle : obstacles) {
        obstacle->draw(window);
    }
    
    // 绘制UI
    drawUI();
    
    // 绘制调试信息
    drawDebugInfo();
    
    // 绘制游戏说明
    drawInstructions();
    
    window.display();
}

void Game::spawnObstacle() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    // 随机x位置
    std::uniform_real_distribution<float> xDist(
        Config::PARTICLE_OBSTACLE_RADIUS, 
        Config::WINDOW_WIDTH - Config::PARTICLE_OBSTACLE_RADIUS);
    
    // 随机速度
    std::uniform_real_distribution<float> speedDist(
        Config::OBSTACLE_SPEED_MIN, Config::OBSTACLE_SPEED_MAX);
    
    // 随机类型
    std::uniform_int_distribution<int> typeDist(0, 3);
    
    float x = xDist(gen);
    float speed = speedDist(gen);
    int typeIndex = typeDist(gen);
    
    ObstacleParticle::Type type;
    switch (typeIndex) {
        case 0: 
            type = ObstacleParticle::Type::Fire;
            break;
        case 1: 
            type = ObstacleParticle::Type::Ice;
            break;
        case 2: 
            type = ObstacleParticle::Type::Electric;
            break;
        case 3: 
            type = ObstacleParticle::Type::Poison;
            break;
        default: 
            type = ObstacleParticle::Type::Fire;
            break;
    }
    
    obstacles.push_back(std::make_unique<ObstacleParticle>(x, -50, speed, type));
    
    // 控制台输出（可选）
    static int obstacleCount = 0;
    obstacleCount++;
    if (obstacleCount % 5 == 0) {
        std::cout << "Spawned obstacle #" << obstacleCount 
                  << " (type: " << typeIndex << ", speed: " << speed << ")" << std::endl;
    }
}

bool Game::checkCollisions() {
    for (const auto& obstacle : obstacles) {
        if (player.getBounds().intersects(obstacle->getBounds())) {
            // 触发碰撞效果
            obstacle->triggerCollisionEffect();
            obstacle->triggerDestroyEffect();
            
            // 控制台输出碰撞信息
            std::cout << "Collision with obstacle type: ";
            switch (obstacle->getType()) {
                case ObstacleParticle::Type::Fire: std::cout << "Fire"; break;
                case ObstacleParticle::Type::Ice: std::cout << "Ice"; break;
                case ObstacleParticle::Type::Electric: std::cout << "Electric"; break;
                case ObstacleParticle::Type::Poison: std::cout << "Poison"; break;
                default: std::cout << "Unknown"; break;
            }
            std::cout << std::endl;
            
            return true;
        }
    }
    return false;
}

void Game::drawUI() {
    // 绘制分数
    scoreSystem.draw(window);
    
    // 游戏结束画面
    if (currentState == GameState::GameOver) {
        // 半透明黑色覆盖层
        sf::RectangleShape overlay(sf::Vector2f(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT));
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);
        
        // 游戏结束文字
        sf::Text gameOverText("GAME OVER!", font, 48);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setStyle(sf::Text::Bold);
        
        sf::FloatRect textRect = gameOverText.getLocalBounds();
        gameOverText.setOrigin(textRect.left + textRect.width / 2.0f,
                              textRect.top + textRect.height / 2.0f);
        gameOverText.setPosition(Config::WINDOW_WIDTH / 2.0f,
                                Config::WINDOW_HEIGHT / 2.0f - 50);
        window.draw(gameOverText);
        
        // 重新开始提示
        sf::Text restartText("Press R to restart", font, 24);
        restartText.setFillColor(sf::Color::White);
        textRect = restartText.getLocalBounds();
        restartText.setOrigin(textRect.left + textRect.width / 2.0f,
                            textRect.top + textRect.height / 2.0f);
        restartText.setPosition(Config::WINDOW_WIDTH / 2.0f,
                              Config::WINDOW_HEIGHT / 2.0f + 20);
        window.draw(restartText);
    }
}

void Game::drawDebugInfo() {
    // 绘制障碍物数量
    std::stringstream debugStream;
    debugStream << "Obstacles: " << obstacles.size();
    
    sf::Text debugText(debugStream.str(), font, 16);
    debugText.setFillColor(sf::Color::White);
    debugText.setPosition(10, Config::WINDOW_HEIGHT - 40);
    window.draw(debugText);
    
    // 绘制帧率信息（简单版）
    static sf::Clock fpsClock;
    static int frameCount = 0;
    static float fps = 0.0f;
    
    frameCount++;
    if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
        fps = frameCount / fpsClock.restart().asSeconds();
        frameCount = 0;
    }
    
    std::stringstream fpsStream;
    fpsStream << "FPS: " << static_cast<int>(fps);
    
    sf::Text fpsText(fpsStream.str(), font, 16);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10, Config::WINDOW_HEIGHT - 20);
    window.draw(fpsText);
}

void Game::drawInstructions() {
    // 绘制游戏说明
    if (currentState == GameState::Playing) {
        sf::Text instructionText("Avoid the particle obstacles!", font, 18);
        instructionText.setFillColor(sf::Color(200, 200, 200));
        instructionText.setPosition(10, Config::WINDOW_HEIGHT - 80);
        window.draw(instructionText);
        
        // 绘制障碍物类型说明
        sf::Text typesText("Obstacle types: Fire(red) Ice(blue) Electric(purple) Poison(green)", 
                          font, 14);
        typesText.setFillColor(sf::Color(180, 180, 180));
        typesText.setPosition(10, Config::WINDOW_HEIGHT - 60);
        window.draw(typesText);
    }
}