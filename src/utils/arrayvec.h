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

#include "mdarray.h"
#include "../types.h"

#include <algorithm>
#include <cassert>
#include <cstddef>

namespace purebred::utils {

    template<typename T, usize kMaxSize>
    class ArrayVec {

        using ArrayType = MDArray<T, kMaxSize>;
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

    public:
        [[nodiscard]] constexpr ArrayVec() = default;
        constexpr ~ArrayVec() = default;

        [[nodiscard]] constexpr bool operator==(const ArrayVec<T, kMaxSize> &) const = default;
        [[nodiscard]] constexpr ArrayVec &operator=(const ArrayVec<T, kMaxSize> &) = default;
        [[nodiscard]] constexpr ArrayVec &operator=(ArrayVec<T, kMaxSize> &&) = default;
        [[nodiscard]] constexpr ArrayVec(const ArrayVec<T, kMaxSize> &) = default;
        [[nodiscard]] constexpr ArrayVec(ArrayVec<T, kMaxSize> &&) = default;

        [[nodiscard]] constexpr const_reference at(size_type i) const {
            assert(0 <= i && i < mSize);
            return mData.at(i);
        }

        [[nodiscard]] constexpr reference at(size_type i) {
            assert(0 <= i && i < mSize);
            return mData.at(i);
        }

        [[nodiscard]] constexpr const_reference operator[](size_type i) const {
            assert(0 <= i && i < mSize);
            return mData[i];
        }

        [[nodiscard]] constexpr reference operator[](size_type i) {
            assert(0 <= i && i < mSize);
            return mData[i];
        }

        [[nodiscard]] constexpr reference front() {
            return *this->begin();
        }

        [[nodiscard]] constexpr const_reference front() const {
            assert(!this->empty());
            return *this->begin();
        }

        [[nodiscard]] constexpr reference back() {
            assert(!this->empty());
            return *(--this->end());
        }

        [[nodiscard]] constexpr const_reference back() const {
            return *(--this->end());
        }

        [[nodiscard]] constexpr pointer data() {
            return mData.data();
        }

        [[nodiscard]] constexpr const_pointer data() const {
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
            return mData.begin() + static_cast<std::ptrdiff_t>(mSize);
        }

        [[nodiscard]] constexpr const_iterator end() const {
            return mData.begin() + static_cast<std::ptrdiff_t>(mSize);
        }

        [[nodiscard]] constexpr const_iterator cend() const {
            return mData.cbegin() + static_cast<std::ptrdiff_t>(mSize);
        }

        [[nodiscard]] constexpr reverse_iterator rbegin() {
            return mData.rend() - static_cast<std::ptrdiff_t>(mSize);
        }

        [[nodiscard]] constexpr const_reverse_iterator rbegin() const {
            return mData.rend() - static_cast<std::ptrdiff_t>(mSize);
        }

        [[nodiscard]] constexpr const_reverse_iterator crbegin() const {
            return mData.crend() - static_cast<std::ptrdiff_t>(mSize);
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

        constexpr void push(const T &val) {
            assert(mSize < kMaxSize);
            (*this)[mSize++] = val;
        }

        constexpr void pop() {
            mSize--;
            assert(mSize >= 0);
        }

        constexpr void push_heap(const T &val) {
            this->push(val);
            std::push_heap(this->begin(), this->end());
        }

        constexpr void pop_heap() {
            std::pop_heap(this->begin(), this->end());
            this->pop();
        }

        constexpr void heapify() {
            std::make_heap(this->begin(), this->end());
        }

        constexpr void remove_o1(const size_type i) {
            assert(0 <= i && i < mSize);
            (*this)[i] = back();
            this->pop();
        }

        constexpr void clear() {
            mSize = 0;
        }

        [[nodiscard]] constexpr size_type size() const {
            return mSize;
        }

        [[nodiscard]] constexpr size_type max_size() const {
            return kMaxSize;
        }

        [[nodiscard]] constexpr bool empty() const {
            return mSize == 0;
        }

        constexpr void resize(size_type size) {
            assert(size <= kMaxSize);
            mSize = size;
        }

        constexpr void swap(ArrayVec<T, kMaxSize> &other) {
            mData.swap(other.mData);
            std::swap(mSize, other.mSize);
        }

    private:
        ArrayType mData;
        size_type mSize = 0;
    };
}