#include "realsim/ecs/Entity.h"
#include "realsim/ecs/Link.h"

#define FromEnTT(EnTTHandle) RSim::ECS::Entity{m_Scene, (EnTTHandle)}

namespace RSim::ECS
{
    Entity const Entity::Null = { nullptr,entt::null };

    Entity::Entity(Scene *pScene, entt::entity entity) : m_EntityHandle(entity), m_Scene(pScene)
    {
        
    }

    bool Entity::IsNull() const
    {
        return *this == Entity::Null;
    }

    bool Entity::IsFirstChild() const
    {
        return *this == GetParent().GetFirstChild();
    }

    Entity Entity::GetParent() const
    {
        Link const& link = GetLink();
        return FromEnTT(link.Parent);
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
        return FromEnTT(Parent);
    }

    Entity Entity::GetNextSibling() const
    {
        Link const& link = GetLink();
        return FromEnTT(link.NextSibling);
    }

    Entity Entity::GetPreviousSibling() const
    {
        Link const& link = GetLink();
        return FromEnTT(link.PreviousSibling);
    }

    Entity Entity::GetFirstChild() const
    {
        Link const& link = GetLink();
        return FromEnTT(link.FirstChild);
    }

    Entity Entity::GetLastChild() const
    {
        Link const& link = GetLink();
        return FromEnTT(link.LastChild);
    }

    Entity Entity::AddChild(Entity Child)
    {
        RSIM_ASSERTM(*this != Child, "An entity cannot be both its parent and child.");
        Link& childLink = Child.GetComponent<Link>();
        RSIM_ASSERTM(childLink.Parent == entt::null,"The child that is to be added already has a parent.");
        Link& parentLink = GetComponent<Link>();
        // Set the child's parent to this
    	childLink.Parent = *this;
        
        if(parentLink.NumChildren == 0)
        {
            // The parent does not have any children until now, so set the first and last child to the added child
            parentLink.FirstChild = Child;
            parentLink.LastChild = Child;
            // Since we'll only have one child, siblings do not exist
            parentLink.PreviousSibling = entt::null;
            parentLink.NextSibling = entt::null;
            // Increase the # children and set the child index to zero
            parentLink.NumChildren++;
            childLink.ChildIndex = 0;
        }
        else
        {
        	entt::entity PreviousSibling{ parentLink.FirstChild};
            while(PreviousSibling != entt::null)
            {
                RSIM_ASSERTM(PreviousSibling != Child, "The entity you are trying to link is already a child.");
                entt::entity NextSibling = m_Scene->GetComponent<Link>(PreviousSibling).NextSibling;
                if(NextSibling != entt::null)
                {
                    PreviousSibling = NextSibling;
                }
                else
                {
                    break;
                }
            }
            Link& PrevSiblingLink = m_Scene->GetComponent<Link>(PreviousSibling);
            // Update the last child's next sibling to point to the added child
        	PrevSiblingLink.NextSibling = Child;
            // Set the added child's previous sibling as the last child
            childLink.PreviousSibling = PreviousSibling;
            // Since we are inserting at the end, next sibling is null
        	childLink.NextSibling = entt::null;
            // Increase the # of children
        	parentLink.NumChildren++;
            // Set the last child to the added child
            parentLink.LastChild = Child;
            // Since we are inserting at the end, index is # children minus one
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
            parentLink.LastChild = entt::null;
        }

        childLink.NextSibling = entt::null;
        childLink.Parent = entt::null;
        childLink.ChildIndex = Link::InvalidChildIndex();
        parentLink.NumChildren--;

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

        return Entity::Null;
    }

    Entity Entity::GetChildByName(std::string_view Name)
    {
        Link const& parentLink = GetLink();
        entt::entity PreviousSibling{ parentLink.FirstChild };
        while (PreviousSibling != entt::null)
        {
            Link& childLink = m_Scene->GetComponent<Link>(PreviousSibling);
            auto& childName = m_Scene->GetComponent<NameComponent>(PreviousSibling);
            if (childName.Name == Name)
            {
                return { m_Scene, PreviousSibling };
            }
            PreviousSibling = childLink.NextSibling;
        }
        return Entity::Null;
    }

    void Entity::RemoveChildAt(std::size_t Index)
    {
        Entity const entity = GetChildAt(Index);
        RemoveChild(entity);
    }

    void Entity::TryRemoveChildAt(std::size_t Index)
    {
        Entity const entity = GetChildAt(Index);
        if(entity != Entity::Null)
        {
            RemoveChild(entity);
        }
    }

	Dl::float4x4 Entity::GetWorldTransform() const
    {
        if (GetParent().IsNull()) return GetLocalTransform();

        Entity parent = GetParent();
        Dl::float4x4 Transform = GetLocalTransform();
    	while(parent != Null)
        {
            Transform = Transform * parent.GetLocalTransform();
            parent = parent.GetParent();
        }
        return Transform;
    }

    Dl::float4x4 Entity::GetLocalTransform() const
    {
        return m_Scene->GetComponent<TransformComponent>(*this).GetTransform();
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
        m_Scene->GetComponent<NameComponent>(*this).Name = std::move(Name);
    }
}
