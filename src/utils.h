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
#include <cassert>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

template <typename T, auto N>
class AutoIdxArray
{
public:
    constexpr AutoIdxArray() = default;
    constexpr AutoIdxArray(const std::initializer_list<T> &data) { std::copy(data.begin(), data.end(), inner.begin()); }
    [[nodiscard]] constexpr T &operator[](auto idx) { return inner[static_cast<size_t>(idx)]; }
    [[nodiscard]] constexpr const T &operator[](auto idx) const { return inner[static_cast<size_t>(idx)]; }
    [[nodiscard]] constexpr T &front() { return inner.front(); }
    [[nodiscard]] constexpr const T &front() const { return inner.front(); }
    [[nodiscard]] constexpr auto data() { return inner.data(); }
    [[nodiscard]] constexpr auto data() const { return inner.data(); }
    [[nodiscard]] constexpr auto begin() { return inner.begin(); }
    [[nodiscard]] constexpr auto begin() const { return inner.begin(); }
    [[nodiscard]] constexpr auto cbegin() const { return inner.cbegin(); }
    [[nodiscard]] constexpr auto end() { return inner.end(); }
    [[nodiscard]] constexpr auto end() const { return inner.end(); }
    [[nodiscard]] constexpr auto cend() const { return inner.cend(); }
    [[nodiscard]] constexpr auto rbegin() { return inner.rbegin(); }
    [[nodiscard]] constexpr auto rbegin() const { return inner.rbegin(); }
    [[nodiscard]] constexpr auto crbegin() const { return inner.crbegin(); }
    [[nodiscard]] constexpr auto rend() { return inner.rend(); }
    [[nodiscard]] constexpr auto rend() const { return inner.rend(); }
    [[nodiscard]] constexpr auto crend() const { return inner.crend(); }
    [[nodiscard]] constexpr bool empty() const { return inner.empty(); }
    [[nodiscard]] constexpr bool size() const { return inner.size(); }
    [[nodiscard]] constexpr bool max_size() const { return inner.max_size(); }
    constexpr void fill(T &val) { inner.fill(val); }
    constexpr void swap(AutoIdxArray &other) { inner.swap(other); }
    [[nodiscard]] constexpr T &array() { return inner; }
    [[nodiscard]] constexpr const T &array() const { return inner; }
    constexpr operator std::array<T, static_cast<size_t>(N)> &() { return inner; }
    constexpr operator const std::array<T, static_cast<size_t>(N)> &() const { return inner; }
    constexpr operator std::span<T, static_cast<size_t>(N)> &() { return inner; }
    constexpr operator const std::span<T, static_cast<size_t>(N)> &() const { return inner; }

private:
    std::array<T, static_cast<size_t>(N)> inner;
};

template <typename T, auto N>
auto swap(AutoIdxArray<T, N> &a, AutoIdxArray<T, N> &b) { a.swap(b); }

template <typename T, auto N, auto... Ns>
struct ArrayImpl
{
    using Type = AutoIdxArray<typename ArrayImpl<T, Ns...>::Type, N>;
};

template <typename T, auto N>
struct ArrayImpl<T, N>
{
    using Type = AutoIdxArray<T, N>;
};

template <typename T, auto... Ns>
using Array = typename ArrayImpl<T, Ns...>::Type;

template<typename T, size_t Size>
struct ArrayVec {

public:
    size_t size() const { return currSize; }

    constexpr void push_back(const T &value)
    {
        values[currSize] = value;
        currSize++;
        assert(currSize <= Size);
    }

    constexpr void push_back()
    {
        currSize++;
        assert(currSize <= Size);
    }

    constexpr void pop_back()
    {
        currSize--;
        assert(currSize >= 0);
    }

    constexpr void resize(size_t newSize)
    {
        assert(0 <= newSize && newSize <= Size);
        currSize = newSize;
    }

    [[nodiscard]] constexpr T &back() { return values[currSize - 1]; }
    [[nodiscard]] constexpr const T &back() const { return values[currSize - 1]; }
    [[nodiscard]] constexpr T &operator[](size_t index) { return values[index]; }
    [[nodiscard]] constexpr const T &operator[](size_t index) const { return values[index]; }

private:
    Array<T, Size> values;
    size_t currSize;
};

[[nodiscard]] inline std::vector<std::string> split_string(const std::string &str, const std::string &delim)
{
    size_t pos_start = 0, pos_end, delim_len = delim.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = str.find(delim, pos_start)) != std::string::npos)
    {
        token = str.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(str.substr(pos_start));
    return res;
}