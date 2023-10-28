#pragma once
#include <type_traits>

namespace typing
{

	template<class T>
	struct registred_slot;


	template<typename T, typename Val, Val value>
	requires(std::is_class_v<T> && (std::is_enum_v<Val> || std::is_integral_v<Val>))
	struct registred_type_mapping final
	{
		constexpr operator Val() const noexcept { return value; }
		constexpr Val operator()() const noexcept { return value; }; // since c++14
	};

#define REGISTER_SLOT(slot, type)\
	template struct registred_type_mapping<type, decltype(slot), slot>;


	template<typename T>
	using MapType = registred_slot<T>::value;

}
