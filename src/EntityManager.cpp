#include "EntityManager.h"

#include <iostream>
#include <memory>
#include <algorithm>

EntityManager::EntityManager()
{

}

void EntityManager::update()
{
    for (auto& e : m_entitiesToAdd)
    {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);
        
    }

    m_entitiesToAdd.clear();

    removeDeadEntities(m_entities);

    for (auto& [tag, entityVec] : m_entityMap)
    {
        removeDeadEntities(entityVec);
    }
    
}


void EntityManager::removeDeadEntities(EntityVec & vec)
{
    // TODO: remove all dead entities from the input vector
    //       this is called by the update() function         e->isActive()

    //use std::Removeif..
    auto it = std::remove_if(vec.begin(), vec.end(),
        [](const std::shared_ptr<Entity>& e) 
        {
            return !e->isActive();
        });

    // Erase the dead entities from the vector
    vec.erase(it, vec.end());
    
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string & tag)
{
    //std::cout << "addtest1\n";
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

    m_entitiesToAdd.push_back(entity);

    //std::cout << "Entity added to m_entitiesToAdd: " << tag << "\n";


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