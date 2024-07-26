#include "Game.h"

#include <iostream>
#include <fstream>

Game::Game(const std::string & config)
{
    init(config);
}

void Game::init(const std::string & path)
{
    //std::ifstream fin(path);

    //fin >> m_playerConfig.SR >> m_playerConfig.CR;

    m_window.create(sf::VideoMode(1280, 720), "Assignment 2");
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

        //if (!m_paused)
        //{
        //
        //}
        sEnemySpawner();
        sMovement();
        sCollision();
        sUserInput();
        sRender();

        m_currentFrame++;
    }

}

void Game::setPaused(bool paused)
{

}

void Game::spawnPlayer()
{
    // TODO:    Finish adding all properties of the player with the correct values from the config

    // We create every entity by calling EntityManager.addEntity(tag)
    // This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
    auto entity = m_entities.addEntity("player");

    float mx = m_window.getSize().x / 2.0f;
    float my = m_window.getSize().y / 2.0f;

    // Give this entity a Transform so it spawns at (200, 200) with velocity (1,1) and angle 0
    entity->cTransform = std::make_shared<CTransform>(Vec2(mx, my), Vec2(0.0f, 0.0f), 0.0f);

    // The entity's shape will have radius 32, 8 sides, dark grey fill, and red outlines of thickness 4
    entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);

    // Add an input compnent to the player so that we can use inputs
    entity->cInput = std::make_shared<CInput>();

    // Since we watn this Entity to be out player, set out Game's player variable to be this Entity
    // This goes slightly against the EntityMnager paradigm, but we use the plater so much its worth it
    m_player = entity;

}

void Game::spawnEnemy()
{
    auto entity = m_entities.addEntity("enemy");

    float ex = rand() % m_window.getSize().x;
    float ey = rand() % m_window.getSize().y;

    entity->cTransform = std::make_shared<CTransform>(Vec2(ex, ey), Vec2(0.0f, 0.0f), 0.0f);

    entity->cShape = std::make_shared<CShape>(16.0f, 3, sf::Color(0, 0, 255), sf::Color(255, 255, 255), 4.0f);

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

    bullet->cTransform = std::make_shared<CTransform>(target, Vec2(0, 0), 0);
    bullet->cShape = std::make_shared<CShape>(10, 8,sf::Color(255, 255, 255), sf::Color(255, 0, 0), 2);
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

    // Sample
    m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
    m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
}

void Game::sLifespan()
{
    // TODO: implement all lifespan functionality

    // for all entities
    //  if entity has no lifespan component, skip it
    //  if entity has > 0 remaining lifespan, subtract 1
    //  if it has lifespan and is alive
    //      scale its alpha channel properly
    //  if it has lifespan and its time is up
    //      destriy the entity
}

void Game::sCollision()
{
    // TODO: implement all peroper collisions between entities
    //      be sure to use the collision radius, NOT the shape radius

    /*
    for (auto b : m_entities.getEntities("bullet"))
    {
        for (auto e : m_entities.getEntities("enemy"))
        {
            // if bullet is overlapping enemy, set enemy to !isalive or something
        }
    }
    */
}

void Game::sEnemySpawner()
{
    // TODO: code which implements enemy spawning should go here

    //      (use m_currentFrame - m_LastEnemySpawnTime) to determine how long it has been since the last enemy spawned
    spawnEnemy();
}

void Game::sRender()
{
    m_window.clear();

    //std::cout << m_entities.getEntities().size() << "\n";
    //std::cout << "working1 ?\n";

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

        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W:
                    std::cout << "W Key Pressed\n";
                    m_player->cInput->up = true;
                    break;
                default: break;
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::W:
                    std::cout << "W Key Released\n";
                    m_player->cInput->up = false;
                    break;
                default: break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                std::cout << "Left Mouse Buttton Clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << ")\n";
                // call spawnBullet here
                spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
            }

            if (event.mouseButton.button == sf::Mouse::Right)
            {
                std::cout << "Right Mouse Buttton Clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << ")\n";
            }
        }       
    }
}