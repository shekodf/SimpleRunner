#include "Game.h"
#include "../entities/Bullet.h"  // 添加这行，包含Bullet类的定义
#include <iostream>
#include <random>
#include <algorithm>
#include <sstream>
#include <iomanip>

Game::Game() 
    : window(sf::VideoMode(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT), 
             Config::WINDOW_TITLE),
      currentState(GameState::StartScreen),
      obstacleSpawnTimer(0.0f),
      speedIncreaseTimer(0.0f),
      currentObstacleSpeedMin(Config::OBSTACLE_SPEED_MIN),
      currentObstacleSpeedMax(Config::OBSTACLE_SPEED_MAX),
      speedLevel(0),
      showInstructions(true),
      blinkTimer(0.0f) {
    
    window.setFramerateLimit(60);
    
    // 尝试多个字体路径
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        if (!font.loadFromFile("../assets/fonts/arial.ttf")) {
            if (!font.loadFromFile("arial.ttf")) {
                // 如果所有字体文件都找不到，尝试加载系统字体
                std::string systemFontPath = "C:/Windows/Fonts/arial.ttf";
                if (!font.loadFromFile(systemFontPath)) {
                    std::cerr << "Failed to load font. Using default." << std::endl;
                    // 创建一个非常基本的默认字体
                    font.loadFromMemory(nullptr, 0);
                }
            }
        }
    }
    
    scoreSystem.setFont(font);
    
    pressAnyKeyText.setFont(font);
    pressAnyKeyText.setString("Press any key to start...");
    pressAnyKeyText.setCharacterSize(24);
    pressAnyKeyText.setFillColor(sf::Color::White);
    sf::FloatRect textRect = pressAnyKeyText.getLocalBounds();
    pressAnyKeyText.setOrigin(textRect.left + textRect.width / 2.0f,
                             textRect.top + textRect.height / 2.0f);
    pressAnyKeyText.setPosition(Config::WINDOW_WIDTH / 2.0f,
                               Config::WINDOW_HEIGHT - 80);
    
    std::cout << "===========================================" << std::endl;
    std::cout << "Simple Runner with Particle Obstacles" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "Waiting for player to start game..." << std::endl;
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
        
        if (currentState == GameState::StartScreen) {
            if (event.type == sf::Event::KeyPressed) {
                startGame();
                std::cout << "Game started!" << std::endl;
            }
        }
        else if (currentState == GameState::Playing || currentState == GameState::GameOver) {
            if (event.type == sf::Event::KeyPressed) {
                // 重新开始游戏 (仅限游戏结束状态)
                if (event.key.code == sf::Keyboard::R && currentState == GameState::GameOver) {
                    currentState = GameState::Playing;
                    player.reset();
                    obstacles.clear();
                    scoreSystem.reset();
                    obstacleSpawnTimer = 0.0f;
                    resetDifficulty();
                    std::cout << "Game restarted!" << std::endl;
                    std::cout << "Speed reset to level 0" << std::endl;
                }
                
                // 退出游戏
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
                
                // 切换说明显示（游戏中）
                if (event.key.code == sf::Keyboard::H && currentState == GameState::Playing) {
                    showInstructions = !showInstructions;
                    std::cout << "Instructions " << (showInstructions ? "shown" : "hidden") << std::endl;
                }
                
                // 返回菜单（游戏中或游戏结束都可以）
                if (event.key.code == sf::Keyboard::M && 
                   (currentState == GameState::Playing || currentState == GameState::GameOver)) {
                    currentState = GameState::StartScreen;
                    player.reset();
                    obstacles.clear();
                    scoreSystem.reset();
                    obstacleSpawnTimer = 0.0f;
                    resetDifficulty();
                    blinkTimer = 0.0f; // 重置闪烁计时器
                    std::cout << "Returned to start screen" << std::endl;
                }
            }
        }
    }
}

// 在Game.cpp的update函数中，添加额外的子弹管理
void Game::update(float deltaTime) {
    if (currentState == GameState::StartScreen) {
        blinkTimer += deltaTime;
        if (blinkTimer >= 1.0f) {
            blinkTimer = 0.0f;
        }
        return;
    }
    
    if (currentState != GameState::Playing) {
        return;
    }
    
    player.update(deltaTime);
    
    for (auto& obstacle : obstacles) {
        obstacle->update(deltaTime);
    }
    
    // 移除离开屏幕的障碍物
    obstacles.erase(
        std::remove_if(obstacles.begin(), obstacles.end(),
            [](const std::unique_ptr<ObstacleParticle>& o) {
                return o->isOffScreen() || o->shouldRemove();
            }),
        obstacles.end()
    );
    
    // 生成障碍物
    obstacleSpawnTimer += deltaTime;
    if (obstacleSpawnTimer >= Config::PARTICLE_OBSTACLE_SPAWN_TIME) {
        spawnObstacle();
        obstacleSpawnTimer = 0.0f;
    }
    
    scoreSystem.update(deltaTime);
    updateDifficulty(deltaTime);
    
    // 检测子弹与障碍物的碰撞
    checkBulletCollisions();
    
    // 检测玩家与障碍物的碰撞
    if (checkCollisions()) {
        currentState = GameState::GameOver;
        std::cout << "Game Over! Final score: " << scoreSystem.getScore() << std::endl;
        std::cout << "Final speed level: " << speedLevel << std::endl;
    }
}

// 新增函数：检测子弹与障碍物的碰撞
void Game::checkBulletCollisions() {
    for (auto& bullet : player.getBullets()) {
        if (!bullet->isActive()) continue;
        
        for (auto& obstacle : obstacles) {
            if (bullet->getBounds().intersects(obstacle->getBounds())) {
                // 触发子弹的销毁效果
                bullet->triggerDestroyEffect();
                
                // 触发障碍物的碰撞效果和销毁效果
                obstacle->triggerCollisionEffect();
                obstacle->triggerDestroyEffect();
                
                // 增加分数（击碎障碍物得50分）
                scoreSystem.addScore(50);
                
                std::cout << "Obstacle destroyed! +50 points" << std::endl;
                
                break; // 一颗子弹只能击中一个障碍物
            }
        }
    }
}

bool Game::checkCollisions() {
    for (const auto& obstacle : obstacles) {
        if (player.getBounds().intersects(obstacle->getBounds())) {
            // 触发碰撞效果
            obstacle->triggerCollisionEffect();
            obstacle->triggerDestroyEffect();
            
            std::cout << "Collision with obstacle type: ";
            switch (obstacle->getType()) {
                case ObstacleParticle::Type::Fire: std::cout << "Fire"; break;
                case ObstacleParticle::Type::Ice: std::cout << "Ice"; break;
                case ObstacleParticle::Type::Electric: std::cout << "Electric"; break;
                case ObstacleParticle::Type::Poison: std::cout << "Poison"; break;
                default: std::cout << "Unknown"; break;
            }
            std::cout << " at speed level " << speedLevel << std::endl;
            
            return true;
        }
    }
    return false;
}

void Game::render() {
    window.clear(Config::BACKGROUND_COLOR);
    
    switch (currentState) {
        case GameState::StartScreen:
            drawStartScreen();
            break;
            
        case GameState::Playing:
            player.draw(window);
            
            for (auto& obstacle : obstacles) {
                obstacle->draw(window);
            }
            
            drawUI();
            drawDebugInfo();
            
            if (showInstructions) {
                drawGameInstructions();
            }
            break;
            
        case GameState::GameOver:
            player.draw(window);
            
            for (auto& obstacle : obstacles) {
                obstacle->draw(window);
            }
            
            drawUI();
            drawDebugInfo();
            drawGameOverUI();
            break;
    }
    
    window.display();
}

void Game::drawStartScreen() {
    sf::Text titleText("Simple Runner with Particles", font, 48);
    titleText.setFillColor(sf::Color::Yellow);
    titleText.setStyle(sf::Text::Bold);
    
    sf::FloatRect titleRect = titleText.getLocalBounds();
    titleText.setOrigin(titleRect.left + titleRect.width / 2.0f,
                       titleRect.top + titleRect.height / 2.0f);
    titleText.setPosition(Config::WINDOW_WIDTH / 2.0f, 80);  // 上移标题
    
    // 绘制游戏图标/装饰
    sf::CircleShape titleCircle(60);
    titleCircle.setFillColor(sf::Color(255, 100, 50, 100));
    titleCircle.setOutlineColor(sf::Color::Yellow);
    titleCircle.setOutlineThickness(3);
    titleCircle.setOrigin(60, 60);
    titleCircle.setPosition(Config::WINDOW_WIDTH / 2.0f, 100);
    window.draw(titleCircle);
    
    window.draw(titleText);
    
    // 增大黑框，适应更多文字
    sf::RectangleShape instructionBox(sf::Vector2f(650, 380));
    instructionBox.setFillColor(sf::Color(0, 0, 0, 180));
    instructionBox.setOutlineColor(sf::Color::White);
    instructionBox.setOutlineThickness(3);
    instructionBox.setPosition((Config::WINDOW_WIDTH - 650) / 2.0f, 170); // 下移一点
    window.draw(instructionBox);
    
    sf::Text instructionTitle("Game Instructions", font, 28); // 减小字体
    instructionTitle.setFillColor(sf::Color::Cyan);
    instructionTitle.setStyle(sf::Text::Bold);
    
    sf::FloatRect instructionTitleRect = instructionTitle.getLocalBounds();
    instructionTitle.setOrigin(instructionTitleRect.left + instructionTitleRect.width / 2.0f,
                              instructionTitleRect.top + instructionTitleRect.height / 2.0f);
    instructionTitle.setPosition(Config::WINDOW_WIDTH / 2.0f, 200);
    window.draw(instructionTitle);
    
    // 更简洁的说明文本
    std::vector<std::string> instructions = {
        "Welcome to Simple Runner with Particle Obstacles!",
        "",
        "Objective:",
        "- Avoid obstacles falling from top",
        "- Shoot obstacles with bullets to destroy them (+50 points)",
        "- Survive for high score",
        "",
        "Important:",
        "- You have ONLY 3 bullets for the entire game!",
        "- Use them wisely to destroy difficult obstacles",
        "",
        "Controls:",
        "- A / Left Arrow: Move left",
        "- D / Right Arrow: Move right",
        "- SPACE: Shoot bullets (max 3 for entire game)",
        "- ESC: Exit game",
        "- H: Toggle instructions",
        "- M: Return to menu",
        "- R: Restart after game over",
        "",
        "Obstacle Types:",
        "- Fire (Red): Fast with flames",
        "- Ice (Blue): Slow with ice",
        "- Electric (Purple): Medium with arcs",
        "- Poison (Green): Medium with cloud",
        "",
        "Difficulty:",
        "- Speed increases every 10 seconds",
        "- Obstacles get faster over time"
    };
    
    float yPos = 240;  // 从更低位置开始
    for (const auto& line : instructions) {
        sf::Text lineText(line, font, 16);  // 使用16号字体
        
        // 根据内容类型设置颜色
        if (line == "Objective:" || line == "Controls:" || line == "Obstacle Types:" || line == "Difficulty:") {
            lineText.setFillColor(sf::Color(100, 255, 100));
        } else if (line.find("Welcome") != std::string::npos) {
            lineText.setFillColor(sf::Color::White);
        } else if (line.find("- ") == 0) {
            lineText.setFillColor(sf::Color::White);
        } else {
            lineText.setFillColor(sf::Color(200, 200, 200));
        }
        
        lineText.setPosition(Config::WINDOW_WIDTH / 2.0f - 300, yPos); // 左边距加大
        
        // 绘制文本阴影效果
        sf::Text shadowText = lineText;
        shadowText.setFillColor(sf::Color(0, 0, 0, 150));
        shadowText.setPosition(lineText.getPosition().x + 2, lineText.getPosition().y + 2);
        window.draw(shadowText);
        
        window.draw(lineText);
        yPos += (line.empty() ? 10 : 22);  // 空行间距小，有内容行间距大
    }
    
    // 将"按任意键开始"下移，避免重叠
    if (blinkTimer < 0.5f) {
        pressAnyKeyText.setFillColor(sf::Color::White);
        window.draw(pressAnyKeyText);
    }
    else {
        pressAnyKeyText.setFillColor(sf::Color(255, 255, 255, 128));
        window.draw(pressAnyKeyText);
    }
    
    // 绘制键盘提示
    sf::Text keyHintText("Press any key to start game", font, 20);
    keyHintText.setFillColor(sf::Color(255, 200, 100));
    sf::FloatRect hintRect = keyHintText.getLocalBounds();
    keyHintText.setOrigin(hintRect.left + hintRect.width / 2.0f,
                         hintRect.top + hintRect.height / 2.0f);
    keyHintText.setPosition(Config::WINDOW_WIDTH / 2.0f, Config::WINDOW_HEIGHT - 50); // 放在底部
    window.draw(keyHintText);
    
    // 更新pressAnyKeyText位置，放在keyHintText上方
    pressAnyKeyText.setPosition(Config::WINDOW_WIDTH / 2.0f, Config::WINDOW_HEIGHT - 100);
    
    // 绘制版本信息
    sf::Text versionText("v1.0", font, 14);
    versionText.setFillColor(sf::Color(100, 100, 100));
    versionText.setPosition(Config::WINDOW_WIDTH - 50, Config::WINDOW_HEIGHT - 20);
    window.draw(versionText);
    
    // 绘制简单装饰线
    sf::RectangleShape topLine(sf::Vector2f(400, 2));
    topLine.setFillColor(sf::Color(100, 200, 255, 150));
    topLine.setPosition(Config::WINDOW_WIDTH / 2.0f - 200, 160);
    window.draw(topLine);
    
    sf::RectangleShape bottomLine(sf::Vector2f(400, 2));
    bottomLine.setFillColor(sf::Color(100, 200, 255, 150));
    bottomLine.setPosition(Config::WINDOW_WIDTH / 2.0f - 200, Config::WINDOW_HEIGHT - 120);
    window.draw(bottomLine);
    
    // 绘制粒子效果示例
    sf::CircleShape fireExample(8);
    fireExample.setFillColor(sf::Color(255, 100, 50));
    fireExample.setPosition(Config::WINDOW_WIDTH / 2.0f + 200, 320);
    window.draw(fireExample);
    
    sf::CircleShape iceExample(8);
    iceExample.setFillColor(sf::Color(100, 200, 255));
    iceExample.setPosition(Config::WINDOW_WIDTH / 2.0f + 200, 345);
    window.draw(iceExample);
    
    sf::CircleShape electricExample(8);
    electricExample.setFillColor(sf::Color(200, 100, 255));
    electricExample.setPosition(Config::WINDOW_WIDTH / 2.0f + 200, 370);
    window.draw(electricExample);
    
    sf::CircleShape poisonExample(8);
    poisonExample.setFillColor(sf::Color(100, 255, 100));
    poisonExample.setPosition(Config::WINDOW_WIDTH / 2.0f + 200, 395);
    window.draw(poisonExample);
}

void Game::drawGameInstructions() {
    if (currentState == GameState::Playing) {
        sf::RectangleShape background(sf::Vector2f(400, 160)); // 增加高度
        background.setFillColor(sf::Color(0, 0, 0, 180));
        background.setPosition(10, 10);
        window.draw(background);
        
        sf::Text titleText("Game Instructions (Press H to hide)", font, 18);
        titleText.setFillColor(sf::Color::Yellow);
        titleText.setPosition(20, 15);
        window.draw(titleText);
        
        sf::Text warningText("WARNING: You have ONLY 3 bullets for entire game!", 
                            font, 14);
        warningText.setFillColor(sf::Color::Red);
        warningText.setStyle(sf::Text::Bold);
        warningText.setPosition(20, 45);
        window.draw(warningText);
        
        sf::Text instructionText("Avoid the particle obstacles! Controls: A/D, Left/Right Arrow", 
                                font, 14);
        instructionText.setFillColor(sf::Color::White);
        instructionText.setPosition(20, 70);
        window.draw(instructionText);

        sf::Text bulletControlText("Press SPACE to shoot (3 bullets total, use wisely!)", 
                                font, 14);
        bulletControlText.setFillColor(sf::Color(100, 255, 255));
        bulletControlText.setPosition(20, 95);
        window.draw(bulletControlText);
        
        sf::Text obstacleText("Obstacle types: Fire(red) Ice(blue) Electric(purple) Poison(green)", 
                             font, 14);
        obstacleText.setFillColor(sf::Color::White);
        obstacleText.setPosition(20, 120);
        window.draw(obstacleText);
        
        sf::Text tipText("Tip: Save bullets for fast obstacles you can't dodge!", 
                        font, 14);
        tipText.setFillColor(sf::Color(255, 200, 100));
        tipText.setPosition(20, 145);
        window.draw(tipText);
    }
}

void Game::drawGameOverUI() {
    sf::RectangleShape overlay(sf::Vector2f(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);
    
    sf::Text gameOverText("GAME OVER!", font, 48);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setStyle(sf::Text::Bold);
    
    sf::FloatRect textRect = gameOverText.getLocalBounds();
    gameOverText.setOrigin(textRect.left + textRect.width / 2.0f,
                          textRect.top + textRect.height / 2.0f);
    gameOverText.setPosition(Config::WINDOW_WIDTH / 2.0f,
                            Config::WINDOW_HEIGHT / 2.0f - 80);
    window.draw(gameOverText);
    
    std::stringstream scoreStream;
    scoreStream << "Final Score: " << scoreSystem.getScore();
    sf::Text scoreText(scoreStream.str(), font, 32);
    scoreText.setFillColor(sf::Color::White);
    textRect = scoreText.getLocalBounds();
    scoreText.setOrigin(textRect.left + textRect.width / 2.0f,
                       textRect.top + textRect.height / 2.0f);
    scoreText.setPosition(Config::WINDOW_WIDTH / 2.0f,
                         Config::WINDOW_HEIGHT / 2.0f - 20);
    window.draw(scoreText);
    
    std::stringstream levelStream;
    levelStream << "Reached Speed Level: " << speedLevel;
    sf::Text levelText(levelStream.str(), font, 24);
    levelText.setFillColor(sf::Color::Yellow);
    textRect = levelText.getLocalBounds();
    levelText.setOrigin(textRect.left + textRect.width / 2.0f,
                       textRect.top + textRect.height / 2.0f);
    levelText.setPosition(Config::WINDOW_WIDTH / 2.0f,
                         Config::WINDOW_HEIGHT / 2.0f + 20);
    window.draw(levelText);
    
    // 游戏结束提示 - 分开显示更清晰
    sf::Text restartText("Press R to restart game", font, 20);
    restartText.setFillColor(sf::Color(100, 255, 100));
    textRect = restartText.getLocalBounds();
    restartText.setOrigin(textRect.left + textRect.width / 2.0f,
                         textRect.top + textRect.height / 2.0f);
    restartText.setPosition(Config::WINDOW_WIDTH / 2.0f,
                           Config::WINDOW_HEIGHT / 2.0f + 60);
    window.draw(restartText);
    
    sf::Text menuText("Press M to return to menu", font, 20);
    menuText.setFillColor(sf::Color(100, 200, 255));
    textRect = menuText.getLocalBounds();
    menuText.setOrigin(textRect.left + textRect.width / 2.0f,
                      textRect.top + textRect.height / 2.0f);
    menuText.setPosition(Config::WINDOW_WIDTH / 2.0f,
                        Config::WINDOW_HEIGHT / 2.0f + 90);
    window.draw(menuText);
    
    sf::Text exitText("Press ESC to exit game", font, 16);
    exitText.setFillColor(sf::Color(255, 200, 100));
    textRect = exitText.getLocalBounds();
    exitText.setOrigin(textRect.left + textRect.width / 2.0f,
                      textRect.top + textRect.height / 2.0f);
    exitText.setPosition(Config::WINDOW_WIDTH / 2.0f,
                        Config::WINDOW_HEIGHT / 2.0f + 120);
    window.draw(exitText);
}

void Game::drawUI() {
    scoreSystem.draw(window);
    
    if (currentState == GameState::Playing) {
        // ... 原有的速度信息显示 ...
        
        // 修改子弹信息显示
        std::stringstream bulletStream;
        bulletStream << "Bullets: " << player.getRemainingBullets() << "/" << 3;
        sf::Text bulletText(bulletStream.str(), font, 18);
        
        // 根据剩余子弹数改变颜色
        if (player.getRemainingBullets() == 0) {
            bulletText.setFillColor(sf::Color::Red);
        } else if (player.getRemainingBullets() == 1) {
            bulletText.setFillColor(sf::Color::Yellow);
        } else {
            bulletText.setFillColor(sf::Color::Cyan);
        }
        
        bulletText.setPosition(Config::WINDOW_WIDTH - 200, 120);
        window.draw(bulletText);
        
        // 显示警告信息（如果没有子弹了）
        if (player.getRemainingBullets() == 0) {
            sf::Text warningText("NO BULLETS LEFT!", font, 14);
            warningText.setFillColor(sf::Color::Red);
            warningText.setStyle(sf::Text::Bold);
            warningText.setPosition(Config::WINDOW_WIDTH - 200, 145);
            window.draw(warningText);
        }
        
        // ... 原有的冷却指示器 ...
    }
}

void Game::drawDebugInfo() {
    std::stringstream debugStream;
    debugStream << "Obstacles: " << obstacles.size();
    
    sf::Text debugText(debugStream.str(), font, 16);
    debugText.setFillColor(sf::Color::White);
    debugText.setPosition(10, Config::WINDOW_HEIGHT - 40);
    window.draw(debugText);
    
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
    
    std::stringstream timeStream;
    timeStream << "Time: " << static_cast<int>(scoreSystem.getTimeAlive()) << "s";
    
    sf::Text timeText(timeStream.str(), font, 16);
    timeText.setFillColor(sf::Color::White);
    timeText.setPosition(10, Config::WINDOW_HEIGHT - 60);
    window.draw(timeText);
}

void Game::startGame() {
    currentState = GameState::Playing;
    player.reset();
    obstacles.clear();
    scoreSystem.reset();
    obstacleSpawnTimer = 0.0f;
    resetDifficulty();
    showInstructions = true;
    
    std::cout << "===========================================" << std::endl;
    std::cout << "Game Started!" << std::endl;
    std::cout << "Controls: A/D, Left/Right Arrow to move" << std::endl;
    std::cout << "Press SPACE to shoot bullets (max 3)" << std::endl;
    std::cout << "Press H to toggle instructions" << std::endl;
    std::cout << "Press M to return to menu" << std::endl;
    std::cout << "===========================================" << std::endl;
}

void Game::updateDifficulty(float deltaTime) {
    speedIncreaseTimer += deltaTime;
    
    if (speedIncreaseTimer >= Config::SPEED_INCREASE_INTERVAL) {
        speedLevel++;
        
        currentObstacleSpeedMin += Config::SPEED_INCREASE_AMOUNT;
        currentObstacleSpeedMax += Config::SPEED_INCREASE_AMOUNT;
        
        if (currentObstacleSpeedMax > Config::MAX_OBSTACLE_SPEED) {
            currentObstacleSpeedMax = Config::MAX_OBSTACLE_SPEED;
            currentObstacleSpeedMin = std::min(currentObstacleSpeedMin, Config::MAX_OBSTACLE_SPEED - 50);
        }
        
        speedIncreaseTimer = 0.0f;
        
        std::cout << "===========================================" << std::endl;
        std::cout << "Speed increased! Level: " << speedLevel << std::endl;
        std::cout << "Obstacle speed range: " << currentObstacleSpeedMin 
                  << " - " << currentObstacleSpeedMax << std::endl;
        std::cout << "===========================================" << std::endl;
    }
}

void Game::spawnObstacle() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    std::uniform_real_distribution<float> xDist(
        Config::PARTICLE_OBSTACLE_RADIUS, 
        Config::WINDOW_WIDTH - Config::PARTICLE_OBSTACLE_RADIUS);
    
    std::uniform_real_distribution<float> speedDist(
        currentObstacleSpeedMin, currentObstacleSpeedMax);
    
    std::uniform_int_distribution<int> typeDist(0, 3);
    
    float x = xDist(gen);
    float speed = speedDist(gen);
    int typeIndex = typeDist(gen);
    
    ObstacleParticle::Type type;
    switch (typeIndex) {
        case 0: type = ObstacleParticle::Type::Fire; break;
        case 1: type = ObstacleParticle::Type::Ice; break;
        case 2: type = ObstacleParticle::Type::Electric; break;
        case 3: type = ObstacleParticle::Type::Poison; break;
        default: type = ObstacleParticle::Type::Fire; break;
    }
    
    obstacles.push_back(std::make_unique<ObstacleParticle>(x, -50, speed, type));
    
    static int obstacleCount = 0;
    obstacleCount++;
    if (obstacleCount % 5 == 0) {
        std::cout << "Spawned obstacle #" << obstacleCount 
                  << " (type: " << typeIndex << ", speed: " << speed << ", level: " << speedLevel << ")" << std::endl;
    }
}

void Game::resetDifficulty() {
    speedIncreaseTimer = 0.0f;
    currentObstacleSpeedMin = Config::OBSTACLE_SPEED_MIN;
    currentObstacleSpeedMax = Config::OBSTACLE_SPEED_MAX;
    speedLevel = 0;
}