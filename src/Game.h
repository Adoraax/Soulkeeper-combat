#pragma once

#include "Entity.h"
#include "EntityManager.h"

//#include <memory>
#include <SFML/Graphics.hpp>
#include <unordered_map>

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig  { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OT, V, L; float S; };

class Game
{
    sf::RenderWindow    m_window;           // the window we will draw to
    EntityManager       m_entities;         // vector of entities to maintain
    sf::Font            m_font;             // the font we will use to draw
    sf::Text            m_text;             // the score text to be drawn to the screen
    sf::Clock           m_clock;
    sf::Clock           m_mouseClock;
    std::map<sf::Keyboard::Key, sf::Clock> m_keyPressClocks; // Track the time of the last key press
    PlayerConfig        m_playerConfig;
    EnemyConfig         m_enemyConfig;
    BulletConfig        m_bulletConfig;
    Vec2                m_target = { 0, 0 };
    int                 m_score = 0;
    int                 m_currentFrame = 0;
    int                 m_lastEnemySpawnTime = 0;
    int                 m_bulletSpawnTime = 0;
    int                 m_enemySpeed = 0;
    bool m_isDashingLeft = false; 
    bool m_isDashingRight = false; 
    bool                m_isMousePressed = false;
    bool                m_isChasing = true;
    bool                m_paused = false;   // whether we update game logic
    bool                m_running = true;   // whether the game is running
    bool                m_isSlowMotionApplied = false;
    bool                m_isJumping = false;
    float               m_deltatime;
    float               m_bulletSpeed = 17.0f;
    float               m_timeScale = 1.0f;
    float               m_decayRate = 2.0f;
    float               groundLimit;
    float               GRAVITY = 20.0f;    
    float               AIR_RESISTANCE = 0.995f;             // Air resistance factor
    float               timeAdjustedDelta;
    //float               dragCoefficient = 0.005f;
    float               GROUND_LEVEL;             // Ground level
    const float         BOUNCE_FACTOR = 0.4f;        // Energy loss during bounce
    const float         VELOCITY_THRESHOLD = 6.0f;  // Minimum velocity to stop bouncing
    const float         GROUND_FRICTION = 0.95f;            // Friction factor
    const float         DOUBLE_TAP_THRESHOLD = 0.8f;
    
    std::unordered_map<sf::Keyboard::Key, bool> m_keyHeld;


    std::shared_ptr<Entity> m_player;

    void init(const std::string & config);  // initialize the GameState with a cofig file
    void setPaused(bool paused);            // pause the game

    void sMovement();
    void sUserInput();
    void sLifespan();
    void sRender();
    void sEnemySpawner();
    void sCollision();
    void sBoundarySpawner();
    void sApplyGravity();

    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> entity);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2 & mousePos);
    void spawnArrow(std::shared_ptr<Entity> entity, const Vec2 & mousePos);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity);
    void spawnBoundary();
    
    void slowMotion();

    void chasePlayer();

public:
    Vec2 getPlayerPosition() const;  // get player's position

    Game(const std::string & config);       // constructor, takes in game config
    void run();
};