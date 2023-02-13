#pragma once

#include <entt/entt.hpp>
#include "realsim/ecs/CommonComponents.h"

namespace RSim::ECS
{
    class Entity;

    class Scene
    {
    public:
        Scene();

        Scene(Scene &&) = default;

        Scene(const Scene &) = delete;

        [[nodiscard]] entt::registry& GetEnTTRegistry();

        [[nodiscard]] size_t GetCreatedEntityCount() const;

    	auto CreateEntity() -> Entity;

        auto FromEnTT(entt::entity entity) -> Entity;

        void Destroy(Entity entity);

        void Update(float dt);

        template<typename T>
        auto GetComponent(entt::entity entity) -> T &;

        template<typename ...Component>
        auto TryGet(entt::entity entity);

        template<typename T, typename... Args>
        auto AddComponent(entt::entity entity, Args &&...args) -> T &;

        template<typename T>
        auto RemoveComponent(entt::entity entity) -> void;

        template<typename T>
        auto DestroyAllComponentsOfType() -> void;

    	void Shutdown();
    private:
        void OnDestroyLink(entt::registry& registry, entt::entity entity);


    private:
        entt::registry m_Registry{};
    };

    template<typename T>
    inline auto Scene::GetComponent(entt::entity entity) -> T &
    {
        return m_Registry.get<T>(entity);
    }

    template <typename ... Component>
    auto Scene::TryGet(entt::entity entity)
    {
        return m_Registry.try_get<Component...>(entity);
    }

    template<typename T, typename... Args>
    inline auto Scene::AddComponent(entt::entity entity, Args &&...args) -> T &
    {
        return m_Registry.emplace<T>(entity, std::forward<Args>(args)...);
    }

    template<typename T>
    auto Scene::RemoveComponent(entt::entity entity) -> void
    {
        m_Registry.remove<T>(entity);
    }

    template <typename T>
    auto Scene::DestroyAllComponentsOfType() -> void
    {
        m_Registry.clear<T>();
    }
}
