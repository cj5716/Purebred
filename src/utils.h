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

#pragma once
#include <array>
#include <cstdint>

// Thanks to Stormphrax for this code for multiple-dimensional arrays without ugly nesting
template <typename T, size_t N, size_t... Ns>
struct ArrayImpl
{
    using Type = std::array<typename ArrayImpl<T, Ns...>::Type, N>;
};

template <typename T, size_t N>
struct ArrayImpl<T, N>
{
    using Type = std::array<T, N>;
};

template <typename T, size_t... Ns>
using Array = typename ArrayImpl<T, Ns...>::Type;

template<typename T, size_t Size>
class ArrayVec {

public:
    size_t size() const { return currSize; }

    void push_back(const T &value) {
        values.at(currSize) = value;
        currSize++;
        assert(currSize < Size);
    }

    void push_back() {
        currSize++;
        assert(currSize < Size);
    }

    void pop_back() {
        assert(currSize > 0);
        currSize--;
    }

    T &back() { return values.at(currSize - 1); }
    T &at(int index) { return values.at(index); }

private:
    Array<T, Size> values;
    size_t currSize;
};