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

		iterator begin() { return iterator(this, valued_list.head); }
		iterator end() { return iterator(this, null_index); }
		const_iterator cbegin() { return const_iterator(this, valued_list.head); }
		const_iterator cend() { return const_iterator(this, null_index); }

		template <typename... Args>
		inline T &emplace(Args &&...args) &
		{
			Node *insert_node = nullptr;
			NodePtr ind = null_index;
			try
			{
				if (free_list.empty())
				{
					insert_node = &nodes.emplace_back();
					ind = index_of(insert_node);
				}
				else
				{
					ind = free_list.head;
					insert_node = &get_node(ind);
					free_list.erase(*this, ind);
				}
				assert(insert_node != nullptr);
				insert_node->construct(ind, std::forward<Args>(args)...);
			}
			catch (...)
			{
				if (insert_node)
				{
					insert_node->destroy();
					nodes.pop_back();
				}
				throw;
			}
			valued_list.insert_back(*this, ind);
			return insert_node->get_data();
		}

		template <typename Iter>
		inline void erase(Iter &&it)
		{
			NodePtr del_node_ptr = it.index;
			if (del_node_ptr != null_index)
			{
				get_node(del_node_ptr).destroy();
				valued_list.erase(*this, del_node_ptr);
				free_list.insert_back(*this, del_node_ptr);
			}
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
		friend Node;
		friend List;
		using NodePtr = size_t;
		static constexpr NodePtr null_index = static_cast<NodePtr>(-1); ///< -1 value reserved for end-value
		inline constexpr size_t index_of(const Node *const node) const noexcept
		{
			return node - nodes.data();
		}
		inline constexpr Node &get_node(NodePtr index)
		{
			return nodes[index];
		}

		std::vector<Node> nodes;
		List free_list;
		List valued_list;
	};

	template <typename T>
	struct Storage<T>::Node final
	{
		friend Storage<T>::iterator;
		friend Storage<T>::const_iterator;
		friend Storage<T>::List;
		template <typename... Args>
		inline T &construct(NodePtr index, Args &&...args)
		{
			if (data.has_value())
				throw std::runtime_error("Tried to create value in valued node");
			T val(std::forward<Args>(args)...);
			opt_type p = std::make_pair(index, std::move(val));
			return data.emplace(std::move(p)).second;
		}

		inline void destroy() noexcept
		{
			assert(data.has_value());
			data.reset();
		}

		inline const T &get_data() const & { return data.value().second; }
		inline T &get_data() & { return data.value().second; }

	private:
		NodePtr next = null_index;
		NodePtr prev = null_index;
		using opt_type = std::pair<NodePtr, T>;
		std::optional<opt_type> data = std::nullopt;
	};

	template <typename T>
	struct Storage<T>::List final
	{
		NodePtr head = null_index;
		NodePtr tail = null_index;
		size_t size = 0;

		inline void insert_front(Storage<T> &cont, NodePtr node_ptr) noexcept
		{
			Node &node = cont.get_node(node_ptr);
			node.prev = null_index;
			node.next = head;
			if (head == null_index)
				tail = node_ptr;
			else
				cont.get_node(head).prev = node_ptr;
			head = node_ptr;
			++size;
		}
		inline void insert_back(Storage<T> &cont, NodePtr node_ptr) noexcept
		{
			Node &node = cont.get_node(node_ptr);
			node.prev = tail;
			node.next = null_index;
			if (tail == null_index)
				head = node_ptr;
			else
				cont.get_node(tail).next = node_ptr;
			tail = node_ptr;
			++size;
		}
		inline void erase(Storage<T> &cont, NodePtr node_ptr) noexcept
		{
			Node &node = cont.get_node(node_ptr);
			if (node.prev != null_index)
				cont.get_node(node.prev).next = node.next;
			if (node.next != null_index)
				cont.get_node(node.next).prev = node.prev;
			if (head == node_ptr)
				head = node.next;
			if (tail == node_ptr)
				tail = node.prev;
			--size;
#ifdef DEBUG
			node.next = null_index;
			node.prev = null_index;
#endif
		}
		inline bool empty() const noexcept { return size == 0; }
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
		basic_iterator(Cont &cont, const T &obj) noexcept
			: cont(&cont)
		{
			// Before obj we store index of list node which is store obj.
			// Now erase op have O(1) complexity
			// =========== Begin Unsafe block =========
			index = reinterpret_cast<const Storage<T>::Node::opt_type *>(reinterpret_cast<const char *>(&obj) - sizeof(NodePtr))->first;
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
			if (index == null_index)
				index = cont->valued_list.head;
			else
				index = GetNode().next;
			return *this;
		}
		basic_iterator operator++(int) const
		{
			basic_iterator copy(*this);
			++*this; // call the prefix increment
			return copy;
		}
		basic_iterator operator--()
		{
			if (index == null_index)
				index = cont->valued_list.tail;
			else
				index = GetNode().prev;
			return *this;
		}
		basic_iterator operator--(int) const
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
		Cont *cont = nullptr;	   ///< pointer on Storage. It's pointer because we must have default constructor
		size_t index = null_index; ///< offset in Storage::nodes vector. if storage built on vector, pointer on Node can be changed, so we must store offset

		const Storage<T>::Node &GetNode() const &
		{
			if (!cont)
				throw std::runtime_error("iterator doesn't bound to storage");
			return cont->get_node(index);
		}
		Storage<T>::Node &GetNode() &
		{
			return GetNode();
		}
	};
}