#ifndef ENTITY_H
#define ENTITY_H

#include <memory>
#include <string>
#include <vector>

#include "Components.h"


class Entity {
    friend class EntityManager;

    bool m_active = true;
    size_t m_id = 0;
    std::string m_tag = "default";
    Entity(size_t id, std::string tag);
public:
    // component pointers
    std::shared_ptr<CTransform> cTransform;
    std::shared_ptr<CShape> cShape;
    std::shared_ptr<CCollision> cCollision;
    std::shared_ptr<CInput> cInput;
    std::shared_ptr<CScore> cScore;
    std::shared_ptr<CLifespan> cLifespan;
    std::shared_ptr<CFollow> cFollow;

    [[nodiscard]] bool isActive() const;

    [[nodiscard]] const std::string &tag() const;

    [[nodiscard]] size_t id() const;

    void destroy();

    std::shared_ptr<Entity> back(const std::vector<std::shared_ptr<Entity>>& EntityVecInput) const;
};

#endif //ENTITY_H