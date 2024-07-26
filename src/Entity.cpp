#include "Entity.h"

// Constructor
Entity::Entity(const size_t id, const std::string& tag)
    : m_id(id)
    , m_tag(tag)
    , m_active(true)
{
}

// Check if the entity is active
bool Entity::isActive() const
{
    return m_active;
}

// Get the tag of the entity
const std::string & Entity::tag() const
{
    return m_tag;
}

// Get the ID of the entity
const size_t Entity::id() const
{
    return m_id;
}

// Mark the entity for destruction
void Entity::destroy()
{
    m_active = false;
}
