#ifndef BULLET_H
#define BULLET_H

#include <SFML/Graphics.hpp>

class Bullet {
public:
    Bullet(float x, float y);
    
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    
    bool isOffScreen() const;
    sf::FloatRect getBounds() const;
    
    bool isActive() const { return active; }
    void setActive(bool active) { this->active = active; }
    
    void triggerDestroyEffect();
    bool shouldRemove() const { return !active && destroyTimer >= 0.3f; }
    
private:
    sf::CircleShape shape;
    float speed;
    bool active;
    float destroyTimer;
    sf::Color originalColor;
};

#endif