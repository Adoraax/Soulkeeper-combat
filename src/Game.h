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
    PlayerConfig        m_playerConfig;
    EnemyConfig         m_enemyConfig;
    BulletConfig        m_bulletConfig;
    Vec2                m_target = { 0, 0 };
    int                 m_score = 0;
    int                 m_currentFrame = 0;
    int                 m_lastEnemySpawnTime = 0;
    int                 m_bulletSpawnTime = 0;
    int                 m_enemySpeed = 0;
    bool                m_isChasing = true;
    bool                m_paused = false;   // whether we update game logic
    bool                m_running = true;   // whether the game is running
    bool                m_isSlowMotionApplied = false;
    float               m_deltatime;
    float               m_bulletSpeed = 17.0f;
    float               m_timeScale = 1.0f;
    float               m_decayRate = 2.0f;
    float               timeAdjustedDelta;
    float               multiplier;
    
    // Physics
    float               groundLimit;
    float               GRAVITY = 20.0f;    
    float               AIR_RESISTANCE = 0.995f;             // Air resistance factor
    float               GROUND_LEVEL;             // Ground level
    const float         BOUNCE_FACTOR = 0.4f;        // Energy loss during bounce
    const float         VELOCITY_THRESHOLD = 6.0f;  // Minimum velocity to stop bouncing
    const float         GROUND_FRICTION = 0.95f;            // Friction factor

    // Movement + User Input
    bool                m_isDashingLeft = false; 
    bool                m_isDashingRight = false; 
    bool                m_isMousePressed = false;
    bool                m_isJumping = false;
    bool                m_canDash = true;
    float               m_dashVelocity = 2.0f;              // Current dash velocity
    const float         DASH_DISTANCE = 500.0f;    // Total distance covered in a dash
    const float         DASH_DURATION = 0.25f;     // Time (in seconds) for a full dash
    const float         DOUBLE_TAP_THRESHOLD = 0.25f;
    const float         DASH_COOLDOWN = 1.0f;
    sf::Clock           m_mouseClock;
    sf::Clock           m_dashClock; 
    sf::Clock           m_dashCooldownClock;
    std::map<sf::Keyboard::Key, sf::Clock> m_keyPressClocks; // Track the time of the last key press
    std::map<sf::Keyboard::Key, bool> m_keyClockStarted;
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
    void spawnTrajectoryArc(std::shared_ptr<Entity> entity, const Vec2 & mousePos);
    void spawnBoundary();
    
    void slowMotion();
    void mapTrajectory();
    void chasePlayer();

public:
    Vec2 getPlayerPosition() const;  // get player's position

    Game(const std::string & config);       // constructor, takes in game config
    void run();
};