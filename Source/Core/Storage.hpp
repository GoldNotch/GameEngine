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

		iterator begin() { return iterator(this, valued_list.head == nullptr ? iterator::end_index : index_of(valued_list.head)); }
		iterator end() { return iterator(this, iterator::end_index); }
		const_iterator cbegin() { return const_iterator(this, valued_list.head == nullptr ? const_iterator::end_index : index_of(valued_list.head)); }
		const_iterator cend() { return const_iterator(this, const_iterator::end_index); }


		template <typename... Args>
		inline T &emplace(Args &&...args)
		{
			Node* insert_node = nullptr;
			if (free_list.empty())
				insert_node = &nodes.emplace_back();
			else
			{
				insert_node = free_list.head;
				free_list.erase(*insert_node);
			}
			assert(insert_node != nullptr);
			T& ret = insert_node->construct(std::forward<Args>(args)...);
			valued_list.insert_back(*insert_node);
			return ret;
		}

		template<typename Iter>
		inline void erase(Iter it)
		{
			Node& del_node = it.GetNode();
			del_node.destroy();
			valued_list.erase(del_node);
			free_list.insert_back(del_node);
		}

		inline constexpr size_t size() const noexcept { return valued_list.size; }

		inline void clear() noexcept 
		{ 
			nodes.clear();
			free_list = {};
			valued_list = {};
		}

	private:
		friend iterator;
		friend const_iterator;
		struct Node;
		struct List;
		using NodePtr = Node *;

		inline constexpr size_t index_of(const NodePtr node) const noexcept 
		{ return node - nodes.data(); }

		std::vector<Node> nodes;
		List free_list;
		List valued_list;
	};

	template <typename T>
	struct Storage<T>::Node final
	{
		friend Storage<T>::iterator;
		friend Storage<T>::const_iterator;
		template <typename... Args>
		inline T &construct(Args &&...args)
		{
			assert(!data.has_value());
			return data.emplace(std::make_pair(this, std::forward<Args>(args)...));
		}

		inline void destroy() noexcept
		{
			assert(data.has_value());
			data.reset();
		}

		inline const T &get_data() const & { return data.value().second; }
		inline T &get_data() & { return data.value().second; }

	private:
		NodePtr next = nullptr;
		NodePtr prev = nullptr;
		std::optional<std::pair<Node*, T>> data = std::nullopt;
	};

	template<typename T>
	struct Storage<T>::List final
	{
		NodePtr head = nullptr;
		NodePtr tail = nullptr;
		size_t size = 0;

		inline void insert_front(Node& node) noexcept
		{
			node.prev = nullptr;
			node.next = head;
			if (head == nullptr)
				tail = &node;
			head = &node;
			++size;
		}
		inline void insert_back(Node& node) noexcept
		{
			node.prev = tail;
			node.next = nullptr;
			if (tail == nullptr)
				head = &node;
			tail = &node;
			++size;
		}
		inline void erase(Node& node) noexcept
		{
			if (node.prev)
				node.prev->next = node.next;
			if (node.next)
				node.next->prev = node.prev;
			if (head == &node)
				head = node.next;
			if (tail == &node)
				tail = node.prev;
			--size;
			#ifdef DEBUG
			node.next = nullptr;
			node.prev = nullptr;
			#endif
		}
		inline bool empty() const noexcept {return size == 0;}
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
		basic_iterator(const Cont& cont, const T& obj) noexcept
			: cont(&cont)
		{
			// Before pointer on obj we store pointer on list node which is store obj.
			// Now erase op have O(1) complexity
			// =========== Begin Unsafe block =========
			const Node* node = reinterpret_cast<const Node*>(reinterpret_cast<const char*>(&obj) - sizeof(Node*));
			index = cont.index_of(node);
			//============= End of unsafe block =========
		}
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
			if (index != end_index)
			{
			Node *next = GetNode().next;
			if (next != nullptr)
				index = cont->index_of(next);
			else
				index = end_index;
			}
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
	};
}