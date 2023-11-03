#pragma once
#include <vector>
#include <array>
#include <memory>
#include <functional>

#include <TypeMapping.hpp>
#include <Storage.hpp>


class Obj1 {};
class Obj2 {};

// iterate over objects by type
//		iterate over all objects
// create objects (don't invalidate pointers and iterators)
// delete objects (don't invalidate pointers and iterators)





class Scene
{
	enum ObjSlot
	{
		STATIC_MESH_OBJECT = 0,
		SLOT2 = 1,
		TOTAL
	};

	MAP_TABLE(ObjToSlot, ObjSlot);

	struct ProxyContainer
	{
		ProxyContainer() = default;

		template<typename ObjT>
		Core::Storage<ObjT>& Get() & noexcept { Init<ObjT>(); return *cast<ObjT>(container.get()); }

		template<typename ObjT>
		const Core::Storage<ObjT>& Get() const& noexcept { Init<ObjT>(); return *cast<ObjT>(container.get()); }

	private:
		using dtor_func = std::function<void(void*)>;
		std::unique_ptr<void, dtor_func> container = nullptr;
		/*
		* This container can store reference on any-typed pool.
		* I don't want make interface class IPool, because pool is finite class in Core and it can be used in many cases.
		* So I store it like void* and use reinterpret_cast for usage
		*/

		template<typename ObjT>
		void Init()
		{
			if (!container)
				container = std::unique_ptr<void, dtor_func>(new Core::Storage<ObjT>(),
					[](void* cont) { delete cast<ObjT>(cont); });
		}

		template<typename ObjT>
		static Core::Storage<ObjT>* cast(void* const ptr) { return reinterpret_cast<Core::Storage<ObjT>*>(ptr); }
	};
public:
	Scene() = default;

	template <typename ObjT>
	void push_back(ObjT obj)
	{
		auto&& pool = containers[static_cast<size_t>(ObjToSlot::type2key_v<ObjT>)].Get<ObjT>();
	}

private:
	std::array<ProxyContainer, static_cast<size_t>(ObjSlot::TOTAL)> containers;
};

ADD_MAPPING(Scene::ObjToSlot, Scene::ObjSlot::STATIC_MESH_OBJECT, Obj1);
ADD_MAPPING(Scene::ObjToSlot, Scene::ObjSlot::SLOT2, Obj2);
