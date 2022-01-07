// Copyright Ivan Sorokin 2022. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <cassert>
#include <cstddef>
#include <type_traits>

template <typename T, typename E>
struct enum_map
{
    static_assert(std::is_same_v<std::underlying_type_t<E>, size_t>);

    T& operator[](E index)
    {
        size_t index_n = static_cast<size_t>(index);
        assert(index_n < size());
        return elements[index_n];
    }

    T const& operator[](E index) const
    {
        size_t index_n = static_cast<size_t>(index);
        assert(index_n < size());
        return elements[index_n];
    }

    constexpr size_t size() const
    {
        return static_cast<size_t>(E::max);
    }

    T elements[static_cast<size_t>(E::max)];
};
