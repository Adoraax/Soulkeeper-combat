#include "Game.h"

#include <iostream>
#include <fstream>
#include <cmath>

Game::Game(const std::string & config)
{
    init(config);
}

void Game::init(const std::string & path)
{
    //std::ifstream fin(path);

    //fin >> m_playerConfig.SR >> m_playerConfig.CR;

    m_window.create(sf::VideoMode(3640, 1080), "Assignment 2");
    m_window.setFramerateLimit(60);
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
        //std::cout << "Delta Time: " << m_deltatime << "\n";
        m_entities.update();
        //sEnemySpawner();
        sApplyGravity();
        sCollision();
        sUserInput();
        slowMotion();
        sMovement();
        sRender(); 
        //sLifespan();
        //chasePlayer();
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
    //entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f); // circle shape
    entity->cShape = std::make_shared<CShape>(sf::Vector2f(90.0f, 180.0f), sf::Color(0, 0, 0), sf::Color(255, 0, 0), 2.0f); // rectangle shape

    entity->cInput = std::make_shared<CInput>();

    m_player = entity;
    //std::cout << "Player Spawned\n";
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

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
    // TODO: spawn small enemies at the location of the input enemy e

    // when we create the smaller enemy, we have to read the values of the original enemy
    // - spawn a number of small enemies equal to the vertices of the shape
    // - set each small enemy to the same color as the original, hakf the size
    // - small enemies are worth double points of the original enemy
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
    static int arrowCount = 0; // Static variable to keep track of the number of arrows spawned
    auto arrow = m_entities.addEntity("arrow");

    Vec2 direction = (m_target - m_player->cTransform->pos).normalize();
    
    arrow->cTransform = std::make_shared<CTransform>(m_player->cTransform->pos, direction * m_bulletSpeed, 0.0f, 0.0f);
    arrow->cShape = std::make_shared<CShape>(sf::Vector2f(70.0f, 15.0f), sf::Color(0, 0, 0), sf::Color(255, 255, 255), 2.0f);

    m_bulletSpawnTime = m_currentFrame;

    // Increment the arrow count and log it to the console
    for (auto& arrow : m_entities.getEntities("arrow"))
    {
            arrowCount++;
            std::cout << "Arrow number: " << arrowCount << "\n";
    }
    
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

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{

}


// Slow Motion Function using scaling velocity -- trajectory is affected
// void Game::slowMotion()
// {
//     static bool slowMotionApplied = false; // Tracks whether slow motion is active

//     if (m_isMousePressed && !slowMotionApplied)
//     {
//         for (auto& e : m_entities.getEntities())
//         {
//             if (e->tag() != "player" && e->cTransform)
//             {
//                 e->cTransform->speed *= 0.25f; // Scale down velocity
//             }
//         }
//         // Scale other forces like gravity, if necessary
//         GRAVITY *= 0.25f;
//         //AIR_RESISTANCE *= 0.25f;
//         slowMotionApplied = true;
//     }
//     else if (!m_isMousePressed && slowMotionApplied)
//     {
//         for (auto& e : m_entities.getEntities())
//         {
//             if (e->tag() != "player" && e->cTransform)
//             {
//                 e->cTransform->speed *= 4.0f; // Reset velocity to normal
//             }
//         }
//         // Reset gravity scaling
//         GRAVITY *= 4.0f;
//         //AIR_RESISTANCE *= 4.0f;
//         slowMotionApplied = false;
//     }
// }


// Slow motion using frame limits -- static time scaling
// void Game::slowMotion()
// {
//     m_timeScale = std::exp(-m_decayRate * m_mouseClock.getElapsedTime().asSeconds());
//     // std::cout << "Time Scale: " << m_timeScale << "\n";
//     // std::cout << "Mouse held for: " << m_mouseClock.getElapsedTime().asSeconds() << " seconds\n";
//     if (m_isMousePressed && !m_isSlowMotionApplied)
//     {
//         for (auto& e : m_entities.getEntities())
//         {
//             if (e->tag() != "player" && e->cTransform)
//             {
//                 //e->cTransform->velocity *= 0.5f;
//                 m_window.setFramerateLimit(15);
//             }
//         }
//         m_isSlowMotionApplied = true; // Set the flag to indicate slow motion is applied
//     }
//     else if (!m_isMousePressed && m_isSlowMotionApplied)
//     {
//         for (auto& e : m_entities.getEntities())
//         {
//             if (e->tag() != "player" && e->cTransform)
//             {
//                 //e->cTransform->velocity *= 2.0f; // Restore the original velocity
//                 m_window.setFramerateLimit(60);
//             }
//         }
//         m_isSlowMotionApplied = false; // Reset the flag
//     }
// }


// Slow motion using frame limits -- dynamic time scaling
void Game::slowMotion()
{
    // << "slowMotion" << "\n";
    if (m_isMousePressed)
    {
        // Calculate the time scale based on elapsed time
        float elapsedTime = m_mouseClock.getElapsedTime().asSeconds() * 1.5f;
        m_timeScale = std::exp(-m_decayRate * elapsedTime); // Time decay

        // Calculate the framerate limit (minimum limit of 15 to prevent stutter)
        unsigned int frameLimit = static_cast<unsigned int>(std::max(10.0f, 60.0f * m_timeScale));
        m_window.setFramerateLimit(frameLimit);

        m_isSlowMotionApplied = true;
        //std::cout << "slow motion applied\n";
    }
    else if (m_isSlowMotionApplied) // When the mouse is released, reset framerate
    {
        m_window.setFramerateLimit(60);
        m_isSlowMotionApplied = false;
        //std::cout << "slow motion removed\n";
    }
}


// Update the lifespan of entities with lifespan components
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
    //std::cout << "sCollision" << "\n";
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

    // arrow collision -- oriiignal implementation
    // arrow collision with enemy
    // for (auto& arrow : m_entities.getEntities("arrow"))
    // {
    //     for (auto& enemy : m_entities.getEntities("enemy"))
    //     {
    //         float dx = arrow->cTransform->pos.x - enemy->cTransform->pos.x;
    //         float dy = arrow->cTransform->pos.y - enemy->cTransform->pos.y;
    //         float distance = std::sqrt(dx * dx + dy * dy);
    //         if (distance < (arrow->cShape->rectangle.getSize().x + enemy->cShape->circle.getRadius())) 
    //         {
    //             //std::cout << "enemy destroyed\n";
    //             enemy->destroy();
    //             arrow->destroy();
    //         }
    //     }
    // }


    // Arrow collision with enemy -- safer implementation
    for (auto& arrow : m_entities.getEntities("arrow"))
    {
        // Ensure arrow has the required components
        if (!arrow->cTransform || !arrow->cShape)
        {
            continue;
        }

        for (auto& enemy : m_entities.getEntities("enemy"))
        {
            // Ensure enemy has the required components
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
            if (distance < (arrow1Width + arrow2Width) / 2.0f)
            {
                // Example response: Destroy both arrows
                // float angle1Rad= std::atan2(arrow1->cTransform->velocity.y, arrow1->cTransform->velocity.x);
                // float angle1Deg = angle1Rad * 180.0f / M_PI;
                // float angle2Rad = std::atan2(arrow2->cTransform->velocity.y, arrow2->cTransform->velocity.x);
                // float angle2Deg = angle2Rad * 180.0f / M_PI;

                float reflectionArrow1VelocityY = (arrow1->cTransform->velocity.y + arrow2->cTransform->velocity.y) / 2.0f * 1.5;
                float reflectionArrow1VelocityX = (arrow1->cTransform->velocity.x + arrow2->cTransform->velocity.x) / 2.0f * 1.5;
                float reflectionArrow2VelocityY = (arrow2->cTransform->velocity.y + arrow2->cTransform->velocity.y) / 2.0f * 0.5;
                float reflectionArrow2VelocityX = (arrow2->cTransform->velocity.x + arrow2->cTransform->velocity.x) / 2.0f * 0.5;

                arrow1->cTransform->velocity = Vec2(reflectionArrow1VelocityX, reflectionArrow1VelocityY);
                arrow2->cTransform->velocity = Vec2(reflectionArrow2VelocityX, reflectionArrow2VelocityY);



                // Break out of the inner loop to avoid accessing a destroyed arrow
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
            if (e->tag() == "player")
            {
                e->cTransform->velocity.y += GRAVITY * m_deltatime * 1.75f;
            }
            else if (e->tag() == "enemy")
            {
                e->cTransform->velocity.y += GRAVITY * m_deltatime;
            }
            else if (e->tag() == "bullet")
            {
                e->cTransform->velocity.y += GRAVITY * m_deltatime;
            }
            else if (e->tag() == "arrow")
            {
                e->cTransform->velocity.y += GRAVITY * m_deltatime;
            }

            // Update position based on velocity
            e->cTransform->pos += e->cTransform->velocity * m_deltatime;

            // Handle Y-axis deceleration
            // if (e->cTransform->pos.y < groundLimit)
            // {
            //     if (e->tag() == "player")
            //     {
            //         e->cTransform->velocity.y /= AIR_RESISTANCE;
            //     }
            //     else if (e->tag() == "enemy")
            //     {
            //         e->cTransform->velocity.y /= AIR_RESISTANCE; // Air resistance
            //     }
            //     else if (e->tag() == "bullet")
            //     {
            //         e->cTransform->velocity.y /= AIR_RESISTANCE; // Air resistance
            //     }
            //     else if (e->tag() == "arrow")
            //     {
            //         e->cTransform->velocity.y /= AIR_RESISTANCE; // Air resistance
            //     }
            // }


            // Handle X-axis deceleration
            // sf::Vector2f dragForce(e->cTransform->velocity.x * (-dragCoefficient * m_bulletSpeed), e->cTransform->velocity.y * (-dragCoefficient * m_bulletSpeed));
            if (std::abs(e->cTransform->velocity.x) > 1.0f)
            {
                if (e->cTransform->pos.y < groundLimit)
                {
                    if (e->tag() == "player")
                    {
                        e->cTransform->velocity.x *= AIR_RESISTANCE;
                    }
                    else if (e->tag() == "enemy")
                    {
                        e->cTransform->velocity.x *= AIR_RESISTANCE; // Air resistance
                    }
                    else if (e->tag() == "bullet")
                    {
                        e->cTransform->velocity.x *= AIR_RESISTANCE; // Air resistance
                    }
                    else if (e->tag() == "arrow")
                    {
                        e->cTransform->velocity.x *= AIR_RESISTANCE; // Air resistance
                    }
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

            groundLimit = GROUND_LEVEL;
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
                    std::cout << "Arrow angle: " << angleDeg << "\n";
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

    if (elapsed < DASH_DURATION)
    {
        // Calculate dash velocity using deltatime
        float dashSpeed = (m_isDashingRight ? DASH_DISTANCE : -DASH_DISTANCE) / DASH_DURATION;
        m_player->cTransform->velocity.x = dashSpeed;
        m_player->cShape->rectangle.setSize(sf::Vector2f(90.0f, 90.0f));
        m_player->cShape->rectangle.setOrigin(45.0f, 45.0f);

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
                m_player->cTransform->velocity.y = -15;
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
            m_player->cTransform->velocity.x = 0; // Stop horizontal movement when no input
        }
    }


    // Apply regular velocity updates
    if (!m_isDashingLeft && !m_isDashingRight)
    {
        m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
        m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
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
            arrow->cTransform->pos += arrow->cTransform->velocity;
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

            // Handle movement keys
            switch (key)
            {
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
                
                std::cout << "Left Mouse Button Pressed\n";
            }
        }

        if (event.type == sf::Event::MouseButtonReleased)
        {
            if (event.mouseButton.button == sf::Mouse::Left && m_isMousePressed)
            {
                m_target = Vec2(event.mouseButton.x, event.mouseButton.y);
                sf::Time elapsed = m_mouseClock.getElapsedTime();
                m_isMousePressed = false;

                float chargedSpeed = 0;
                if (elapsed.asSeconds() < 0.75)
                {
                    chargedSpeed = elapsed.asSeconds() * elapsed.asSeconds() * 6 + 17.0f;
                }
                else
                {
                    chargedSpeed = elapsed.asSeconds() * elapsed.asSeconds() * 18 + 17.0f;
                }

                m_bulletSpeed = chargedSpeed; // Set final bullet speed
                std::cout << "Left Mouse Button Released\n";
                std::cout << "Bullet Speed Charged: " << m_bulletSpeed << "\n";

                spawnArrow(m_player, m_target);
                m_bulletSpeed = 17.0f; // Reset speed after usage
            }
        }
    }

    // Real-time bullet speed update while mouse is pressed
    if (m_isMousePressed)
    {
        sf::Time elapsed = m_mouseClock.getElapsedTime();
        float chargingSpeed = 0;
        if (elapsed.asSeconds() < 0.75)
        {
            chargingSpeed = elapsed.asSeconds() * elapsed.asSeconds() * 6 + 17.0f;
        }
        else
        {
            chargingSpeed = elapsed.asSeconds() * elapsed.asSeconds() * 18 + 17.0f;
        }

        std::cout << "Charging Bullet Speed: " << chargingSpeed << " (Elapsed: " << elapsed.asSeconds() << " seconds)\n";
    }
}
