#pragma once

#include <tuple>

#include <entt/entt.hpp>
#define RSIM_ENABLE_ASSERTS

#include "realsim/core/Assert.h"

#include "realsim/ecs/Scene.h"
#include "realsim/ecs/Link.h"

namespace RSim::ECS
{
    class Entity
    {
    public:
        Entity() = default;

        Entity(Scene *pScene, entt::entity entity);

        static Entity Null() { return { nullptr,entt::null }; }

        template<typename T>
        auto GetComponent() -> T &;

        template<typename T, typename... Args>
        auto AddComponent(Args &&...args) -> T &;

        /**
    	 * \return The parent entity(*this) with the updated link.
    	 */
    	Entity AddChild(Entity Child);
        /**
         * \brief Removes the given entity as a child.
         * \param Child Child to remove.
         * \return The parent entity(*this).
         */
        Entity RemoveChild(Entity Child);

        [[nodiscard]] TransformComponent& GetLocalTransform();
        [[nodiscard]] TransformComponent const& GetLocalTransform() const;
        [[nodiscard]] Link const& GetLink() const;

        template<typename T>
        void RemoveComponent() const;

        operator entt::entity() const
        {
            return m_EntityHandle;
        }

    private:
        entt::entity m_EntityHandle{entt::null};
        Scene *m_Scene{nullptr};
    };

    template<typename T>
    auto Entity::GetComponent() -> T &
    {
        return m_Scene->GetComponent<T>(m_EntityHandle);
    }

    template<typename T, typename... Args>
    auto Entity::AddComponent(Args &&...args) -> T &
    {
        return m_Scene->AddComponent<T>(m_EntityHandle, std::forward<Args>(args)...);
    }

    template<typename T>
    void Entity::RemoveComponent() const
    {
        m_Scene->RemoveComponent<T>(m_EntityHandle);
    }
}
