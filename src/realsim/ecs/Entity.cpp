#include "realsim/ecs/Entity.h"

namespace RSim::ECS
{
    Entity::Entity(Scene *pScene, entt::entity entity) : m_EntityHandle(entity), m_Scene(pScene)
    {

    }

    Entity Entity::AddChild(Entity Child)
    {
        RSIM_ASSERTM(*this != Child, "An entity cannot be both its parent and child.");
        Link& childLink = Child.GetComponent<Link>();
        Link& parentLink = GetComponent<Link>();
        // Set the child's parent to this
    	childLink.Parent = *this;
        
        if(parentLink.NumChildren == 0)
        {
            parentLink.FirstChild = Child;
            parentLink.LastChild = Child;
            parentLink.PreviousSibling = entt::null;
            parentLink.NextSibling = entt::null;
            parentLink.NumChildren++;
            childLink.ChildIndex = 0;
        }
        else
        {
        	entt::entity PreviousSibling{ parentLink.FirstChild};
            while(PreviousSibling != entt::null)
            {
                RSIM_ASSERTM(PreviousSibling != Child, "The entity you are trying to link is already a child.");
                PreviousSibling = m_Scene->GetComponent<Link>(PreviousSibling).NextSibling;
            }
            m_Scene->GetComponent<Link>(PreviousSibling).NextSibling = Child;
            childLink.PreviousSibling = PreviousSibling;
        	childLink.NextSibling = entt::null;
        	parentLink.NumChildren++;
            childLink.ChildIndex = parentLink.NumChildren - 1;
            // Parent ---------------------------------------------------
            //   |                      |                               |
            //   |--> Child 0           |--> Child 1                    |--> Child to be added
            //      Prev = null           Prev = Child 0                   Prev = Child 1
            //      Next = Child 1        Next = Child to be added         Next = null
        }
        return *this;
    }

    Entity Entity::RemoveChild(Entity Child)
    {
        RSIM_ASSERTM(*this != Child, "The Child you are trying to remove is the entity itself.");

        return Entity::Null();
    }

    TransformComponent& Entity::GetLocalTransform()
    {
        return m_Scene->GetComponent<TransformComponent>(*this);
    }

    TransformComponent const& Entity::GetLocalTransform() const
    {
        return m_Scene->GetComponent<TransformComponent>(*this);
    }

    Link const& Entity::GetLink() const
    {
        return m_Scene->GetComponent<Link>(*this);
    }
}
