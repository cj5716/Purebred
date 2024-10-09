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
#include <bit>
#include <cstdint>
#include "types.h"
#include "utils.h"

enum class Bitboard : uint64_t
{
    FileA = UINT64_C(0x0101010101010101),
    FileB = UINT64_C(0x0202020202020202),
    FileC = UINT64_C(0x0404040404040404),
    FileD = UINT64_C(0x0808080808080808),
    FileE = UINT64_C(0x1010101010101010),
    FileF = UINT64_C(0x2020202020202020),
    FileG = UINT64_C(0x4040404040404040),
    FileH = UINT64_C(0x8080808080808080),

    Rank1 = UINT64_C(0x00000000000000FF),
    Rank2 = UINT64_C(0x000000000000FF00),
    Rank3 = UINT64_C(0x0000000000FF0000),
    Rank4 = UINT64_C(0x00000000FF000000),
    Rank5 = UINT64_C(0x000000FF00000000),
    Rank6 = UINT64_C(0x0000FF0000000000),
    Rank7 = UINT64_C(0x00FF000000000000),
    Rank8 = UINT64_C(0xFF00000000000000),

    Full  = UINT64_C(0xFFFFFFFFFFFFFFFF),
    Empty = UINT64_C(0x0000000000000000)
};

[[nodiscard]] constexpr Bitboard operator|(Bitboard a, uint64_t b) { return static_cast<Bitboard>(static_cast<uint64_t>(a) | b); }
[[nodiscard]] constexpr Bitboard operator&(Bitboard a, uint64_t b) { return static_cast<Bitboard>(static_cast<uint64_t>(a) & b); }
[[nodiscard]] constexpr Bitboard operator^(Bitboard a, uint64_t b) { return static_cast<Bitboard>(static_cast<uint64_t>(a) ^ b); }

constexpr Bitboard &operator|=(Bitboard &a, uint64_t b) { return a = a | b; }
constexpr Bitboard &operator&=(Bitboard &a, uint64_t b) { return a = a & b; }
constexpr Bitboard &operator^=(Bitboard &a, uint64_t b) { return a = a ^ b; }

[[nodiscard]] constexpr Bitboard operator|(Bitboard a, Bitboard b) { return a | static_cast<uint64_t>(b); }
[[nodiscard]] constexpr Bitboard operator&(Bitboard a, Bitboard b) { return a & static_cast<uint64_t>(b); }
[[nodiscard]] constexpr Bitboard operator^(Bitboard a, Bitboard b) { return a ^ static_cast<uint64_t>(b); }

constexpr Bitboard &operator|=(Bitboard &a, Bitboard b) { return a = a | b; }
constexpr Bitboard &operator&=(Bitboard &a, Bitboard b) { return a = a & b; }
constexpr Bitboard &operator^=(Bitboard &a, Bitboard b) { return a = a ^ b; }

[[nodiscard]] constexpr Bitboard operator<<(Bitboard bb, int shift) { return static_cast<Bitboard>(static_cast<uint64_t>(bb) << shift); }
[[nodiscard]] constexpr Bitboard operator>>(Bitboard bb, int shift) { return static_cast<Bitboard>(static_cast<uint64_t>(bb) >> shift); }

constexpr Bitboard &operator<<=(Bitboard &bb, int shift) { return bb = bb << shift; }
constexpr Bitboard &operator>>=(Bitboard &bb, int shift) { return bb = bb >> shift; }

[[nodiscard]] constexpr Bitboard operator~(Bitboard bb) { return static_cast<Bitboard>(~static_cast<uint64_t>(bb)); }

constexpr Array<Bitboard, 8> Files({Bitboard::FileA,
                                    Bitboard::FileB,
                                    Bitboard::FileC,
                                    Bitboard::FileD,
                                    Bitboard::FileE,
                                    Bitboard::FileF,
                                    Bitboard::FileG,
                                    Bitboard::FileH});

constexpr Array<Bitboard, 8> Ranks({Bitboard::Rank1,
                                    Bitboard::Rank2,
                                    Bitboard::Rank3,
                                    Bitboard::Rank4,
                                    Bitboard::Rank5,
                                    Bitboard::Rank6,
                                    Bitboard::Rank7,
                                    Bitboard::Rank8});

[[nodiscard]] constexpr Bitboard square_bb(Square sq) { return static_cast<Bitboard>(1) << static_cast<int>(sq); }
[[nodiscard]] constexpr bool has_multiple_bits_set(Bitboard bb) { return (bb & static_cast<Bitboard>(static_cast<uint64_t>(bb) - 1)) != Bitboard::Empty; }
[[nodiscard]] constexpr bool has_one_bit_set(Bitboard bb) { return bb != Bitboard::Empty && !has_multiple_bits_set(bb); }
[[nodiscard]] constexpr int count_set_bits(Bitboard bb) { return std::popcount(static_cast<uint64_t>(bb)); }
[[nodiscard]] constexpr int get_lsb_index(Bitboard bb)
{
    assert(bb != Bitboard::Empty);
    return std::countr_zero(static_cast<uint64_t>(bb));
}

[[nodiscard]] constexpr Bitboard get_lsb_bb(Bitboard bb)
{
    assert(bb != Bitboard::Empty);
    return bb & static_cast<Bitboard>(static_cast<uint64_t>(~bb) + 1);
}

constexpr void pop_lsb(Bitboard &bb) { bb &= static_cast<Bitboard>(static_cast<uint64_t>(bb) - 1); }

template <Direction Dir>
[[nodiscard]] constexpr Bitboard shift_by(Bitboard bb)
{
    const int shift = static_cast<int>(Dir);
    const int horr = shift & 0x7;
    bb = (horr == 1) ? bb & ~Bitboard::FileH
       : (horr == 7) ? bb & ~Bitboard::FileA
                     : bb;

    // Do not shift negatively - that is UB
    return shift > 0 ? bb <<  shift
                     : bb >> -shift;
}

template <Direction Dir>
[[nodiscard]] constexpr Bitboard ray(Bitboard bb)
{
    Bitboard orig = bb;
    bb |= shift_by<Dir * 1>(bb);
    bb |= shift_by<Dir * 2>(bb);
    bb |= shift_by<Dir * 4>(bb);
    bb ^= orig;
    return bb;
}

template<Direction Dir>
[[nodiscard]] constexpr Bitboard ray(Bitboard bb, Bitboard blockers)
{
    Bitboard res = Bitboard::Empty;
    while (bb != Bitboard::Empty)
    {
        bb = shift_by<Dir>(bb);
        bb &= ~blockers;
        res |= bb;
    }

    return res;
}

[[nodiscard]] constexpr Bitboard pdep(Bitboard bb, Bitboard mask)
{
    Bitboard res = Bitboard::Empty;

    for (Bitboard bit = static_cast<Bitboard>(1); mask != Bitboard::Empty; bit <<= 1)
    {
        if ((bb & bit) != Bitboard::Empty) res |= get_lsb_bb(mask);
        pop_lsb(mask);
    }

    return res;
}