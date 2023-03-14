#pragma once

#include <tuple>
#include <entt/entt.hpp>
#define RSIM_ENABLE_ASSERTS

#include "BasicMath.hpp"

#include "realsim/core/Assert.h"
#include "realsim/ecs/Scene.h"

namespace RSim::ECS
{
    struct Link;

    /**
     * \brief 
     * ----------------------------------------------------------\n
     * <b>Example usages of ECS hierarchy:</b>\n
     * Parent.AddChild(Child0).AddChild(Child1).AddChild(Child2); \n
     * Parent.RemoveChild(Child0).AddChild(Child4).RemoveChild(Child1);
     */
    class Entity
    {
    public:
        Entity() = default;

        Entity(Scene *pScene, entt::entity entity);

        bool operator==(Entity const& rhs) const { return rhs.m_Scene == m_Scene && rhs.m_EntityHandle == m_EntityHandle; }

        static Entity const Null;

        template<typename T>
        auto GetComponent() -> T &;

        template<typename T, typename... Args>
        auto AddComponent(Args &&...args) -> T &;

        [[nodiscard]] bool IsNull() const;

        [[nodiscard]] Entity GetParent() const;
        [[nodiscard]] Entity GetTopParent() const;
        [[nodiscard]] Entity GetNextSibling() const;
        [[nodiscard]] Entity GetPreviousSibling() const;
        [[nodiscard]] Entity GetFirstChild() const;
        [[nodiscard]] Entity GetLastChild() const;
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

        Entity GetChildAt(std::size_t Index);
        Entity GetChildByName(std::string_view Name);

        void RemoveChildAt(std::size_t Index);
        void TryRemoveChildAt(std::size_t Index);

        /**
         * \brief Transform of the entity, with respect to the world's coordinate frame.
         */
        [[nodiscard]] Dl::float4x4 GetWorldTransform() const;
        /**
         * \brief Transform of the entity, with respect to the coordinate frame of it's parent, which might be another entity's frame or
         * the world frame.
         */
        [[nodiscard]] Dl::float4x4 GetLocalTransform() const;
        [[nodiscard]] NameComponent const& GetName() const;
        [[nodiscard]] Link const& GetLink() const;

        void SetName(std::string Name) const;

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
