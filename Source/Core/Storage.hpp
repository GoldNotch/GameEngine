#pragma once
#include <vector>
#include <optional>
#include <cassert>
#include <type_traits>

namespace Core
{
	template <class T>
	class Storage final
	{
		template <typename Cont>
		struct basic_iterator;

	public:
		using iterator = basic_iterator<Storage>;
		using const_iterator = basic_iterator<const Storage>;

		iterator begin() { return iterator(this, valued_list_head == nullptr ? iterator::end_index : index_of(valued_list_head)); }
		iterator end() { return iterator(this, iterator::end_index); }
		const_iterator cbegin() { return const_iterator(this, valued_list_head == nullptr ? const_iterator::end_index : index_of(valued_list_head)); }
		const_iterator cend() { return const_iterator(this, const_iterator::end_index); }

		template <typename... Args>
		T &emplace(Args &&...args)
		{
			if (free_list_head == nullptr)
			{
				auto &&new_node = nodes.emplace_back();
				new_node.construct(std::forward<Args>(args)...);
				new_node.insert_in_list(valued_list_head);
			}
			else
			{
				free_list_head->construct(std::forward<Args>(args)...);
				Node *node = free_list_head->remove_from_list(free_list_head);
				node->insert_in_list(valued_list_head);
				free_list_size--;
			}
			valued_list_size++;
		}

		template <typename Iter>
		void erase(Iter it)
		{
		}

		constexpr size_t size() const noexcept { return valued_list_size; }

	private:
		friend iterator;
		friend const_iterator;
		struct Node;
		using NodePtr = Node *;

		constexpr size_t index_of(const Node *const node) const noexcept { return node - nodes.data(); }

		std::vector<Node> nodes;
		NodePtr free_list_head = nullptr;
		NodePtr free_list_tail = nullptr;
		size_t free_list_size = 0;
		NodePtr valued_list_head = nullptr;
		NodePtr valued_list_tail = nullptr;
		size_t valued_list_size = 0;
	};

	template <typename T>
	struct Storage<T>::Node
	{
		template <typename... Args>
		T &construct(Args &&...args)
		{
			assert(!data.has_value());
			return data.emplace(std::forward<Args>(args)...);
		}

		void destroy() noexcept
		{
			assert(data.has_value());
			data.reset();
		}

		const T &get_data() const & { return data.value(); }
		T &get_data() & { return data.value(); }

		void insert_in_list(NodePtr &head, NodePtr &tail) noexcept
		{
			prev = nullptr;
			next = head;
			if (head == nullptr)
				tail = this;
			head = this;
		}

		NodePtr remove_from_list(NodePtr &head, NodePtr &tail) noexcept
		{
			if (prev)
				prev->next = next;
			if (next)
				next->prev = prev;
			if (this == head)
				head = next;
			if (this == tail)
				tail = prev;
			next = nullptr;
			prev = nullptr;
			return this;
		}

	private:
		NodePtr next = nullptr;
		NodePtr prev = nullptr;
		std::optional<T> data = std::nullopt;
	};

	template <typename T>
	template <typename Cont>
	struct Storage<T>::basic_iterator final : std::bidirectional_iterator_tag
	{
		friend Storage;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T *;
		using reference = T &;
		using iterator_category = std::bidirectional_iterator_tag;

		basic_iterator() noexcept = default;

	private:
		basic_iterator(Cont *cont, size_t ind) noexcept
			: cont(cont), index(ind) {}

	public:
		constexpr reference operator*() { return GetNode().get_data(); }
		constexpr const reference operator*() const { return GetNode().get_data(); }
		constexpr pointer operator->() { return &GetNode().get_data(); }
		constexpr const pointer operator->() const { return &GetNode().get_data(); }
		basic_iterator operator++()
		{
			next();
			return *this;
		}
		basic_iterator operator++(int)
		{
			basic_iterator copy(*this);
			++*this; // call the prefix increment
			return copy;
		}
		basic_iterator operator--()
		{
			prev();
			return *this;
		}
		basic_iterator operator--(int)
		{
			basic_iterator copy(*this);
			--*this; // call the prefix increment
			return copy;
		}

		bool operator==(const basic_iterator &other) const noexcept
		{
			return cont == other.cont && index == other.index;
		}
		bool operator!=(const basic_iterator &other) const noexcept
		{
			return !(*this == other);
		}

	private:
		Cont *cont = nullptr;										 ///< pointer on Storage. It's pointer because we must have default constructor
		size_t index = 0;											 ///< offset in Storage::nodes vector. if storage built on vector, pointer on Node can be changed, so we must store offset
		static constexpr size_t end_index = static_cast<size_t>(-1); ///< -1 value reserved for end-value

		const Storage<T>::Node &GetNode() const &
		{
			assert(cont);
			return cont->nodes[index];
		}
		Storage<T>::Node &GetNode() &
		{
			assert(cont);
			return cont->nodes[index];
		}

		void next()
		{
			if (index == end_index)
				return;
			Node *next = GetNode().next;
			if (next != nullptr)
				index = cont->index_of(next);
			else
				index = end_index;
		}

		void prev()
		{
			if (index == end_index)
				index = cont->index_of(cont->valued_list_tail);
			else
			{
				Node *prev = GetNode().prev;
				if (prev != nullptr)
					index = cont->index_of(prev);
				else
					index = cont->index_of(cont->valued_list_head);
			}
		}
	};
}