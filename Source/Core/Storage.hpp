#pragma once
#include <list>
#include <memory_resource>
#include <optional>
#include <cassert>
#include <type_traits>
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

		struct ObjectPointer;
		template<typename... Args>
		ObjectPointer emplace(Args&& ...args)
		{
			T val(std::forward<Args>(args)...);
			return container::emplace(end(), std::move(val));
		}

		void erase(ObjectPointer&& ptr)
		{
			container::erase(ref.it);
			ref = end();
		}

	private:
		std::pmr::monotonic_buffer_resource pool { BufSize };
		using iterator = container::iterator;
	};

	template<typename T, size_t BufSize>
	struct Storage<T, BufSize>::ObjectPointer final
	{
		T& operator*() { return it.operator*(); }
		const T& operator*() const { return it.operator*(); }
		T* operator->() { return it.operator->(); }
		const T* operator->() const { return it.operator->(); }

		bool operator==(ObjectReference other) const { return it == other.it; }
	private:
		friend Storage<T, BufSize>;
		ObjectReference(iterator it) : it(it){}
		iterator it;
	};

	template<typename... Ts>
	struct HeterogeneousStorage
	{
		HeterogeneousStorage() = default;
		template<typename ObjT>
		using ObjPtr = Storage<ObjT>::ObjectPointer;
		struct GenericObjectPointer;

		template <typename ObjT, typename... Args>
		ObjPtr<ObjT> emplace(Args &&...args)
		{
			Storage<ObjT>& pool = containers[type_indexer::template index_of<ObjT>].Get<ObjT>();
			return pool.emplace(std::forward<Args>(args)...);
		}

		template <typename ObjT>
		void erase(ObjPtr<ObjT> && ref)
		{
			Storage<ObjT>& pool = containers[type_indexer::template index_of<ObjT>].Get<ObjT>();
			pool.erase(std::forward<ObjRef<ObjT>>(ref));
		}

	private:
		using type_indexer = metaprogramming::type_table<Ts...>;
		struct ProxyContainer
		{
			ProxyContainer() = default;

			template<typename ObjT>
			Storage<ObjT>& Get() & noexcept { Init<ObjT>(); return *cast<ObjT>(container.get()); }

			template<typename ObjT>
			const Storage<ObjT>& Get() const& noexcept { Init<ObjT>(); return *cast<ObjT>(container.get()); }

		private:
			using dtor_func = std::function<void(void*)>;
			std::unique_ptr<void, dtor_func> container = nullptr;
			size_t type_id;
			/*
			* This container can store reference on any-typed pool.
			* I don't want make interface class IPool, because pool is finite class in Core and it can be used in many cases.
			* So I store it like void* and use reinterpret_cast for usage
			*/

			template<typename ObjT>
			void Init()
			{
				if (!container) {
					container = std::unique_ptr<void, dtor_func>(new Storage<ObjT>(),
						[](void* cont) { delete cast<ObjT>(cont); });
					type_id = type_indexer::template index_of<ObjT>;
				}
			}

			template<typename ObjT>
			static Storage<ObjT>* cast(void* const ptr) { return static_cast<Storage<ObjT>*>(ptr); }
		};

		std::array<ProxyContainer, type_indexer::size> containers;
	};


	struct GenericObjectPointer
	{


	private:
		void* obj_ptr = nullptr;
		std::type_info type_info;
	};
}