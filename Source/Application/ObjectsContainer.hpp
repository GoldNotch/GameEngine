#pragma once
#include <vector>
#include <array>
#include <memory>
#include <TypeMapping.hpp>



	class Obj1{};

// iterate over objects by type
//		iterate over all objects
// create objects (don't invalidate pointers and iterators)
// delete objects (don't invalidate pointers and iterators)

class Scene
{
	enum ObjSlot
	{
		STATIC_MESH_OBJECT = 0,
		TOTAL
	};

	SLOT_TYPES(ObjSlot);

	struct ProxyContainer
	{
		template<typename ObjT>
		ProxyContainer();

	private:
		std::unique_ptr<void> container = nullptr;
	};

	template <typename ObjT>
	ObjT &push_back(ObjT obj)
	{
		auto && cont = containers[slot_v<ObjT>];
		
	}

private:
	std::array<ProxyContainer, ObjSlot::TOTAL> containers;
};

REGISTER_SLOT(Scene, ObjSlot::STATIC_MESH_OBJECT, Obj1);