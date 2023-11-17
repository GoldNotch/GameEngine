#pragma once
#include <list>
#include <memory_resource>
#include <optional>
#include <cassert>
#include <type_traits>
#include <variant>
#include "TypeMapping.hpp"

namespace Core
{
	template <class T, size_t BufSize = 4096>
	class Storage final : private std::pmr::list<T>
	{
		using container = std::pmr::list<T>;
	public:
		using container::begin;
		using container::end;
		using container::rbegin;
		using container::rend;
		using container::cbegin;
		using container::cend;
		using container::crbegin;
		using container::crend;
		using container::size;
		using container::clear;
		using container::empty;
		using container::swap;
		using container::operator=;
		using container::assign;
		using container::iterator;
		using container::const_iterator;
		using container::reverse_iterator;
		using container::const_reverse_iterator;

		Storage() = default;
		Storage(const Storage& other)
			: container(other){}

		struct ObjectPointer;
		template<typename... Args>
		ObjectPointer emplace(Args&& ...args)
		{
			T val(std::forward<Args>(args)...);
			return ObjectPointer(this, container::emplace(end(), std::move(val)));
		}

		void erase(ObjectPointer&& ptr)
		{
			container::erase(ptr.it);
			ptr = ObjectPointer(this, end());
		}

	private:
		std::pmr::monotonic_buffer_resource pool{ BufSize };
	};


	template<typename T, size_t BufSize>
	struct Storage<T, BufSize>::ObjectPointer final
	{
		ObjectPointer() = default;
		ObjectPointer(const Storage<T, BufSize>* container, container::iterator it)
			: cont(container), it(it) {}

		constexpr T& operator*()& { return it.operator*(); }
		constexpr const T& operator*() const& { return it.operator*(); }
		constexpr T* operator->()& { return it.operator->(); }
		constexpr const T* operator->() const& { return it.operator->(); }

		constexpr bool operator==(ObjectPointer other) const { return it == other.it; }
		constexpr operator bool() const noexcept { return cont && it != cont->end(); }
		constexpr const Storage<T, BufSize>* GetStorage() const& { return cont; }
	private:
		friend Storage<T, BufSize>;
		container::iterator it;
		const Storage<T, BufSize>* cont = nullptr;
	};


	template<typename... Ts>
	class HeterogeneousStorage final
	{
		template<typename ObjT>
		using StorageBacket = Storage<ObjT>;
		template<typename ObjT>
		using ObjPtr = typename StorageBacket<ObjT>::ObjectPointer;
		template<typename ObjT>
		using typed_iterator = typename StorageBacket<ObjT>::iterator;
		template<typename ObjT>
		using typed_const_iterator = typename StorageBacket<ObjT>::const_iterator;
		template<typename ObjT>
		using typed_reverse_iterator = typename StorageBacket<ObjT>::reverse_iterator;
		template<typename ObjT>
		using typed_const_reverse_iterator = typename StorageBacket<ObjT>::const_reverse_iterator;


	public:
		HeterogeneousStorage() = default;
		struct GenericObjectPointer;
		friend GenericObjectPointer;

		using iterator = std::variant<typed_iterator<Ts>...>;
		using const_iterator = std::variant<typed_const_iterator<Ts>...>;
		using reverse_iterator = std::variant<typed_reverse_iterator<Ts>...>;
		using const_reverse_iterator = std::variant<typed_const_reverse_iterator<Ts>...>;

		template <typename ObjT, typename... Args>
		ObjPtr<ObjT> emplace(Args &&...args)
		{
			return Get<ObjT>().emplace(std::forward<Args>(args)...);
		}

		template <typename ObjT>
		void erase(ObjPtr<ObjT>&& ptr)
		{
			return Get<ObjT>().erase(std::forward<ObjPtr<ObjT>>(ptr));
		}

		void erase(GenericObjectPointer&& ptr) { erase(ptr); }

		template<typename ObjT>
		decltype(auto) begin() noexcept	{ return Get<ObjT>().begin(); }

		template<typename ObjT>
		decltype(auto) end() noexcept { return Get<ObjT>().end(); }

		template<typename ObjT>
		decltype(auto) cbegin() const noexcept { return Get<ObjT>().cbegin(); }

		template<typename ObjT>
		decltype(auto) cend() const noexcept { return Get<ObjT>().cend();	}

		template<typename ObjT>
		decltype(auto) rbegin() noexcept { return Get<ObjT>().rbegin(); }

		template<typename ObjT>
		decltype(auto) rend() noexcept { return Get<ObjT>().rend(); }

		template<typename ObjT>
		decltype(auto) crbegin() const noexcept	{ return Get<ObjT>().crbegin(); }

		template<typename ObjT>
		decltype(auto) crend() const noexcept { return Get<ObjT>().crend(); }

	private:
		using type_indexer = metaprogramming::type_table<Ts...>;

		using VariadicContainer = std::variant<StorageBacket<Ts>...>;
		std::array<VariadicContainer, type_indexer::size> containers = { StorageBacket<Ts>()... };

	private:
		template<typename ObjT>
		StorageBacket<ObjT>& Get()& 
		{
			constexpr size_t idx = type_indexer::template index_of<ObjT>;
			try 
			{	
				return std::get<StorageBacket<ObjT>>(containers[idx]);	
			}
			catch (const std::bad_variant_access&)
			{ 
				throw std::runtime_error("Type doesn't stored in this storage");	
			}
		}
		template<typename ObjT>
		const StorageBacket<ObjT>& Get() const&	{ return Get<ObjT>(); }
	};

	template<typename ObjT, typename HStorageT>
	struct TypedView final
	{
		HStorageT& storage;
		TypedView(HStorageT& storage) : storage(storage) {}

		decltype(auto) begin() noexcept { return storage.begin<ObjT>(); }
		decltype(auto) end() noexcept { return storage.end<ObjT>(); }
		decltype(auto) cbegin() const noexcept { return storage.cbegin<ObjT>(); }
		decltype(auto) cend() const noexcept { return storage.cend<ObjT>(); }
		decltype(auto) rbegin() noexcept { return storage.rbegin<ObjT>(); }
		decltype(auto) rend() noexcept { return storage.rend<ObjT>(); }
		decltype(auto) crbegin() const noexcept { return storage.crbegin<ObjT>(); }
		decltype(auto) crend() const noexcept { return storage.rend<ObjT>(); }
	};

	template<typename... Ts>
	struct HeterogeneousStorage<Ts...>::GenericObjectPointer final
	{
		GenericObjectPointer() = default;

		template<typename ObjPtrT>
		GenericObjectPointer(ObjPtrT ptr) : ptr(ptr) {}

		template<typename ObjPtrT>
		operator ObjPtrT() const
		{
			try {
				return std::get<ObjPtrT>(ptr);
			}
			catch (const std::bad_variant_access&)
			{
				throw std::runtime_error("Type doesn't stored in this storage");
			}
		}

		bool operator==(const GenericObjectPointer& other) const noexcept
		{
			return other.ptr == ptr;
		}

		operator bool() const
		{
			return ptr.valueless_by_exception &&
				std::visit(ptr, [](auto&& ptr) { return ptr.operator bool(); });
		}

	private:
		std::variant<ObjPtr<Ts>...> ptr;
	};
}