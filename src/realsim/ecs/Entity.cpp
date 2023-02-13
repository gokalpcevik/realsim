#include "realsim/ecs/Entity.h"
#include "realsim/ecs/Link.h"

#define FROM_ENTT(EnTTHandle) RSim::ECS::Entity{m_Scene, (EnTTHandle)}

namespace RSim::ECS
{
    Entity::Entity(Scene *pScene, entt::entity entity) : m_EntityHandle(entity), m_Scene(pScene)
    {

    }

    bool Entity::IsNull() const
    {
        return *this == Null();
    }

    Entity Entity::GetParent() const
    {
        Link const& link = GetLink();
        return FROM_ENTT(link.Parent);
    }

    Entity Entity::GetTopParent() const
    {
        Link link = GetLink();
        entt::entity Parent = link.Parent;
        while(Parent != entt::null)
        {
            link = m_Scene->GetComponent<Link>(Parent);
            Parent = link.Parent;
        }
        return FROM_ENTT(Parent);
    }

    Entity Entity::GetNextSibling() const
    {
        Link const& link = GetLink();
        return FROM_ENTT(link.NextSibling);
    }

    Entity Entity::GetPreviousSibling() const
    {
        Link const& link = GetLink();
        return FROM_ENTT(link.PreviousSibling);
    }

    Entity Entity::GetFirstChild() const
    {
        Link const& link = GetLink();
        return FROM_ENTT(link.FirstChild);
    }

    Entity Entity::GetLastChild() const
    {
        Link const& link = GetLink();
        return FROM_ENTT(link.LastChild);
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

            do
            {
                RSIM_ASSERTM(PreviousSibling != Child, "The entity you are trying to link is already a child.");
                PreviousSibling = m_Scene->GetComponent<Link>(PreviousSibling).NextSibling;
            }
            while (PreviousSibling != entt::null);
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
        Link& parentLink = GetComponent<Link>();
        Link& childLink = Child.GetComponent<Link>();

        RSIM_ASSERTM(*this != Child, "The child you are trying to remove is the entity itself.");
    	RSIM_ASSERTM(parentLink.NumChildren > 0, "The parent entity has no children.");
        RSIM_ASSERTM(childLink.Parent == *this,"The child you are trying to remove does not have the parent of *this.");

        entt::entity prevSibling = childLink.PreviousSibling;
        entt::entity nextSibling = childLink.NextSibling;

        // The child being removed is in the middle
    	if(prevSibling != entt::null && nextSibling != entt::null)
		{
            Link& prevLink = m_Scene->GetComponent<Link>(prevSibling);
            Link& nextLink = m_Scene->GetComponent<Link>(nextSibling);
            prevLink.NextSibling = nextSibling;
            nextLink.PreviousSibling = prevSibling;
        }
        // The child being removed is the last child
        else if (prevSibling != entt::null && nextSibling == entt::null)
        {
            Link& prevLink = m_Scene->GetComponent<Link>(prevSibling);
            prevLink.NextSibling = nextSibling;
            parentLink.LastChild = entt::null;
        }
        // The child being removed is the first and only child
        else if (prevSibling == entt::null && nextSibling == entt::null)
        {
            parentLink.FirstChild = entt::null;
        }

        childLink.NextSibling = entt::null;
        childLink.FirstChild = entt::null;
        childLink.Parent = entt::null;
        childLink.ChildIndex = Link::InvalidChildIndex();
        --parentLink.NumChildren;

    	return *this;
    }

    Entity Entity::GetChildAt(std::size_t Index)
    {
        Link const& parentLink = GetLink();
        RSIM_ASSERT(Index < parentLink.NumChildren);
    	entt::entity PreviousSibling{ parentLink.FirstChild };
        while (PreviousSibling != entt::null)
        {
            Link& childLink = m_Scene->GetComponent<Link>(PreviousSibling);
            if (childLink.ChildIndex == Index)
            {
                return { m_Scene, PreviousSibling };
            }
            PreviousSibling = childLink.NextSibling;
        }

        return Null();
    }

    void Entity::RemoveChildAt(std::size_t Index)
    {
        Entity const entity = GetChildAt(Index);
        RemoveChild(entity);
    }

    void Entity::TryRemoveChildAt(std::size_t Index)
    {
        Entity const entity = GetChildAt(Index);
        if(entity != Null())
        {
            RemoveChild(entity);
        }
    }

    TransformComponent& Entity::GetLocalTransform()
    {
        return m_Scene->GetComponent<TransformComponent>(*this);
    }

    TransformComponent const& Entity::GetLocalTransform() const
    {
        return m_Scene->GetComponent<TransformComponent>(*this);
    }

    NameComponent const& Entity::GetName() const
    {
        return m_Scene->GetComponent<NameComponent>(*this);
    }


    Link const& Entity::GetLink() const
    {
        return m_Scene->GetComponent<Link>(*this);
    }

    void Entity::SetName(std::string Name) const
    {
        m_Scene->GetComponent<NameComponent>(*this).Name = Name;
    }
}
