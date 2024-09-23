/*
 * Purebred, a UCI chess engine
 * Copyright (C) 2024 cj5716
 *
 * Purebred is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Purebred is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Purebred. If not, see <https://www.gnu.org/licenses/>.
 */

#include <array>

// Thanks to Stormphrax for this code for multiple-dimensional arrays without ugly nesting
template <typename T, usize N, usize... Ns>
struct ArrayImpl
{
    using Type = std::array<typename ArrayImpl<T, Ns...>::Type, N>;
};

template <typename T, usize N>
struct ArrayImpl<T, N>
{
    using Type = std::array<T, N>;
};

template <typename T, usize... Ns>
using Array = typename ArrayImpl<T, Ns...>::Type;

template<typename T, std::size_t Size>
class ArrayVec {

public:
    
};