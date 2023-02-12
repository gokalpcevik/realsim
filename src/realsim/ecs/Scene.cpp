#include "realsim/ecs/Scene.h"
#include "realsim/ecs/Entity.h"

namespace RSim::ECS
{
	entt::registry& Scene::GetEnTTRegistry()
	{
        return m_Registry;
	}

	size_t Scene::GetCreatedEntityCount() const
	{
        return m_Registry.size();
	}

	auto Scene::CreateEntity() -> Entity
    {
        Entity e{ this, m_Registry.create() };
        e.AddComponent<TransformComponent>();
        e.AddComponent<Link>();
        return e;
    }

    void Scene::Destroy(Entity entity)
    {
        m_Registry.destroy(entity);
    }

    void Scene::Update(float dt)
    {
    }

    void Scene::Shutdown()
    {
    }
}
