#pragma once
#include <entt/entt.hpp>

namespace RSim::ECS
{
	struct Link
	{
		friend class Entity;

		Link() = default;
		Link(entt::entity First, entt::entity Prev, entt::entity Next, entt::entity Parent) :
		FirstChild(First),PreviousSibling(Prev),NextSibling(Next),Parent(Parent) {}

		[[nodiscard]] entt::entity GetFirstChild() const { return FirstChild; }
		[[nodiscard]] entt::entity GetPreviousSibling() const { return PreviousSibling; }
		[[nodiscard]] entt::entity GetNextSibling() const { return NextSibling; }
		[[nodiscard]] entt::entity GetLastChild() const { return LastChild; }
		[[nodiscard]] entt::entity GetParent() const { return Parent; }
		[[nodiscard]] std::size_t GetNumChildren() const { return NumChildren; }

		static constexpr std::size_t InvalidChildIndex() { return std::numeric_limits<std::size_t>::max(); }
	private:
		entt::entity FirstChild{entt::null};
		entt::entity PreviousSibling{entt::null};
		entt::entity NextSibling{entt::null};
		entt::entity LastChild{ entt::null };
		/**
		 * \brief Parent entity. Might be null.
		 */
		entt::entity Parent{entt::null};
		std::size_t NumChildren = 0;
		std::size_t ChildIndex = InvalidChildIndex();	
	};
}
