#ifndef GAME_H
#define GAME_H

#include "Entity.h"
#include "EntityManager.h"

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"


struct PlayerConfig {
    int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, SE, SSE, PD;
    float S;
};
struct EnemyConfig {
    int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;
    float SMIN, SMAX;
};
struct BulletConfig {
    int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L;
    float S;
};


class Game {
    sf::RenderWindow m_window;              // the window we will draw to
    EntityManager m_entities;               // vector of entities to maintain
    EntityManager m_entitiesToAdd;          // vector of new entities to add
    sf::Font m_font;                        // the font we will use to draw
    PlayerConfig m_playerConfig;
    EnemyConfig m_enemyConfig;
    BulletConfig m_bulletConfig;
    sf::Clock m_deltaClock;
    std::shared_ptr<Entity> m_player;
    std::shared_ptr<Entity> m_shield;
    
    int m_textSize = 20;
    int m_textR = 0;
    int m_textG = 0;
    int m_textB = 255;    
    int m_score = 0;
    
    bool m_pause = false;
    bool m_running = true;
    bool m_isMovementActive = true;
    bool m_isCollisionActive = true;
    bool m_isSpawningActive = true;
    bool m_isLifespanActive = true;
    bool m_isShieldActive = false;
    
    unsigned int m_wWidth = 0;
    unsigned int m_wHeight = 0;
    unsigned int m_frameLimit = 60;
    unsigned int m_specialCooldownSec = 3;
    unsigned int m_shieldCooldownSec = 3;
    unsigned int m_shieldTimer = 10;
    unsigned int m_currentFrame = 0;
    unsigned int m_lastEnemySpawnTime = 0;
    unsigned int m_lastSpecialSpawnTime = 0;
    unsigned int m_lastShieldSpawnTime = 0;

    void init(const std::string &path);

    void sMovement();

    void sLifespan();

    void sUserInput();

    void sRender();

    void sEnemySpawner();

    void sGUI();

    void sCollision();

public:
    Game(const std::string &config);

    void update();
    void run();

    void setPaused();

    void spawnPlayer();

    void spawnEnemy();

    void spawnSmallEnemies(std::shared_ptr<Entity> e);

    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &target);
    
    void spawnSpreadBullet(std::shared_ptr<Entity> entity, const Vec2 &target);

    void spawnSpecialWeapon();

    void spawnShield();
};

#endif //GAME_H
