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

#include <bit>
#include <cstdint>
#include "types.h"
#include "utils.h"

enum class Bitboard : uint64_t
{
    FileA = static_cast<uint64_t>(0x0101010101010101),
    FileB = static_cast<uint64_t>(0x0202020202020202),
    FileC = static_cast<uint64_t>(0x0404040404040404),
    FileD = static_cast<uint64_t>(0x0808080808080808),
    FileE = static_cast<uint64_t>(0x1010101010101010),
    FileF = static_cast<uint64_t>(0x2020202020202020),
    FileG = static_cast<uint64_t>(0x4040404040404040),
    FileH = static_cast<uint64_t>(0x8080808080808080),

    Rank1 = static_cast<uint64_t>(0x00000000000000FF),
    Rank2 = static_cast<uint64_t>(0x000000000000FF00),
    Rank3 = static_cast<uint64_t>(0x0000000000FF0000),
    Rank4 = static_cast<uint64_t>(0x00000000FF000000),
    Rank5 = static_cast<uint64_t>(0x000000FF00000000),
    Rank6 = static_cast<uint64_t>(0x0000FF0000000000),
    Rank7 = static_cast<uint64_t>(0x00FF000000000000),
    Rank8 = static_cast<uint64_t>(0xFF00000000000000),

    Full  = static_cast<uint64_t>(0xFFFFFFFFFFFFFFFF),
    Empty = static_cast<uint64_t>(0x0000000000000000)
};

[[nodiscard]] constexpr Bitboard operator|(Bitboard a, Bitboard b) { return static_cast<Bitboard>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b)); }
[[nodiscard]] constexpr Bitboard operator&(Bitboard a, Bitboard b) { return static_cast<Bitboard>(static_cast<uint64_t>(a) & static_cast<uint64_t>(b)); }
[[nodiscard]] constexpr Bitboard operator^(Bitboard a, Bitboard b) { return static_cast<Bitboard>(static_cast<uint64_t>(a) ^ static_cast<uint64_t>(b)); }

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
[[nodiscard]] constexpr int get_lsb(Bitboard bb)
{
    assert(bb != 0);
    return std::countr_zero(static_cast<uint64_t>(bb));
}

constexpr void pop_lsb(Bitboard &bb) { bb &= static_cast<Bitboard>(static_cast<uint64_t>(bb) - 1); }