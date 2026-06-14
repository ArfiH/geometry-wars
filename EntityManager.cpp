#include "EntityManager.h"
#include <iostream>

EntityManager::EntityManager() = default;

// called at beginning of each frame by game engine
// entities added will now be available to use this frame
void EntityManager::update() {
    // TODO:
    // - add entities from m_entitiesToAdd to the proper location(s)
    // - add them to the vector of all entities
    // - add them to the vector inside the map, with the tag as a key
   
    if (m_entitiesToAdd.size()) {
        std::cerr << "m_entitiesToAdd's size: " << m_entitiesToAdd.size() << '\n'; 
    }
    for (auto e: m_entitiesToAdd) {
        m_entities.push_back(e);
        std::string tagStr = e->tag();
        std::cerr << "Added " << tagStr << '\n'; 
        m_entityMap[tagStr].push_back(e);
    }
    m_entitiesToAdd.clear();
   
    // remove dead entities from the vector of all entities
    removeDeadEntities(m_entities);
    // remove dead entities from each vector in the entity map
    // C++20 way ot iterating through [key, value] pairs in a map
    for (auto &[tag, entityVec]: m_entityMap) {
        removeDeadEntities(entityVec);
    }
}

void EntityManager::removeDeadEntities(EntityVec &vec) {
    // TODO: remove all dead entities from the input vector
    // this is called by the update() function

    int removeCnt = 0;
    size_t i = 0;
    for (auto e: m_entities)
    {
        // if e is dead, remove it from m_entities
        // if e is dead, remove it from m_entityMap[e->tag()]
        if (!e->isActive())
        {
            // remove the item and put the last item of that vector in its place
            m_entities[i] = m_entities.back();
            removeCnt++;
            std::cerr << "Deleted " << e->tag() << ' ' << e->id() << " from m_entities\n";
        }
        i++;
    }

    // pop_back() removeCnt times
    while (m_entities.size() > 0 && removeCnt) {
        m_entities.pop_back();
        removeCnt--;
    }
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag) {
    // create a new Entity object
    // store it in the vector of all entities
    // store it in the map of tag->entityVector
    // return the shared pointer pointing to that entity
    auto e = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    m_entitiesToAdd.push_back(e);
    m_entityMap[tag].push_back(e);
    return e;

    // auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    // m_entities.push_back(entity);
    // m_entityMap[tag].push_back(entity);
    // return entity;
}
// note: above code doesn't handle some map-related edge cases

EntityVec &EntityManager::getEntities() {
    return m_entities;
}

EntityVec &EntityManager::getEntities(const std::string &tag) {
    // TODO: check edge cases
    return m_entityMap[tag];
}

const std::map<std::string, EntityVec> &EntityManager::getEntityMap() {
    return m_entityMap;
}

// Iterator Invalidation Example
// void sCollision()
// {
//     EntityVec bullets;
//     EntityVec tiles;
//     for (auto & b: bullets)
//         for (auto & t: tiles)
//             if(Physics::IsCollision(b,t)) bullets.erase(b);
// }
// Solution: Delayed Effects
// Idea: only add or remove entities at the beginning of a frame when it is safe
