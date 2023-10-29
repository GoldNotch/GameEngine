#pragma once
#include <type_traits>

/* 
Compile-type system for mapping types to ints and backward.
It allows you create static array of heterogenous types
*/

/// Enables slotting system for types. Insert before REGISTER_SLOT
#define SLOT_TYPES(slot_type) \
template <class T>	\
struct registred_slot final {};	\
template<slot_type slot> \
using type_by_slot = void; \
template <class T> 	\
inline static constexpr slot_type slot_v = registred_slot<T>::value

//use only in the same namespace where SLOT_TYPES called
#define REGISTER_SLOT(container, slot, type)                                               \
template <>                                                                     \
struct container::registred_slot<type> final                                             \
{                                                                               \
	using value_type = decltype((slot));                                        \
	static_assert(std::is_class_v<type>, "type must be class or struct");     \
	static_assert(std::is_integral_v<value_type> || std::is_enum_v<value_type>, \
					  "slot must be integral");                         \
	inline static constexpr value_type value = (slot);                                 \
	constexpr operator value_type() const noexcept { return value; }            \
	constexpr value_type operator()() const noexcept { return value; }          \
};	\
template<>	\
using container::type_by_slot<slot> = type
