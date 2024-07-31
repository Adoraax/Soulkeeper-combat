#include "EntityManager.h"

#include <iostream>

EntityManager::EntityManager()
{

}

void EntityManager::update()
{
    // TODO:    Add entities from m_entitiesToAdd to the proper location(s)
    //          - add them to the vector of all entities
    //          - add them to the vector inside the map, with the tag as a key
    for (auto& e : m_entitiesToAdd)
    {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);    
    }

    m_entitiesToAdd.clear();

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
    // Use std::remove_if to move all dead entities to the end of the vector
    auto it = std::remove_if(vec.begin(), vec.end(), 
        [](const std::shared_ptr<Entity>& e)
        {
            return !e->isActive(); // Check if the entity is not active (i.e., dead)
        });

    // Erase the dead entities from the vector
    vec.erase(it, vec.end());
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string & tag)
{
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

    m_entitiesToAdd.push_back(entity);

    std::cout << "Entity added to m_entitiesToAdd: " << tag << "\n";


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