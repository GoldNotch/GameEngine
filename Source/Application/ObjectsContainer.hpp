#pragma once
#include <vector>
#include <type_traits>
#include <TypeMapping.hpp>




// iterate over objects by type
//		iterate over all objects
// create objects (don't invalidate pointers and iterators)
// delete objects (don't invalidate pointers and iterators)

struct Scene
{
	enum class ObjSlot
	{
		STATIC_MESH_OBJECT = 0,
		TOTAL
	};

	REGISTER_TYPE_MAPPING(ObjSlot::STATIC_MESH_OBJECT, Obj1);

		template<typename ObjT>
		struct ObjsContainer : private std::vector<ObjT>
		{};

		template<typename ObjT>
		ObjT& push_back(ObjT obj)
		{
			RegisterSlot<ObjT>::value
		}

private:
	std::array<ObjsContainer, ObjSlot::TOTAL> containers;
};

