#include "EntityManager.h"

EntityManager::EntityManager()
{

}

void EntityManager::update()
{
    // TODO:    Add entities from m_entitiesToAdd to the proper location(s)
    //          - add them to the vector of all entities
    //          - add them to the vector inside the map, with the tag as a key
    for (auto e : m_entitiesToAdd)
    {
        m_entities.push_back(e);
    }

    m_entities.clear();

    // remove dead entities from the vector of all entities
    removeDeadEntities(m_entities);

    // remove dead entities from each vector in the enitity map
    // C++17 way of iterating through [key, value] pairs in a map
    for (auto& [tag, entityVec] : m_entityMap)
    {
        removeDeadEntities(entityVec);
    }
}

void EntityManager::removeDeadEntities(EntityVec & vec)
{
    // TODO: remove all dead entities from the input vector
    //       this is called by the update() function

    /*
    //use std::Removeif..
    for (auto e : vec)
    {
        if (e->isActive())
        {
            remove from vec;
        }
    }
    */
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string & tag)
{
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

    m_entitiesToAdd.push_back(entity);

    return entity;
}

const EntityVec & EntityManager::getEntities()
{
    return m_entities;
}

const EntityVec & EntityManager::getEntities(const std::string & tag)
{
    return m_entityMap[tag];
}