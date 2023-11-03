#pragma once
#include <vector>
#include <optional>
#include <cassert>
#include <type_traits>

namespace Core
{
	template<class T>
	class Storage final
	{
		template<typename Cont>
		struct basic_iterator;

	public:
		using iterator = basic_iterator<Storage>;
		using const_iterator = basic_iterator<const Storage>;


		template<typename ...Args>
		T& emplace(Args && ... args)
		{
			if (free_list_head == nullptr)
			{
				auto&& new_node = nodes.emplace_back();
				new_node.construct(std::forward<Args>(args)...);
				new_node.insert_in_list(valued_list_head);
			}
			else
			{
				free_list_head->construct(std::forward<Args>(args)...);
				Node* node = free_list_head->remove_from_list(free_list_head);
				node->insert_in_list(valued_list_head);
			}
		}

		void remove(iterator it)
		{

		}



	private:
		friend iterator;
		struct Node
		{
			using ListNode = Node*;
			template<typename ...Args>
			T& construct(Args && ...args)
			{
				assert(!data.has_value());
				return data.emplace(std::forward<Args>(args)...);
			}

			void destroy() noexcept { assert(data.has_value()); data.reset(); }

			void insert_in_list(ListNode& head) noexcept
			{
				prev = nullptr;
				next = head;
				head = this;
			}

			ListNode remove_from_list(ListNode& head) noexcept
			{
				if (prev)
					prev->next = next;
				if (next)
					next->prev = prev;
				if (this == head)
					head = next;
				next = nullptr;
				prev = nullptr;
				return this;
			}

			const T& get_data() const& { return data.value(); }
			T& get_data()& { return data.value(); }

		private:
			Node* next = nullptr;
			Node* prev = nullptr;
			std::optional<T> data = std::nullopt;
		};
		using NodePtr = Node*;

		std::vector<Node> nodes;
		NodePtr free_list_head = nullptr;
		NodePtr valued_list_head = nullptr;
	};


	template<typename T>
	template<typename Cont>
	struct Storage<T>::basic_iterator final : std::bidirectional_iterator_tag
	{
		friend Storage;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::bidirectional_iterator_tag;

		basic_iterator() noexcept = default;
		basic_iterator(Cont& cont, size_t ind) noexcept
			: cont(&cont), index(ind) {}

		reference operator*() const { GetNode().get_data(); }
		basic_iterator operator++();
		basic_iterator operator++(int);
		basic_iterator operator--();
		basic_iterator operator--(int);

	private:
		Cont* cont = nullptr;
		size_t index = 0;

		const Storage<T>::Node& GetNode() const& { assert(cont); return cont->nodes[index]; }
		Storage<T>::Node& GetNode()& { assert(cont); return cont->nodes[index]; }
	};
}