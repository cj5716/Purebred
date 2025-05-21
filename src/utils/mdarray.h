/*
 * Purebred, a UCI chess engine
 * Copyright (C) 2025 cj5716
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

#include "../types.h"

#include <array>
#include <initializer_list>

namespace purebred::utils {

    // Credits to Stockfish for the backbone of this MDArray code, with some slight changes by me (both stylistic and functional)
    template<typename T, usize kSize, usize... kSizes>
    class MDArray;

    namespace Detail {
        template<typename T, usize kSize, usize... kSizes>
        struct MDArrayHelper {
            using ChildType = MDArray<T, kSizes...>;
        };

        template<typename T, usize kSize>
        struct MDArrayHelper<T, kSize> {
            using ChildType = T;
        };
    }

    // MDArray is a generic N-dimensional array.
    // The template parameters (kSize and kSizes) encode the dimensions of the array.
    template<typename T, usize kSize, usize... kSizes>
    class MDArray {
        using ChildType = typename Detail::MDArrayHelper<T, kSize, kSizes...>::ChildType;
        using ArrayType = std::array<ChildType, kSize>;

    public:

        [[nodiscard]] constexpr MDArray() = default;
        constexpr ~MDArray() = default;

        [[nodiscard]] constexpr bool operator==(const MDArray<T, kSize, kSizes...> &) const = default;
        [[nodiscard]] constexpr MDArray &operator=(const MDArray<T, kSize, kSizes...> &) = default;
        [[nodiscard]] constexpr MDArray &operator=(MDArray<T, kSize, kSizes...> &&) = default;
        [[nodiscard]] constexpr MDArray(const MDArray<T, kSize, kSizes...> &) = default;
        [[nodiscard]] constexpr MDArray(MDArray<T, kSize, kSizes...> &&) = default;

        [[nodiscard]] constexpr MDArray &operator=(const ArrayType &data) {
            mData = data;
        }

        [[nodiscard]] constexpr MDArray(const ArrayType &data) {
            mData = data;
        }

        [[nodiscard]] constexpr MDArray &operator=(const std::initializer_list<ChildType> data) {
            std::copy(data.begin(), data.end(), mData.begin());
        }

        [[nodiscard]] constexpr MDArray(const std::initializer_list<ChildType> data) {
            std::copy(data.begin(), data.end(), mData.begin());
        }

        using value_type             = typename ArrayType::value_type;
        using size_type              = typename ArrayType::size_type;
        using difference_type        = typename ArrayType::difference_type;
        using reference              = typename ArrayType::reference;
        using const_reference        = typename ArrayType::const_reference;
        using pointer                = typename ArrayType::pointer;
        using const_pointer          = typename ArrayType::const_pointer;
        using iterator               = typename ArrayType::iterator;
        using const_iterator         = typename ArrayType::const_iterator;
        using reverse_iterator       = typename ArrayType::reverse_iterator;
        using const_reverse_iterator = typename ArrayType::const_reverse_iterator;

        [[nodiscard]] constexpr reference at(size_type index) {
            return mData.at(index);
        }

        [[nodiscard]] constexpr const_reference at(size_type index) const {
            return mData.at(index);
        }

        [[nodiscard]] constexpr reference operator[](size_type index) {
            return mData[index];
        }

        [[nodiscard]] constexpr const_reference operator[](size_type index) const {
            return mData[index];
        }

        [[nodiscard]] constexpr reference front() {
            return mData.front();
        }

        [[nodiscard]] constexpr const_reference front() const {
            return mData.front();
        }

        [[nodiscard]] constexpr reference back() {
            return mData.back();
        }

        [[nodiscard]] constexpr const_reference back() const {
            return mData.back();
        }

        [[nodiscard]] pointer data() {
            return mData.data();
        }

        [[nodiscard]] const_pointer data() const {
            return mData.data();
        }

        [[nodiscard]] constexpr iterator begin() {
            return mData.begin();
        }

        [[nodiscard]] constexpr const_iterator begin() const {
            return mData.begin();
        }

        [[nodiscard]] constexpr const_iterator cbegin() const {
            return mData.cbegin();
        }

        [[nodiscard]] constexpr iterator end() {
            return mData.end();
        }

        [[nodiscard]] constexpr const_iterator end() const {
            return mData.end();
        }

        [[nodiscard]] constexpr const_iterator cend() const {
            return mData.cend();
        }

        [[nodiscard]] constexpr reverse_iterator rbegin() {
            return mData.rbegin();
        }

        [[nodiscard]] constexpr const_reverse_iterator rbegin() const {
            return mData.rbegin();
        }

        [[nodiscard]] constexpr const_reverse_iterator crbegin() const {
            return mData.crbegin();
        }

        [[nodiscard]] constexpr reverse_iterator rend() {
            return mData.rend();
        }

        [[nodiscard]] constexpr const_reverse_iterator rend() const {
            return mData.rend();
        }

        [[nodiscard]] constexpr const_reverse_iterator crend() const {
            return mData.crend();
        }

        [[nodiscard]] constexpr bool empty() const {
            return mData.empty();
        }

        [[nodiscard]] constexpr size_type size() const {
            return mData.size();
        }

        [[nodiscard]] constexpr size_type max_size() const {
            return mData.max_size();
        }

        void fill(const T &v) {
            for (auto &ele : mData) {
                if constexpr (sizeof...(kSizes) == 0)
                    ele = v;
                else
                    ele.fill(v);
            }
        }

        constexpr void swap(MDArray<T, kSize, kSizes...> &other) {
            mData.swap(other.mData);
        }

    private:
        ArrayType mData;
    };
}