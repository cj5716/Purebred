/*
 * Purebred, a UCI chess engine
 * Copyright (C) 2025 cj5716
 *
 * Purebred is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Purebred is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Purebred. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "core.h"
#include "types.h"
#include "utils/mdarray.h"

namespace purebred {

    enum class Direction : i32 {
        kUp = 8,
        kDown = -kUp,
        kRight = 1,
        kLeft = -kRight,

        kUpLeft = kUp + kLeft,
        kUpRight = kUp + kRight,
        kDownLeft = kDown + kLeft,
        kDownRight = kDown + kRight
    };

    [[nodiscard]] constexpr Direction operator+(Direction a, Direction b) {
        return static_cast<Direction>(static_cast<i32>(a) + static_cast<i32>(b));
    }

    [[nodiscard]] constexpr Direction operator-(Direction a, Direction b) {
        return static_cast<Direction>(static_cast<i32>(a) - static_cast<i32>(b));
    }

    class Biterator;

    class Bitboard {
    public:
        [[nodiscard]] constexpr Bitboard() = default;
        [[nodiscard]] constexpr bool operator==(const Bitboard &) const = default;

        explicit constexpr Bitboard(u64 bb) {
            mData = bb;
        }

        explicit constexpr Bitboard(Square sq) {
            mData = static_cast<u64>(1) << sq.raw();
        }

        [[nodiscard]] constexpr u64 raw() const {
            return mData;
        }

        [[nodiscard]] constexpr operator u64() const {
            return this->raw();
        }

        [[nodiscard]] constexpr Bitboard operator&(u64 other) const {
            return Bitboard{mData & other};
        }

        [[nodiscard]] constexpr Bitboard operator|(u64 other) const {
            return Bitboard{mData | other};
        }

        [[nodiscard]] constexpr Bitboard operator^(u64 other) const {
            return Bitboard{mData ^ other};
        }

        constexpr Bitboard &operator&=(u64 other) {
            return *this = *this & other;
        }

        constexpr Bitboard &operator|=(u64 other) {
            return *this = *this | other;
        }

        constexpr Bitboard &operator^=(u64 other) {
            return *this = *this ^ other;
        }

        [[nodiscard]] constexpr Bitboard operator~() const {
            return Bitboard{~mData};
        }

        [[nodiscard]] constexpr Bitboard operator<<(i32 shift) const {
            assert(shift >= 0);
            return Bitboard{mData << shift};
        }

        [[nodiscard]] constexpr Bitboard operator>>(i32 shift) const {
            assert(shift >= 0);
            return Bitboard{mData >> shift};
        }

        constexpr Bitboard &operator<<=(i32 shift) {
            return *this = *this << shift;
        }

        constexpr Bitboard &operator>>=(i32 shift) {
            return *this = *this >> shift;
        }

        [[nodiscard]] constexpr bool get_bit(Square sq) {
            return mData & Bitboard{sq};
        }

        constexpr Bitboard &set_bit(Square sq) {
            return *this |= Bitboard{sq};
        }

        constexpr Bitboard &unset_bit(Square sq) {
            return *this &= ~Bitboard{sq};
        }

        constexpr Bitboard &toggle_bit(Square sq) {
            return *this ^= Bitboard{sq};
        }

        constexpr Bitboard &reset() {
            return *this = Bitboard{};
        }

        [[nodiscard]] constexpr i32 count_bits() const {
            return __builtin_popcountll(mData);
        }

        [[nodiscard]] constexpr bool empty() const {
            return *this == Bitboard{};
        }

        [[nodiscard]] constexpr bool multiple_bits_set() const {
            return mData & (mData - 1);
        }

        [[nodiscard]] constexpr bool one_bit_set() const {
            return !this->empty() && !this->multiple_bits_set();
        }

        [[nodiscard]] constexpr Square lsb() const {
            assert(!this->empty());
            return Square{__builtin_ctzll(mData)};
        }

        constexpr void pop_lsb() {
            mData &= mData - 1;
        }

        // Use the Carry-Rippler trick to enumerate all subsets of a bitboard.
        [[nodiscard]] constexpr Bitboard next_subset(Bitboard curr) const {
            return Bitboard{(curr - mData) & mData};
        }

        template <Direction kDir>
        [[nodiscard]] constexpr Bitboard shift() const;

        template <Direction kDir>
        [[nodiscard]] constexpr Bitboard ray(Bitboard occ = Bitboard{}) const;

    private:
        u64 mData;

        [[nodiscard]] constexpr Biterator begin() const;
        [[nodiscard]] constexpr Biterator end() const;

        friend class Biterator;
    };

    struct Bitboards {
        constexpr Bitboards() = delete;

        static constexpr utils::MDArray<Bitboard, Ranks::kNum> kRanks = {
            Bitboard{U64C(0x00000000000000FF)},
            Bitboard{U64C(0x000000000000FF00)},
            Bitboard{U64C(0x0000000000FF0000)},
            Bitboard{U64C(0x00000000FF000000)},
            Bitboard{U64C(0x000000FF00000000)},
            Bitboard{U64C(0x0000FF0000000000)},
            Bitboard{U64C(0x00FF000000000000)},
            Bitboard{U64C(0xFF00000000000000)}
        };

        static constexpr Bitboard kRank1 = kRanks[Ranks::k1];
        static constexpr Bitboard kRank2 = kRanks[Ranks::k2];
        static constexpr Bitboard kRank3 = kRanks[Ranks::k3];
        static constexpr Bitboard kRank4 = kRanks[Ranks::k4];
        static constexpr Bitboard kRank5 = kRanks[Ranks::k5];
        static constexpr Bitboard kRank6 = kRanks[Ranks::k6];
        static constexpr Bitboard kRank7 = kRanks[Ranks::k7];
        static constexpr Bitboard kRank8 = kRanks[Ranks::k8];

        static constexpr utils::MDArray<Bitboard, Files::kNum> kFiles = {
            Bitboard{U64C(0x0101010101010101)},
            Bitboard{U64C(0x0202020202020202)},
            Bitboard{U64C(0x0404040404040404)},
            Bitboard{U64C(0x0808080808080808)},
            Bitboard{U64C(0x1010101010101010)},
            Bitboard{U64C(0x2020202020202020)},
            Bitboard{U64C(0x4040404040404040)},
            Bitboard{U64C(0x8080808080808080)}
        };

        static constexpr Bitboard kFileA = kFiles[Files::kA];
        static constexpr Bitboard kFileB = kFiles[Files::kB];
        static constexpr Bitboard kFileC = kFiles[Files::kC];
        static constexpr Bitboard kFileD = kFiles[Files::kD];
        static constexpr Bitboard kFileE = kFiles[Files::kE];
        static constexpr Bitboard kFileF = kFiles[Files::kF];
        static constexpr Bitboard kFileG = kFiles[Files::kG];
        static constexpr Bitboard kFileH = kFiles[Files::kH];

        static constexpr Bitboard kEmpty{};
        static constexpr Bitboard kAll = ~kEmpty;
        static constexpr Bitboard kDarkSquares{U64C(0xAA55AA55AA55AA55)};
        static constexpr Bitboard kLightSquares = kAll ^ kDarkSquares;
    };

    // Shift a bitboard in a specified direction, keeping in mind overflow across the board.
    template <Direction kDir>
    constexpr Bitboard Bitboard::shift() const {
        const int shift = static_cast<int>(kDir);
        Bitboard temp = *this;

        // Calculate horizontal component
        const int horr = shift & 0x7;
        if (horr == 1) temp &= ~Bitboards::kFileH;
        else if (horr == 7) temp &= ~Bitboards::kFileA;

        // Do not shift negatively - that is UB
        return shift > 0 ? temp <<  shift
                         : temp >> -shift;
    }

    // Shift a bitboard repeatedly, keeping in mind occupancy (ie, we cannot pass through pieces)
    template<Direction kDir>
    constexpr Bitboard Bitboard::ray(Bitboard occ) const {

        Bitboard res = this->shift<kDir>();

        while (true) {
            const Bitboard temp = (res & ~occ).shift<kDir>();
            if ((res == (res |= temp))) break;
        }

        return res;
    }

    // Biterator utility (thanks to Ciekce)
    // It is syntactic sugar for when we want to iterate through all squares of a bitboard quickly,
    // and allows us to do something like "for (Square sq : bb)"
    class Biterator {
    public:
        constexpr Biterator &operator++() {
            mBB.pop_lsb();
            return *this;
        }

        [[nodiscard]] constexpr Square operator*() const {
            return mBB.lsb();
        }

        [[nodiscard]] constexpr bool operator==(const Biterator &) const = default;

    private:
        Bitboard mBB;

        explicit constexpr Biterator(Bitboard bb) {
            mBB = bb;
        }

        friend class Bitboard;
    };

    constexpr Biterator Bitboard::begin() const {
        return Biterator{*this};
    }

    constexpr Biterator Bitboard::end() const {
        return Biterator{Bitboards::kEmpty};
    }
}
