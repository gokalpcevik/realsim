#include "realsim/ecs/Scene.h"
#include "realsim/ecs/Entity.h"
#include "realsim/ecs/Link.h"

namespace RSim::ECS
{
	Scene::Scene()
	{
        m_Registry.on_destroy<Link>().connect<&Scene::OnDestroyLink>(this);
	}

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
        e.AddComponent<NameComponent>();

        return e;
    }

	auto Scene::FromEnTT(entt::entity entity) -> Entity
	{
        Entity e{ this, entity };
        // Will the compiler optimize these out? TODO: Research more.
        volatile auto& tc  = m_Registry.get_or_emplace<TransformComponent>(entity);
        volatile auto& link = m_Registry.get_or_emplace<Link>(entity);
        volatile auto& nc = m_Registry.get_or_emplace<NameComponent>(entity);
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

    void Scene::OnDestroyLink(entt::registry& registry, entt::entity entity)
    {
        Entity entity_{ this , entity };
        Entity parent = entity_.GetParent();
        if (parent != Entity::Null) parent.RemoveChild(entity_);

        Link const& parentLink = entity_.GetLink();
        entt::entity PreviousSibling{ parentLink.GetFirstChild() };
        while (PreviousSibling != entt::null)
        {
            Link& childLink = GetComponent<Link>(PreviousSibling);
            Destroy({ this,PreviousSibling });
            PreviousSibling = childLink.GetNextSibling();
        }
    }
}
