#pragma once
#include <type_traits> 


namespace Core::metaprogramming
{
    template<typename... Ts>
    class type_table final
    {
        template<size_t Index, typename T, typename... Ts>
        struct type_table_row;

        template<typename Table, size_t idx>
        struct row_indexer;

        using data = type_table_row<0, Ts...>;

    public:
        static constexpr size_t size = sizeof...(Ts);
        template<typename T>
        static constexpr size_t index_of = data::template index_of<T>();
        template<size_t idx>
        using type_of = typename row_indexer<data, idx>::indexed_type;
        using FirstT = type_of<0>;
        using LastT = type_of<size - 1>;
    };


    template<typename... _Ts>
    template<size_t Index, typename T, typename... Ts>
    struct type_table<_Ts...>::type_table_row final
    {
        static constexpr size_t index = Index;
        using type = T;
        using next = type_table_row<index + 1, Ts...>;

        template<typename U>
        static constexpr size_t index_of()
        {
            if constexpr (std::is_same_v<U, T>) return index;
            else return next::template index_of<U>();
        }
    };


    template<typename... _Ts>
    template<size_t Index, typename T>
    struct type_table<_Ts...>::type_table_row<Index, T> final
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

    template< typename... _Ts>
    template<typename Table, size_t idx>
    struct type_table<_Ts...>::row_indexer final 
    {
        using indexed_type = typename row_indexer<typename Table::next, idx - 1>::indexed_type;
    };

    template< typename... _Ts>
    template<typename Table>
    struct type_table<_Ts...>::row_indexer<Table, 0> final
    {
        using indexed_type = typename Table::type;
    };

}

  