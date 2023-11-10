#pragma once
#include <type_traits> 


namespace Core::metaprogramming
{
    template<typename... Ts>
    class type_table final
    {
        template<size_t Index, typename T, typename... Ts>
        struct type_table_base;
        template<typename Table, size_t index>
        struct type_finder;

        using data = type_table_base<0, Ts...>;

    public:
        static constexpr size_t size = sizeof...(Ts);
        template<typename T>
        static constexpr size_t index_of = data::template index_of<T>();
        template<size_t _index>
        using type_of = typename type_finder<data, _index>::found_type;
    };


    template<typename... _Ts>
    template<size_t Index, typename T, typename... Ts>
    struct type_table<_Ts...>::type_table_base final
    {
        static constexpr size_t index = Index;
        using type = T;
        using next = type_table_base<index + 1, Ts...>;

        template<typename U>
        static constexpr size_t index_of()
        {
            if constexpr (std::is_same_v<U, T>) return index;
            else return next::template index_of<U>();
        }
    };


    template<typename... _Ts>
    template<size_t Index, typename T>
    struct type_table<_Ts...>::type_table_base<Index, T> final
    {
        static constexpr size_t index = Index;
        using type = T;
        template<typename U>
        static constexpr size_t index_of()
        {
            if constexpr (std::is_same_v<U, T>) return index;
            else throw "No type in table";
        }
    };

    template<typename... _Ts>
    template<typename Table, size_t index>
    struct type_table<_Ts...>::type_finder final {
        using found_type = std::conditional_t<Table::index == index,
            typename Table::type, typename type_finder<typename Table::next, index>::found_type>;
    };

}

  