#include "Game.h"

#include <iostream>
#include <fstream>
#include <cmath>

#define tick_rate 128
#define visual_frame_rate 60

Game::Game(const std::string & config)
{
    init(config);
}

void Game::init(const std::string & path)
{
    //std::ifstream fin(path);

    //fin >> m_playerConfig.SR >> m_playerConfig.CR;

    m_window.create(sf::VideoMode(3840, 1080), "Assignment 2");
    m_window.setFramerateLimit(tick_rate);
    GROUND_LEVEL = m_window.getSize().y - 2.0f;

    spawnPlayer();
    
}

void Game::run()
{
    // TODO: add pause functionality in here
    //       some systems should function while paused (rendering)
    //       some systems shouldn't (movement / input)
    
    while (m_running)
    {
        m_deltatime = m_clock.restart().asSeconds();
        multiplier = m_deltatime * visual_frame_rate;
        m_entities.update();
        //sEnemySpawner();
        sApplyGravity();
        sCollision();
        sUserInput();
        slowMotion();
        sMovement();
        sRender(); 
        //sLifespan();
        chasePlayer();
        //sBoundarySpawner();
        m_currentFrame++; 
    }
}

void Game::setPaused(bool paused)
{

}

Vec2 Game::getPlayerPosition() const
{
    if (m_player && m_player->cTransform)
    {
        return m_player->cTransform->pos;
    }
    return Vec2(0, 0);
}

// Spawn a player entity
void Game::spawnPlayer()
{
    auto entity = m_entities.addEntity("player");

    float mx = m_window.getSize().x / 2.0f - (m_window.getSize().x / 2.0f)/2;
    float my = m_window.getSize().y / 2.0f - (m_window.getSize().y / 2.0f)/-2;

    entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(0.0f, 0.0f), 0.0f, 1.0f);
    entity->cShape = std::make_shared<CShape>(sf::Vector2f(90.0f, 180.0f), sf::Color(0, 0, 0), sf::Color(255, 0, 0), 2.0f);

    entity->cInput = std::make_shared<CInput>();

    m_player = entity;
    m_isMelee = true;
    spawnPlayerArm();
    spawnPlayerElbow();
    spawnPlayerForearm();
    //spawnSword();
}

void Game::spawnPlayerForearm()
{
    // Create a new entity for the arm
    auto forearm = m_entities.addEntity("forearm");

    // Set the sword's transform component (position, velocity, angle, speed)
    forearm->cTransform = std::make_shared<CTransform>(
        m_player->cTransform->pos,  // Start position: same as player's position
        Vec2(0.0f, 0.0f),           // Initial velocity
        0.0f,                       // Initial angle
        0.0f                        // Initial speed
    );

    // Set the sword's shape component (rectangle with outline)
    forearm->cShape = std::make_shared<CShape>(
        sf::Vector2f(45.0f, 90.0f), // Sword size
        sf::Color::Black,              // Fill color
        sf::Color::Red,            // Outline color
        1.0f                         // Outline thickness
    );
}

void Game::spawnPlayerElbow()
{
    // Create a new entity for the elbow
    auto elbow = m_entities.addEntity("elbow");

    // Set the sword's transform component (position, velocity, angle, speed)
    elbow->cTransform = std::make_shared<CTransform>(
        m_player->cTransform->pos,  // Start position: same as player's position
        Vec2(0.0f, 0.0f),           // Initial velocity
        0.0f,                       // Initial angle
        0.0f                        // Initial speed
    );

    // Set the sword's shape component (rectangle with outline)
    elbow->cShape = std::make_shared<CShape>(
        15.0f,
        8,
        sf::Color::Black,              // Fill color
        sf::Color::Red,            // Outline color
        1.0f                         // Outline thickness
    );
}

void Game::spawnPlayerArm()
{
    // Create a new entity for the arm
    auto arm = m_entities.addEntity("arm");

    // Set the sword's transform component (position, velocity, angle, speed)
    arm->cTransform = std::make_shared<CTransform>(
        m_player->cTransform->pos,  // Start position: same as player's position
        Vec2(0.0f, 0.0f),           // Initial velocity
        0.0f,                       // Initial angle
        0.0f                        // Initial speed
    );

    // Set the sword's shape component (rectangle with outline)
    arm->cShape = std::make_shared<CShape>(
        sf::Vector2f(90.0f, 22.5f), // Sword size
        sf::Color::Black,              // Fill color
        sf::Color::Red,            // Outline color
        1.0f                         // Outline thickness
    );
}

// Spawn an enemy entity
void Game::spawnEnemy()
{
    auto entity = m_entities.addEntity("enemy");

//    float ex = rand() % (m_window.getSize().x);
//    float ey = rand() % (m_window.getSize().y);

    float ex = rand() % (m_window.getSize().x / 2) + (m_window.getSize().x / 2);
    float ey = rand() % (m_window.getSize().y / 2) + (m_window.getSize().y / 2);

    int r = (1 + rand() % 255);
    int g = (1 + rand() % 255);
    int b = (1 + rand() % 255);

    int randVertices = (3 + rand() % 8);

    float randRadius = static_cast<float>(16 + rand() % 45);

    // Ensure the enemy does not spawn on the player
    float playerX = m_player->cTransform->pos.x;
    float playerY = m_player->cTransform->pos.y;
    float playerRadius = m_player->cShape->circle.getRadius() + 60.0f;
    Vec2 enemyPosition(ex, ey);
    float enemySpeed = (100.0f / randRadius)/1.5; // Calculate speed based on radius
    while (std::abs(ex - playerX) < playerRadius && std::abs(ey - playerY) < playerRadius)
    {
        ex = rand() % m_window.getSize().x;
        ey = rand() % m_window.getSize().y;
        enemyPosition = Vec2(ex, ey);
    }

    Vec2 direction = (m_player->cTransform->pos - enemyPosition).normalize();
    entity->cTransform = std::make_shared<CTransform>(enemyPosition, direction * enemySpeed, 0.0f, enemySpeed);
    entity->cShape = std::make_shared<CShape>(randRadius, randVertices, sf::Color(r, g, b), sf::Color(255, 255, 255), 4.0f);

    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2 & target)
{
    auto bullet = m_entities.addEntity("bullet");

    Vec2 direction = (m_target - m_player->cTransform->pos).normalize();

    bullet->cTransform = std::make_shared<CTransform>(m_player->cTransform->pos, direction * m_bulletSpeed, 0.0f, 0.0f);
    bullet->cShape = std::make_shared<CShape>(10, 8, sf::Color(0, 0, 0), sf::Color(255, 255, 255), 2);
    bullet->cLifespan = std::make_shared<CLifespan>(60);

    m_bulletSpawnTime = m_currentFrame;
}


void Game::spawnArrow(std::shared_ptr<Entity> entity, const Vec2 & target)
{
    auto arrow = m_entities.addEntity("arrow");

    Vec2 direction = (m_target - m_player->cTransform->pos).normalize();
    
    arrow->cTransform = std::make_shared<CTransform>(m_player->cTransform->pos, direction * m_bulletSpeed, 0.0f, 0.0f);
    arrow->cShape = std::make_shared<CShape>(sf::Vector2f(70.0f, 15.0f), sf::Color(0, 0, 0), sf::Color(255, 255, 255), 2.0f);
    arrow->cShape->rectangle.setOrigin(arrow->cShape->rectangle.getSize().x / 2.0f, arrow->cShape->rectangle.getSize().y / 2.0f);

    m_bulletSpawnTime = m_currentFrame;
}

// void Game::spawnSword(std::shared_ptr<Entity> entity, const Vec2 & target)
// {
//     auto sword = m_entities.addEntity("sword");

//     Vec2 direction = (m_target - m_player->cTransform->pos).normalize();
//     float playerPositionX = m_player->cTransform->pos.x;
//     float playerPositionY = m_player->cTransform->pos.y;
//     Vec2 swordPosition(playerPositionX + m_player->cShape->rectangle.getSize().x/2.0f, playerPositionY - m_player->cShape->rectangle.getSize().y/2.0f);
//     sword->cTransform = std::make_shared<CTransform>(Vec2(swordPosition), Vec2(0,0), -40.0f, 0.0f);
//     sword->cShape = std::make_shared<CShape>(sf::Vector2f(45.0f, 240.0f), sf::Color(0, 0, 0), sf::Color(255, 255, 255), 2.0f);
//     sword->cShape->rectangle.setOrigin(sword->cShape->rectangle.getSize().x / 2.0f, sword->cShape->rectangle.getSize().y);
// }

void Game::spawnSword()
{
    // Create a new entity for the sword
    auto sword = m_entities.addEntity("sword");

    // Set the sword's transform component (position, velocity, angle, speed)
    sword->cTransform = std::make_shared<CTransform>(
        m_player->cTransform->pos,  // Start position: same as player's position
        Vec2(0.0f, 0.0f),           // Initial velocity
        0.0f,                       // Initial angle
        0.0f                        // Initial speed
    );

    // Set the sword's shape component (rectangle with outline)
    sword->cShape = std::make_shared<CShape>(
        sf::Vector2f(150.0f, 10.0f), // Sword size
        sf::Color::Black,              // Fill color
        sf::Color::Green,            // Outline color
        1.0f                         // Outline thickness
    );

}



void Game::holsterWeapon()
{
    m_isHolstered = true;
}

// TODO: FIX THIS FUNCTION
void Game::spawnBoundary()
{
    auto entity = m_entities.addEntity("wall");

    // Calculate the size and position of the boundary
    float boundaryWidth = 200.0f; // Full width of the screen
    float boundaryHeight = m_window.getSize().y / 3.0f; // 1/4 the height of the screen
    float boundaryPosX = m_window.getSize().x/2 - boundaryWidth * 2; // Start from the left edge of the screen
    float boundaryPosY = m_window.getSize().y - boundaryHeight/2 - 4.0f; // Touch the bottom of the screen

    // Set the transform and shape of the boundary entity
    entity->cTransform = std::make_shared<CTransform>(Vec2(boundaryPosX, boundaryPosY), Vec2(0.0f, 0.0f), 0.0f, 0.0f);
    entity->cShape = std::make_shared<CShape>(
        sf::Vector2f(boundaryWidth, boundaryHeight), // Size of the rectangle
        sf::Color(0, 0, 0), // Fill color
        sf::Color(255, 0, 0), // Outline color
        4.0f // Outline thickness
    );

    // Set the position of the rectangle shape
    entity->cShape->rectangle.setPosition(boundaryPosX, boundaryPosY);
}

void Game::sBoundarySpawner()
{
    spawnBoundary();
}


void Game::slowMotion()
{
    m_isSlowMotionApplied = false;
    if (m_isMousePressed)
    {
        //std::cout << "slowMotion" << "\n";
        for (auto& e : m_entities.getEntities())
        {
            if (e->tag() != "player" && e->cTransform)
            {
                e->cTransform->pos += (e->cTransform->velocity * m_deltatime * multiplier)/3.0f; // Scale velocity
            }
        }
        m_isSlowMotionApplied = true;
    }
}

void Game::sLifespan()
{
    for (auto& e : m_entities.getEntities())
    {
        if (e && e->cLifespan && e->cShape)
        {
            // Calculate total lifespan duration
            float totalLifespan = e->cLifespan->total;
            float remainingLifespan = e->cLifespan->remaining;

            // Calculate the color decrement per frame
            if (totalLifespan > 0)
            {
                // Determine the decrement per frame
                float fadeDecrementPerFrame = 255.0f / totalLifespan;

                sf::Color fillColor = e->cShape->circle.getFillColor();
                float newR = std::max(0.0f, fillColor.r - fadeDecrementPerFrame);
                float newG = std::max(0.0f, fillColor.g - fadeDecrementPerFrame);
                float newB = std::max(0.0f, fillColor.b - fadeDecrementPerFrame);

                e->cShape->circle.setFillColor(sf::Color(static_cast<sf::Uint8>(newR),
                                                         static_cast<sf::Uint8>(newG),
                                                         static_cast<sf::Uint8>(newB)));

                if (e->isActive())
                {
                    e->cLifespan->remaining--;

                    // Destroy the entity when lifespan ends
                    if (e->cLifespan->remaining == 0)
                    {
                        e->destroy();
                    }
                }
            }
        }
    }
}


// Collision detection
void Game::sCollision()
{
    // enemy collision with player
    for (auto& enemy : m_entities.getEntities("enemy"))
    {
        for (auto& player : m_entities.getEntities("player"))
        {
            float dx = m_player->cTransform->pos.x - enemy->cTransform->pos.x;
            float dy = m_player->cTransform->pos.y - enemy->cTransform->pos.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance < (m_player->cShape->circle.getRadius() + enemy->cShape->circle.getRadius())) 
            {
                //std::cout << "collision\n";
                float mx = m_window.getSize().x / 2.0f;
                float my = m_window.getSize().y / 2.0f;
                player->cTransform->pos.x = mx;
                player->cTransform->pos.y = my;
                enemy->destroy();
            }
        }
    }

    // bullet collision with enemy
    for (auto& bullet : m_entities.getEntities("bullet"))
    {
        for (auto& enemy : m_entities.getEntities("enemy"))
        {
            float dx = bullet->cTransform->pos.x - enemy->cTransform->pos.x;
            float dy = bullet->cTransform->pos.y - enemy->cTransform->pos.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance < (bullet->cShape->circle.getRadius() + enemy->cShape->circle.getRadius())) 
            {
                //std::cout << "enemy destroyed\n";
                enemy->destroy();
                bullet->destroy();
            }
        }
    }

    // Arrow collision with enemy -- safer implementation
    for (auto& arrow : m_entities.getEntities("arrow"))
    {
        if (!arrow->cTransform || !arrow->cShape)
        {
            continue;
        }
        for (auto& enemy : m_entities.getEntities("enemy"))
        {
            if (!enemy->cTransform || !enemy->cShape)
            {
                continue;
            }

            // Calculate the distance between the arrow and the enemy
            float dx = arrow->cTransform->pos.x - enemy->cTransform->pos.x;
            float dy = arrow->cTransform->pos.y - enemy->cTransform->pos.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            // Check collision based on shape dimensions
            float arrowWidth = arrow->cShape->rectangle.getSize().x;
            float enemyRadius = enemy->cShape->circle.getRadius();

            if (distance < (arrowWidth + enemyRadius))
            {
                // Destroy the enemy and the arrow
                enemy->destroy();
                arrow->destroy();

                // Break out of the inner loop to avoid accessing a destroyed arrow
                break;
            }
        }
    }


    //enemy collision with boundary
    for (auto& enemy : m_entities.getEntities("enemy"))
    {
        for (auto& wall : m_entities.getEntities("wall"))
        {
            float dx = wall->cTransform->pos.x - enemy->cTransform->pos.x;
            float dy = wall->cTransform->pos.y - enemy->cTransform->pos.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance < (wall->cShape->rectangle.getSize().x/2 + enemy->cShape->circle.getRadius()*2)) 
            {
                m_isChasing = false;
                enemy->cTransform->velocity = Vec2(0.0f, 0.0f);
            }
        }
    }

    //arrow collision with arrow
    for (auto& arrow1 : m_entities.getEntities("arrow"))
    {
        // Ensure the first arrow has the required components
        if (!arrow1->cTransform || !arrow1->cShape)
        {
            continue;
        }

        for (auto& arrow2 : m_entities.getEntities("arrow"))
        {
            // Skip self-collision
            if (arrow1 == arrow2) continue;

            // Ensure the second arrow has the required components
            if (!arrow2->cTransform || !arrow2->cShape)
            {
                continue;
            }

            // Calculate the distance between the two arrows
            float dx = arrow1->cTransform->pos.x - arrow2->cTransform->pos.x;
            float dy = arrow1->cTransform->pos.y - arrow2->cTransform->pos.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            // Get the dimensions of the arrows for collision detection
            float arrow1Width = arrow1->cShape->rectangle.getSize().x;
            float arrow2Width = arrow2->cShape->rectangle.getSize().x;

            // Check for collision
            if (distance < (arrow1Width + arrow2Width)/2)
            {
                if (arrow1->cTransform->velocity == Vec2(0, 0))
                {
                    // arrow2->cTransform->velocity = Vec2(0, 0);
                    // arrow2->cTransform->pos.y = groundLimit;
                    continue;
                }
                else if (arrow2->cTransform->velocity == Vec2(0, 0))
                {
                    // arrow1->cTransform->velocity = Vec2(0, 0);
                    // arrow1->cTransform->pos.y = groundLimit;
                    continue;
                }
                else
                {
                    float reflectionArrow1VelocityY = ((arrow1->cTransform->velocity.y + arrow2->cTransform->velocity.y) / 2.0f) * 1.5;
                    float reflectionArrow1VelocityX = ((arrow1->cTransform->velocity.x + arrow2->cTransform->velocity.x) / 2.0f) * 1.5;
                    float reflectionArrow2VelocityY = ((arrow2->cTransform->velocity.y + arrow2->cTransform->velocity.y) / 2.0f) * 0.5;
                    float reflectionArrow2VelocityX = ((arrow2->cTransform->velocity.x + arrow2->cTransform->velocity.x) / 2.0f) * 0.5;

                    arrow1->cTransform->velocity = Vec2(reflectionArrow1VelocityX, reflectionArrow1VelocityY);
                    arrow2->cTransform->velocity = Vec2(reflectionArrow2VelocityX, reflectionArrow2VelocityY);
                }
                

                break;
            }
        }
    }


}

// Spawn enemies
void Game::sEnemySpawner()
{
    int framesPerSpawn = 60;
    if (m_currentFrame - m_lastEnemySpawnTime >= framesPerSpawn)
    {
        spawnEnemy();
    }

}

// Cause enemies to move towards player
void Game::chasePlayer()
{
    Vec2 newDirection;
    if (m_isChasing == true)
    {
        for (auto& enemy : m_entities.getEntities("enemy"))
        {
            newDirection = (m_player->cTransform->pos - enemy->cTransform->pos).normalize();
            enemy->cTransform->velocity = newDirection * enemy->cTransform->speed;
        }

        for (auto& enemy : m_entities.getEntities("enemy"))
        {
            if (enemy->cTransform)
            {
                enemy->cTransform->pos += enemy->cTransform->velocity;
            }
        }
    }
}

// Apply gravity to all entities with a transform component
void Game::sApplyGravity()
{
    //std::cout << "sApplyGravity "<< "\n";
    for (auto& e : m_entities.getEntities())
    {
        // Ensure the entity has both transform and shape components
        if (e->cTransform && e->cShape)
        {
            // Apply gravity to Y velocity
            if (e->tag() == "player" && m_isSlowMotionApplied)
            {
                e->cTransform->velocity.y += ((GRAVITY * m_deltatime * multiplier) /3);
            }
            else if (e->tag() == "player" && !m_isSlowMotionApplied)
            {
                e->cTransform->velocity.y += (GRAVITY * m_deltatime) * 1.75f;
            }
            
            else if (e->tag() == "enemy")
            {
                e->cTransform->velocity.y += GRAVITY * m_deltatime;
            }
            else if (e->tag() == "bullet")
            {
                e->cTransform->velocity.y += GRAVITY * m_deltatime;
            }
            else if (e->tag() == "arrow" && m_isSlowMotionApplied)
            {
                e->cTransform->velocity.y += ((GRAVITY * m_deltatime * multiplier) /3);
            }
            else if (e->tag() == "arrow" && !m_isSlowMotionApplied)
            {
                e->cTransform->velocity.y += (GRAVITY * m_deltatime);
            }
            else if (e->tag() == "sword")
            {
                e->cTransform->velocity.y += GRAVITY * m_deltatime * 5.0f;
            }

            // Handle X-axis deceleration
            if (std::abs(e->cTransform->velocity.x) > 1.0f)
            {
                if (e->cTransform->pos.y < groundLimit)
                {

                }
                else
                {
                    if (e->tag() == "player")
                    {
                        e->cTransform->velocity.x *= GROUND_FRICTION;
                    }
                    else if (e->tag() == "enemy")
                    {
                        e->cTransform->velocity.x *= GROUND_FRICTION; // Ground friction
                    }
                    else if (e->tag() == "bullet")
                    {
                        e->cTransform->velocity.x *= GROUND_FRICTION; // Ground friction
                    }
                    else if (e->tag() == "arrow")
                    {
                        e->cTransform->velocity.x *= GROUND_FRICTION; // Ground friction
                    }
                    else if (e->tag() == "sword")
                    {
                        e->cTransform->velocity.x *= GROUND_FRICTION; // Ground friction
                    }
                }
            }
            else
            {
                if (e->tag() == "player")
                {
                    e->cTransform->velocity.x = 0.0f; // Stop X motion if below threshold
                }
                else if (e->tag() == "enemy")
                {
                    e->cTransform->velocity.x = 0.0f; // Stop X motion if below threshold
                }
                else if (e->tag() == "bullet")
                {
                    e->cTransform->velocity.x = 0.0f; // Stop X motion if below threshold
                }
                else if (e->tag() == "arrow")
                {
                    e->cTransform->velocity.x = 0.0f; // Stop X motion if below threshold
                }
            }

            groundLimit = GROUND_LEVEL - 400.0f;
            if (e->tag() == "player")
            {
                groundLimit -= e->cShape->rectangle.getSize().y / 2.0f;
            }
            else if (e->tag() == "enemy")
            {
                groundLimit -= e->cShape->circle.getRadius();
            }
            else if (e->tag() == "bullet")
            {
                groundLimit -= e->cShape->circle.getRadius();
            }
            else if (e->tag() == "arrow")
            {
                groundLimit -= e->cShape->rectangle.getSize().y / 2.0f;
            }

            // Check for collision with the ground
            if (e->cTransform->pos.y >= groundLimit)
            {
                // Snap the entity to just above the ground
                e->cTransform->pos.y = groundLimit;

                // Bounce or stop based on velocity
                if (std::abs(e->cTransform->velocity.y) >= VELOCITY_THRESHOLD)
                {
                    if (e->tag() == "player")
                    {
                        e->cTransform->velocity.y = 0.0f;
                    }
                    else if (e->tag() == "enemy")
                    {
                        e->cTransform->velocity.y = -e->cTransform->velocity.y * BOUNCE_FACTOR;
                    }
                    else if (e->tag() == "bullet")
                    {
                        e->cTransform->velocity.y = -e->cTransform->velocity.y * BOUNCE_FACTOR;
                    }
                    else if (e->tag() == "arrow")
                    {
                        e->cTransform->velocity.y = -e->cTransform->velocity.y * BOUNCE_FACTOR;
                    }
                    else if (e->tag() == "sword")
                    {
                        e->cTransform->velocity.y = -e->cTransform->velocity.y * BOUNCE_FACTOR;
                    }
                }
                else
                {
                    if (e->tag() == "player")
                    {
                        e->cTransform->velocity.y = 0.0f; // Stop Y motion
                    }
                    else if (e->tag() == "enemy")
                    {
                        e->cTransform->velocity.y = 0.0f; // Stop Y motion
                    }
                    else if (e->tag() == "bullet")
                    {
                        e->cTransform->velocity.y = 0.0f; // Stop Y motion
                    }
                    else if (e->tag() == "arrow")
                    {
                        e->cTransform->velocity.y = 0.0f; // Stop Y motion
                    }
                }

                // Additional slowing logic for low velocities
                if (std::abs(e->cTransform->velocity.x) < 12.0f && std::abs(e->cTransform->velocity.x) > 1.0f)
                {
                    if (e->tag() == "player")
                    {
                        e->cTransform->velocity.x *= (GROUND_FRICTION * 0.8f);
                    }
                    else if (e->tag() == "enemy")
                    {
                        e->cTransform->velocity.x *= (GROUND_FRICTION * 0.8f);
                    }
                    else if (e->tag() == "bullet")
                    {
                        e->cTransform->velocity.x *= (GROUND_FRICTION * 0.8f);
                    }
                    else if (e->tag() == "arrow")
                    {
                        e->cTransform->velocity.x *= (GROUND_FRICTION * 0.8f);
                    }
                }
            }
            // Update position based on velocity
            e->cTransform->pos += ((e->cTransform->velocity * multiplier)/3);

            // Additional logic for arrows
            if (e->tag() == "arrow")
            {
                // angle of velocity vector
                float angleRad = std::atan2(e->cTransform->velocity.y, e->cTransform->velocity.x);
                float angleDeg = angleRad * 180.0f / M_PI;

                // arrows rotation to the calculated angle
                e->cShape->rectangle.setRotation(angleDeg);
                if (e->cTransform->pos.y < groundLimit)
                {
                    //std::cout << "Arrow angle: " << angleDeg << "\n";
                }
            }
        }
    }
}


// sRender -- safer implementation
void Game::sRender()
{
    m_window.clear();

    for (auto& e : m_entities.getEntities())
    {
        // Ensure the entity has the required components before rendering
        if (e->cShape && e->cTransform)
        {
            // Update positions of shapes based on the transform
            e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
            e->cShape->rectangle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

            // Render shapes only if they are valid
            if (e->cShape->circle.getRadius() > 0)
            {
                m_window.draw(e->cShape->circle);
            }
            if (e->cShape->rectangle.getSize().x > 0 && e->cShape->rectangle.getSize().y > 0)
            {
                m_window.draw(e->cShape->rectangle);
            }
        }
    }

    m_window.display();
}


void Game::sMovement()
{

    float elapsed = m_dashClock.getElapsedTime().asSeconds();
    
    if (m_isSlowMotionApplied)
    {
        DASH_DURATION = 0.75f / multiplier;
    }
    else
    {
        DASH_DURATION = 0.5f;
    }

    if (elapsed < DASH_DURATION)
    {
        float dashSpeed = (m_isDashingRight ? DASH_DISTANCE : -DASH_DISTANCE) / DASH_DURATION;

        
        m_player->cTransform->velocity.x = (dashSpeed * multiplier)/3;
        
        m_player->cShape->rectangle.setSize(sf::Vector2f(180.0f, 90.0f));
        m_player->cShape->rectangle.setOrigin(90.0f, 45.0f);

        if (m_player->cTransform->pos.y <= groundLimit)
        {
            // Player is on the ground, dash along the ground
            m_player->cTransform->velocity.y = 0;
            
            m_player->cTransform->pos.y = GROUND_LEVEL - m_player->cShape->rectangle.getSize().y / 2.0f;
        }
    }
    else
    {
        // End dash
        m_isDashingRight = false;
        m_isDashingLeft = false;
        m_player->cTransform->velocity.x = 0; // Reset velocity
        m_player->cShape->rectangle.setSize(sf::Vector2f(90.0f, 180.0f));
        m_player->cShape->rectangle.setOrigin(45.0f, 90.0f);
    }

    // Regular player movement (only if not dashing)
    if (!m_isDashingLeft && !m_isDashingRight)
    {
        if (m_player->cInput->up && !m_isJumping)
        {
            if (m_player->cTransform->pos.y >= GROUND_LEVEL - m_player->cShape->rectangle.getSize().y / 2.0f)
            {
                m_player->cTransform->velocity.y = -10;
                m_isJumping = true;
            }
        }

        if (m_player->cInput->left)
        {
            m_player->cTransform->velocity.x = -6;
        }
        else if (m_player->cInput->right)
        {
            m_player->cTransform->velocity.x = 6;
        }
        else
        {
            m_player->cTransform->velocity.x = 0;
        }
    }

    // Apply velocity updates
    if (!m_isDashingLeft && !m_isDashingRight)
    {
        if (!m_isSlowMotionApplied)
        {
            m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
            m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
        }
        else
        {
            m_player->cTransform->pos.x += (m_player->cTransform->velocity.x * multiplier)/3;
            m_player->cTransform->pos.y += (m_player->cTransform->velocity.y * multiplier)/3;
        }
    }

    for (auto& arm : m_entities.getEntities("arm"))
    {
        for (auto& elbow : m_entities.getEntities("elbow"))
        {
            // 1. Anchor the arm's base to the player's position
            sf::Vector2f playerPos(m_player->cTransform->pos.x, m_player->cTransform->pos.y);

            // 2. Calculate the direction and distance between the player and the elbow
            sf::Vector2f direction(elbow->cTransform->pos.x - playerPos.x, elbow->cTransform->pos.y - playerPos.y); // Vector from player to elbow
            float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y); // Distance between player and elbow
            float angle = std::atan2(direction.y, direction.x) * 180.0f / M_PI; // Convert to degrees

            // 3. Set the arm's position and rotation
            arm->cTransform->pos = playerPos; // Base of the arm
            arm->cShape->rectangle.setPosition(playerPos); // Set base position
            arm->cShape->rectangle.setRotation(angle);     // Rotate to face elbow

            // 4. Adjust the origin to the base (bottom-left of the arm)
            arm->cShape->rectangle.setSize(sf::Vector2f(90.0f, 22.5f)); // Arm dimensions (length, thickness)
            arm->cShape->rectangle.setOrigin(0.0f, 11.25f); // Origin at the base: bottom-left

            sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
            sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
            sf::Vector2f elbowPos(elbow->cTransform->pos.x, elbow->cTransform->pos.y);
            sf::Vector2f directionToMouse = mousePosF - elbowPos;
            float distanceToMouse = std::sqrt(directionToMouse.x * directionToMouse.x + directionToMouse.y * directionToMouse.y);
            //normalize the direction
            directionToMouse /= distanceToMouse;
            float moveSpeed = 15.0f;
            elbow->cTransform->pos += directionToMouse * moveSpeed;

            // 5. Update the elbow's position based on the arm's length
            float armLength = 90.0f;
            sf::Vector2f newDirection = elbowPos - playerPos;
            float newDistance = std::sqrt(newDirection.x * newDirection.x + newDirection.y * newDirection.y);

            // attempt 1
            // sf::Vector2f directionME(mousePos.x - elbowPos.x, mousePos.x - elbowPos.x);
            // float distanceME = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            // elbow->cTransform->pos = playerPos + sf::Vector2f(std::cos(angle * M_PI / 180.0f) * armLength,
            //                                                 std::sin(angle * M_PI / 180.0f) * armLength); 

            // elbow->cShape->circle.setPosition(sf::Vector2f(elbow->cTransform->pos.x, elbow->cTransform->pos.y));
            // elbow->cTransform->velocity.x = directionME.x / distanceME * 5;
            // elbow->cTransform->velocity.y = directionME.y / distanceME * 5;


            // attempt 2
            // if (newDistance > armLength)
            // {
            //     newDirection /= newDistance; // Normalize the direction vector
            //     elbow->cTransform->pos = playerPos + newDirection * armLength;
            // }
            
            elbow->cShape->circle.setPosition(sf::Vector2f(elbow->cTransform->pos.x, elbow->cTransform->pos.y));
        }
    }

    for (auto& forearm : m_entities.getEntities("forearm"))
    {
        for (auto& elbow : m_entities.getEntities("elbow"))
        {
            if (forearm->cTransform)
            {
                // 1. Anchor the forearm's base to the elbow's position
                sf::Vector2f elbowPos(elbow->cTransform->pos.x, elbow->cTransform->pos.y);

                // 2. Get mouse position relative to the window
                sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                // 3. Calculate the angle between the elbow and the mouse
                sf::Vector2f direction = mousePosF - elbowPos; // Vector from elbow to mouse
                float angle = atan2(direction.y, direction.x) * 180.0f / M_PI; // Convert to degrees

                // 4. Set the forearm's position and rotation
                forearm->cTransform->pos = elbowPos; // Base of the forearm
                forearm->cShape->rectangle.setPosition(elbowPos); // Set base position
                forearm->cShape->rectangle.setRotation(angle);     // Rotate to face mouse

                // 5. Adjust the origin to the base (bottom-left of the forearm)
                forearm->cShape->rectangle.setSize(sf::Vector2f(90.0f, 22.5f)); // Forearm dimensions (length, thickness)
                forearm->cShape->rectangle.setOrigin(0.0f, 11.25f); // Origin at the base: bottom-left

                // Note: SetOrigin(0.0f, center Y) ensures rotation occurs at the forearm's base.
            }
        }
    }

    for (auto& sword : m_entities.getEntities("sword"))
    {
        for (auto& forearm : m_entities.getEntities("forearm"))
        {
            if (sword->cTransform && forearm->cTransform)
            {
                // 1. Calculate the arm's tip position
                sf::Vector2f armBasePos(forearm->cTransform->pos.x, forearm->cTransform->pos.y); // Base of the arm
                float armLength = 90.0f; // Length of the arm
                float armAngleRad = forearm->cShape->rectangle.getRotation() * M_PI / 180.0f; // Convert angle to radians

                // Tip position using trigonometry
                sf::Vector2f armTipPos = armBasePos + sf::Vector2f(
                    armLength * std::cos(armAngleRad),
                    armLength * std::sin(armAngleRad)
                );
                // Sword hilt position (anchored to player)
                sf::Vector2f hiltPos(armTipPos);

                // Update the sword tip's position
                // if (!m_isSlowMotionApplied)
                // {
                //     sword->cTransform->pos += sword->cTransform->velocity * m_deltatime;
                // }
                // else if (m_isSlowMotionApplied)
                // {
                //     sword->cTransform->pos += sword->cTransform->velocity;
                // }
                

                // Prevent the tip of the sword from going below ground level
                if (sword->cTransform->pos.y > GROUND_LEVEL)
                {
                    sword->cTransform->pos.y = GROUND_LEVEL;
                    sword->cTransform->velocity.y = 0;
                }

                // Calculate the direction vector from the hilt to the tip
                sf::Vector2f direction(sword->cTransform->pos.x - hiltPos.x, sword->cTransform->pos.y - hiltPos.y);

                // Constrain the tip's distance from the hilt (fixed-length sword)
                float swordLength = 280.0f; // Total length of the sword
                float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

                if (distance > swordLength)
                {
                    // Normalize the direction and clamp the tip's position to the sword length
                    direction /= distance; // Normalize the direction vector
                    sword->cTransform->pos = hiltPos + direction * swordLength;
                }

                // Calculate the angle of the sword based on the hilt and tip positions
                float angle = atan2(direction.y, direction.x) * 180.0f / M_PI;

                // Update the sword's position and rotation
                sword->cShape->rectangle.setPosition(hiltPos); // Set the hilt position
                sword->cShape->rectangle.setRotation(angle);   // Rotate the sword to match the direction
                sword->cShape->rectangle.setSize(sf::Vector2f(swordLength, 45.0f)); // Fixed size
                sword->cShape->rectangle.setOrigin(280.0f, 22.5f); // Hilt is the origin (base of the sword)
            }
        }
    }



    // Bullet movement
    for (auto& bullet : m_entities.getEntities("bullet"))
    {
        if (bullet->cTransform)
        {
            bullet->cTransform->pos += bullet->cTransform->velocity;
        }
    }

    // Arrow movement
    for (auto& arrow : m_entities.getEntities("arrow"))
    {
        if (arrow && arrow->cTransform)
        {
            if (m_isSlowMotionApplied) 
            {
                continue;
            }
            else
            {
                arrow->cTransform->pos += arrow->cTransform->velocity;
            }
        }
    }
}

// Handle user input
void Game::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_running = false;
        }

        if (event.type == sf::Event::KeyPressed)
        {
            sf::Keyboard::Key key = event.key.code;

            // Check if dashing is allowed (cooldown completed)
            if (m_dashCooldownClock.getElapsedTime().asSeconds() >= DASH_COOLDOWN)
            {
                m_canDash = true; // Reset dash availability
            }

            if (!m_keyHeld[key])
            {
                float now = m_keyPressClocks[key].getElapsedTime().asSeconds();

                if (m_canDash && m_keyClockStarted[key] && now < DOUBLE_TAP_THRESHOLD)
                {
                    if (key == sf::Keyboard::D && !m_isDashingRight)
                    {
                        m_isDashingRight = true;
                        m_canDash = false;
                        m_dashCooldownClock.restart();
                        m_dashClock.restart(); // Start dash timer
                    }
                    else if (key == sf::Keyboard::A && !m_isDashingLeft)
                    {
                        m_isDashingLeft = true;
                        m_canDash = false;
                        m_dashCooldownClock.restart();
                        m_dashClock.restart(); // Start dash timer
                    }
                }

                m_keyClockStarted[key] = true;
                m_keyPressClocks[key].restart();
            }

            m_keyHeld[key] = true; // Mark key as held

            
            switch (key)
            {
                // Handle movement keys
                case sf::Keyboard::W:
                    m_player->cInput->up = true;
                    m_isJumping = false;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = true;
                    break;
                case sf::Keyboard::D:
                    m_player->cInput->right = true;
                    break;

                // Melee Ranged Switch
                case sf::Keyboard::Q:
                    if (m_isMelee)
                    {
                        m_isMelee = false;
                        m_isRanged = true;
                    }
                    else
                    {
                        m_isMelee = true;
                        m_isRanged = false;
                    }

                    break;
                default:
                    break;
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            sf::Keyboard::Key key = event.key.code;
            m_keyHeld[key] = false; // Mark key as released

            switch (key)
            {
                // Handle movement keys
                case sf::Keyboard::W:
                    m_player->cInput->up = false;
                    break;
                case sf::Keyboard::A:
                    m_player->cInput->left = false;
                    break;
                case sf::Keyboard::D:
                    m_player->cInput->right = false;
                    break;
                default:
                    break;
            }
        }

        // Shooting controls
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                m_mouseClock.restart();
                m_isMousePressed = true;
            }
        }

        if (event.type == sf::Event::MouseButtonReleased)
        {
            if (event.mouseButton.button == sf::Mouse::Left && m_isMousePressed)
            {
                m_target = Vec2(event.mouseButton.x, event.mouseButton.y);
                sf::Time elapsed = m_mouseClock.getElapsedTime();
                m_isMousePressed = false;

                if (!m_isMelee)
                {
                    float chargedSpeed = 0;
                    if (elapsed.asSeconds() < 4.0)
                    {
                        if (elapsed.asSeconds() < 0.5)
                        {
                            chargedSpeed = 15.0f;
                        }
                        else
                        {
                            chargedSpeed = elapsed.asSeconds() * elapsed.asSeconds() * 7 + 15.0f;
                        }
                    }
                    else
                    {
                        chargedSpeed = 127.0f;
                    }
                    m_bulletSpeed = chargedSpeed; // Set final bullet speed

                    spawnArrow(m_player, m_target);

                    m_bulletSpeed = 15.0f; 
                }
                else if (m_isMelee) // Melee logic (sword swing)
                {
                    // float chargeTime = elapsed.asSeconds(); // Get swing charge time
                    // float swingForce = std::min(chargeTime * 500.0f, 2000.0f); // Calculate swing force

                    // // Get the direction vector from player to mouse (used for rotation and swinging direction)
                    // sf::Vector2f playerPos(m_player->cTransform->pos.x, m_player->cTransform->pos.y);
                    // sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
                    // sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                    // sf::Vector2f direction = mousePosF - playerPos;
                    // direction = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y); // Normalize direction

                    // // Apply force to the sword after charging
                    // for (auto& sword : m_entities.getEntities("sword"))
                    // {
                    //     if (sword->cTransform)
                    //     {
                    //         sword->cTransform->velocity = direction * swingForce; // Apply velocity in the direction of the mouse
                    //     }
                    // }
                }
            }
        }
    }
}
