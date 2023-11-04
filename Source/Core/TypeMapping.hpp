#pragma once
#include <type_traits>

#define MAP_TABLE(name, _key_type)                                             \
  struct name final {                                                          \
    using key_type = _key_type;                                                \
                                                                               \
  private:                                                                     \
    template <typename T> struct type2key final {};                            \
    template <key_type key> struct key2type final {};                          \
                                                                               \
  public:                                                                      \
    template <typename T>                                                      \
    inline static constexpr key_type type2key_v = type2key<T>::value;          \
    template <key_type key> using key2type_t = key2type<key>::type;            \
  }

#define ADD_MAPPING(table_name, _key, _type)                                   \
  template <> struct table_name::type2key<_type> final {                       \
    inline static constexpr decltype((_key)) value = (_key);                   \
  };                                                                           \
  template <> struct table_name::key2type<_key> final { using type = _type; }
  