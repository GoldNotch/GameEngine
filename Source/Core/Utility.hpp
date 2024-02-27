#pragma once

#include <utility>

namespace Core::utils
{

// internal overload with indices tag
template<typename InIt, std::size_t... Idxs,
         typename ValueType = typename std::iterator_traits<InIt>::value_type,
         typename Array = std::array<ValueType, sizeof...(Idxs)>>
constexpr Array make_array(InIt first, std::index_sequence<Idxs...>)
{
  return Array{{(void(Idxs), *first++)...}};
}

// externally visible interface
template<std::size_t N, typename InIt,
         typename ValueType = typename std::iterator_traits<InIt>::value_type>
constexpr std::array<ValueType, N> make_array(InIt first, InIt last)
{
  // last is not relevant if we're assuming the size is N
  // I'll assert it is correct anyway
  static_assert(last - first == N);
  return make_array(first, std::make_index_sequence<N>{});
}


/// @brief combines two hash result
/// @param seed - old hash-value to combine with
/// @param v - value to hash
template<class T, typename Hasher = std::hash<T>>
inline void hash_combine(std::size_t & seed, const T & v) noexcept
{
  Hasher hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}


} // namespace Core::utils
