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

    spawnPlayer();
}

void Game::run()
{
    // TODO: add pause functionality in here
    //       some systems should function while paused (rendering)
    //       some systems shouldn't (movement / input)
    while (m_running)
    {
        m_entities.update();
        sEnemySpawner();
        sMovement();
        sCollision();
        sUserInput();
        sRender(); 
        sLifespan();
        chasePlayer();

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

void Game::spawnPlayer()
{
    auto entity = m_entities.addEntity("player");

    float mx = m_window.getSize().x / 2.0f;
    float my = m_window.getSize().y / 2.0f;

    entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(0.0f, 0.0f), 0.0f);
    entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);
    entity->cInput = std::make_shared<CInput>();

    m_player = entity;
}



void Game::spawnEnemy()
{
    auto entity = m_entities.addEntity("enemy");

    float ex = rand() % m_window.getSize().x;
    float ey = rand() % m_window.getSize().y;

    //random colours
    int r = (1+ rand() % 255);
    int g = (1+ rand() % 255);
    int b = (1+ rand() % 255);

    //random vertices
    int randVertices = (3 + rand() % 8);

    //random radius
    float randRadius = static_cast<float>(16+ rand() % 32);
    

    // Ensure the enemy does not spawn on the player
    float playerX = m_player->cTransform->pos.x;
    float playerY = m_player->cTransform->pos.y;
    float playerRadius = m_player->cShape->circle.getRadius() + 60.0f;
    Vec2 enemyPosition(ex, ey);
    m_enemySpeed = 2.0f;
    while (std::abs(ex - playerX) < playerRadius && std::abs(ey - playerY) < playerRadius)
    {
        ex = rand() % m_window.getSize().x;
        ey = rand() % m_window.getSize().y;
        enemyPosition = Vec2(ex, ey);
    }

    Vec2 direction = (m_player->cTransform->pos - enemyPosition).normalize();
    entity->cTransform = std::make_shared<CTransform>(enemyPosition, direction * m_enemySpeed, 0.0f);
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
    // TODO: implement the spawning of bullets which travels towards target
    //       - bullet speed is given as a scalar speed
    //       - you must set the velocity by using fomula in notes
    auto bullet = m_entities.addEntity("bullet");

    float bulletSpeed = 12.0f;

    Vec2 direction = (m_target - m_player->cTransform->pos).normalize();

    bullet->cTransform = std::make_shared<CTransform>(m_player->cTransform->pos, direction * bulletSpeed, 0);
    bullet->cShape = std::make_shared<CShape>(10, 8, sf::Color(255, 255, 255), sf::Color(0, 0, 0), 2);
    bullet->cLifespan = std::make_shared<CLifespan>(60);

    m_bulletSpawnTime = m_currentFrame;
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{

}

void Game::sMovement()
{
    // TODO: implement all entity movement in this function
    //       you should read the m_player->cInput component to determine if the player is moving

    m_player->cTransform->velocity = { 0 , 0 };

    // implement player movement
    if (m_player->cInput->up)
    {
        m_player->cTransform->velocity.y = -5;
    }

    if (m_player->cInput->down)
    {
        m_player->cTransform->velocity.y = 5;
    }

    if (m_player->cInput->left)
    {
        m_player->cTransform->velocity.x = -5;
    }

    if (m_player->cInput->right)
    {
        m_player->cTransform->velocity.x = 5;
    }


    // Sample
    m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
    m_player->cTransform->pos.y += m_player->cTransform->velocity.y;

    //m_player->cTransform->pos += m_player->cTransform->velocity;

    for (auto& bullet : m_entities.getEntities("bullet"))
    {
        if (bullet->cTransform)
        {
            bullet->cTransform->pos += bullet->cTransform->velocity;
        }
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


void Game::sCollision()
{
    for (auto& enemy : m_entities.getEntities("enemy"))
    {
        for (auto& player : m_entities.getEntities("player"))
        {
            float dx = m_player->cTransform->pos.x - enemy->cTransform->pos.x;
            float dy = m_player->cTransform->pos.y - enemy->cTransform->pos.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance < (m_player->cShape->circle.getRadius() + enemy->cShape->circle.getRadius())) 
            {
                std::cout << "collision\n";
                float mx = m_window.getSize().x / 2.0f;
                float my = m_window.getSize().y / 2.0f;
                player->cTransform->pos.x = mx;
                player->cTransform->pos.y = my;
                enemy->destroy();
            }
        }
    }

    for (auto& bullet : m_entities.getEntities("bullet"))
    {
        for (auto& enemy : m_entities.getEntities("enemy"))
        {
            float dx = bullet->cTransform->pos.x - enemy->cTransform->pos.x;
            float dy = bullet->cTransform->pos.y - enemy->cTransform->pos.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (distance < (bullet->cShape->circle.getRadius() + enemy->cShape->circle.getRadius())) 
            {
                std::cout << "enemy destroyed\n";
                enemy->destroy();
                bullet->destroy();
            }
        }
    }
}


void Game::sEnemySpawner()
{
    // TODO: code which implements enemy spawning should go here

    //      (use m_currentFrame - m_LastEnemySpawnTime) to determine how long it has been since the last enemy spawned
    int framesPerSpawn = 20;
    if (m_currentFrame - m_lastEnemySpawnTime >= framesPerSpawn)
    {
        spawnEnemy();
    }

}

void Game::chasePlayer()
{
    Vec2 newDirection;

    for (auto& enemy : m_entities.getEntities("enemy"))
    {
        newDirection = (m_player->cTransform->pos - enemy->cTransform->pos).normalize();
        enemy->cTransform->velocity = newDirection * m_enemySpeed;
    }

    for (auto& enemy : m_entities.getEntities("enemy"))
    {
        if (enemy->cTransform)
        {
            enemy->cTransform->pos += enemy->cTransform->velocity;
        }
    }
}

void Game::sRender()
{
    m_window.clear();

    Vec2 playerPos = getPlayerPosition();
    //std::cout << "Player Position: (" << playerPos.x << ", " << playerPos.y << ")\n";  

    for (auto& e : m_entities.getEntities())
    {
        //std::cout << "working2 ?\n";
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
    
        e->cTransform->angle += 1.0f;
        e->cShape->circle.setRotation(e->cTransform->angle);

        m_window.draw(e->cShape->circle);

    }

    

    m_window.display();
}

void Game::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_running = false;
        }
        // W = Up Movement
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W:
                    m_player->cInput->up = true;
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = true;
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
            switch (event.key.code)
            {
                case sf::Keyboard::W:
                    m_player->cInput->up = false;
                    break;
                case sf::Keyboard::S:
                    m_player->cInput->down = false;
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
        
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                std::cout << "Left Mouse Buttton Clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << ")\n";
                // call spawnBullet here
                //spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
                m_target = Vec2(event.mouseButton.x, event.mouseButton.y);
                //spawnBullet(m_player, Vec2(m_player->cTransform->pos));
                spawnBullet(m_player, m_target);
            }

            if (event.mouseButton.button == sf::Mouse::Right)
            {
                std::cout << "Right Mouse Buttton Clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << ")\n";
            }
        }       
    }
}