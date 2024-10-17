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

#include <cstdint>
#include "bitboard.h"
#include "types.h"
#include "utils.h"

constexpr int RookBits   = 12;
constexpr int BishopBits = 9;

inline Array<Bitboard, Colour::NumTypes, Square::NumTypes> PawnAttacks;
inline Array<Bitboard, Square::NumTypes> KnightAttacks;
inline Array<Bitboard, 1 << BishopBits> BishopAttacks;
inline Array<Bitboard, 1 << RookBits> RookAttacks;
inline Array<Bitboard, Square::NumTypes> KingAttacks;

inline Array<Bitboard, Square::NumTypes, Square::NumTypes> LineBBs;
inline Array<Bitboard, Square::NumTypes, Square::NumTypes> BetweenBBs;

constexpr Array<uint64_t, Square::NumTypes> BishopMagics =
{
    UINT64_C(0x0080810410820200), UINT64_C(0x2010520422401000), UINT64_C(0x88A01411A0081800), UINT64_C(0x1001050002610001),
    UINT64_C(0x9000908280000000), UINT64_C(0x20080442A0000001), UINT64_C(0x0221A80045080800), UINT64_C(0x000060200A404000),
    UINT64_C(0x0020100894408080), UINT64_C(0x0800084021404602), UINT64_C(0x0040804100298014), UINT64_C(0x5080201060400011),
    UINT64_C(0x49000620A0000000), UINT64_C(0x8000001200300000), UINT64_C(0x4000008241100060), UINT64_C(0x0000040920160200),
    UINT64_C(0x0042002000240090), UINT64_C(0x000484100420A804), UINT64_C(0x0008000102000910), UINT64_C(0x04880010A8100202),
    UINT64_C(0x0004018804040402), UINT64_C(0x0202100108281120), UINT64_C(0xC201162010101042), UINT64_C(0x0240088022010B80),
    UINT64_C(0x008301600C240814), UINT64_C(0x000028100E142050), UINT64_C(0x0020880000838110), UINT64_C(0x00410800040204A0),
    UINT64_C(0x2012002206008040), UINT64_C(0x004402881900A008), UINT64_C(0x14A80004804C1080), UINT64_C(0xA004814404800F02),
    UINT64_C(0x00C0180230101600), UINT64_C(0x000C905200020080), UINT64_C(0x060400080010404A), UINT64_C(0x00040401080C0100),
    UINT64_C(0x0020121010140040), UINT64_C(0x0000500080000861), UINT64_C(0x8202090241002020), UINT64_C(0x2008022008002108),
    UINT64_C(0x0200402401042000), UINT64_C(0x0002E03210042000), UINT64_C(0x0110040080422400), UINT64_C(0x908404C0584040C0),
    UINT64_C(0x1000204202240408), UINT64_C(0x8002002200200200), UINT64_C(0x2002008101081414), UINT64_C(0x0002080021098404),
    UINT64_C(0x0060110080680000), UINT64_C(0x1080048108420000), UINT64_C(0x0400184014100000), UINT64_C(0x008081A004012240),
    UINT64_C(0x00110080448182A0), UINT64_C(0xA4002000604A4000), UINT64_C(0x0004002811049020), UINT64_C(0x00024A0410A10220),
    UINT64_C(0x0808090089013000), UINT64_C(0x0C80800400805800), UINT64_C(0x0001020100061618), UINT64_C(0x1202820040501008),
    UINT64_C(0x413010050C100405), UINT64_C(0x0004248204042020), UINT64_C(0x0044004408280110), UINT64_C(0x6010220080600502)
};

constexpr Array<uint64_t, Square::NumTypes> RookMagics =
{
    UINT64_C(0x8A80104000800020), UINT64_C(0x0084020100804000), UINT64_C(0x00800A1000048020), UINT64_C(0xC4100020B1000200),
    UINT64_C(0x9400440002080420), UINT64_C(0x0A8004002A801200), UINT64_C(0x0840140C80400100), UINT64_C(0x010000820C412300),
    UINT64_C(0x0910800212400820), UINT64_C(0x0008050190002800), UINT64_C(0x0001080800102000), UINT64_C(0x0041080080201001),
    UINT64_C(0x020820040800890A), UINT64_C(0x0010800200008440), UINT64_C(0x03200800418A0022), UINT64_C(0x0250060600201100),
    UINT64_C(0x4440002400860020), UINT64_C(0x1004402800084000), UINT64_C(0x00041404C0140004), UINT64_C(0x5000400908001400),
    UINT64_C(0x0000020841000830), UINT64_C(0x00830A0101000500), UINT64_C(0x014040A002804040), UINT64_C(0x4400101008854220),
    UINT64_C(0xE008025220022600), UINT64_C(0x0440244008603000), UINT64_C(0x0008024004009000), UINT64_C(0x0801009002100002),
    UINT64_C(0x0400200200010811), UINT64_C(0x3204020044012400), UINT64_C(0x0002100088200100), UINT64_C(0x020800A004091041),
    UINT64_C(0x000210C224200241), UINT64_C(0x00200A0C02040080), UINT64_C(0x004D8028104C0800), UINT64_C(0x813C0A0002900012),
    UINT64_C(0x0008104200208020), UINT64_C(0x240400A000A04080), UINT64_C(0x0802199100100042), UINT64_C(0x062C4C0020100280),
    UINT64_C(0x0020104280800820), UINT64_C(0x20C8010080A80200), UINT64_C(0x1114084080464008), UINT64_C(0x2000025430001805),
    UINT64_C(0x1404C4A100110008), UINT64_C(0x0000008400012008), UINT64_C(0x3045140080022010), UINT64_C(0x8040028410080100),
    UINT64_C(0x0220200310204820), UINT64_C(0x0200082244048202), UINT64_C(0x00090984C0208022), UINT64_C(0x8000110120040900),
    UINT64_C(0x9000402400080084), UINT64_C(0x2402100100038020), UINT64_C(0x0098400600008028), UINT64_C(0x000111000040200C),
    UINT64_C(0x0102402208108102), UINT64_C(0x0440041482204101), UINT64_C(0x4004402000040811), UINT64_C(0x804A000810402002),
    UINT64_C(0x0008000209020401), UINT64_C(0x0440341108009002), UINT64_C(0x0000008825084204), UINT64_C(0x2084002112428402)
};

inline Array<Bitboard, Square::NumTypes> BishopMasks;
inline Array<Bitboard, Square::NumTypes> RookMasks;

inline Bitboard get_bishop_attacks(Square sq)
{
    return BishopMasks[sq];
}

inline Bitboard get_bishop_attacks(Square sq, Bitboard occ)
{
    const Bitboard relevant =  occ & BishopMasks[sq];
    const uint64_t index    = (static_cast<uint64_t>(relevant) * BishopMagics[sq]) >> (64 - BishopBits);
    return BishopAttacks[index];
}

inline Bitboard &get_bishop_index_ref(Square sq, Bitboard occ)
{
    const Bitboard relevant =  occ & BishopMasks[sq];
    const uint64_t index    = (static_cast<uint64_t>(relevant) * BishopMagics[sq]) >> (64 - BishopBits);
    return BishopAttacks[index];
}

inline Bitboard get_rook_attacks(Square sq)
{
    return RookMasks[sq];
}

inline Bitboard get_rook_attacks(Square sq, Bitboard occ)
{
    const Bitboard relevant =  occ & RookMasks[sq];
    const uint64_t index    = (static_cast<uint64_t>(relevant) * RookMagics[sq]) >> (64 - RookBits);
    return RookAttacks[index];
}

inline Bitboard &get_rook_index_ref(Square sq, Bitboard occ)
{
    const Bitboard relevant =  occ & RookMasks[sq];
    const uint64_t index    = (static_cast<uint64_t>(relevant) * RookMagics[sq]) >> (64 - RookBits);
    return RookAttacks[index];
}

inline Bitboard get_queen_attacks(Square sq, Bitboard occ)
{
    return get_bishop_attacks(sq, occ) | get_rook_attacks(sq, occ);
}

inline void init_attacks()
{
    for (Square sq = Square::A1; sq < Square::None; ++sq)
    {
        Bitboard sqBB = square_bb(sq);

        PawnAttacks[Colour::White][sq] = shift_by<Direction::UpLeft  >(sqBB) | shift_by<Direction::UpRight  >(sqBB);
        PawnAttacks[Colour::Black][sq] = shift_by<Direction::DownLeft>(sqBB) | shift_by<Direction::DownRight>(sqBB);

        KnightAttacks[sq]  = Bitboard::Empty;
        KnightAttacks[sq] |= shift_by<Direction::UpLeft   >(shift_by<Direction::Up   >(sqBB));
        KnightAttacks[sq] |= shift_by<Direction::UpLeft   >(shift_by<Direction::Left >(sqBB));
        KnightAttacks[sq] |= shift_by<Direction::UpRight  >(shift_by<Direction::Up   >(sqBB));
        KnightAttacks[sq] |= shift_by<Direction::UpRight  >(shift_by<Direction::Right>(sqBB));
        KnightAttacks[sq] |= shift_by<Direction::DownLeft >(shift_by<Direction::Down >(sqBB));
        KnightAttacks[sq] |= shift_by<Direction::DownLeft >(shift_by<Direction::Left >(sqBB));
        KnightAttacks[sq] |= shift_by<Direction::DownRight>(shift_by<Direction::Down >(sqBB));
        KnightAttacks[sq] |= shift_by<Direction::DownRight>(shift_by<Direction::Right>(sqBB));

        BishopMasks[sq] = ray<Direction::UpLeft>(sqBB) | ray<Direction::UpRight>(sqBB) | ray<Direction::DownLeft>(sqBB) | ray<Direction::DownRight>(sqBB);
        RookMasks  [sq] = ray<Direction::Up    >(sqBB) | ray<Direction::Down   >(sqBB) | ray<Direction::Left    >(sqBB) | ray<Direction::Right    >(sqBB);

        const int bishopBits = count_set_bits(BishopMasks[sq]);
        for (int i = 0; i < (1 << bishopBits); ++i)
        {
            const Bitboard mask = static_cast<Bitboard>(i);
            const Bitboard occ  = pdep(BishopMasks[sq], mask);
            Bitboard &entry = get_bishop_index_ref(sq, occ);
            entry  = Bitboard::Empty;
            entry |= ray<Direction::UpLeft   >(sqBB, occ);
            entry |= ray<Direction::UpRight  >(sqBB, occ);
            entry |= ray<Direction::DownLeft >(sqBB, occ);
            entry |= ray<Direction::DownRight>(sqBB, occ);
        }

        const int rookBits = count_set_bits(RookMasks[sq]);
        for (int i = 0; i < (1 << rookBits); ++i)
        {
            const Bitboard mask = static_cast<Bitboard>(i);
            const Bitboard occ  = pdep(RookMasks[sq], mask);
            Bitboard &entry = get_rook_index_ref(sq, occ);
            entry  = Bitboard::Empty;
            entry |= ray<Direction::Up   >(sqBB, occ);
            entry |= ray<Direction::Down >(sqBB, occ);
            entry |= ray<Direction::Left >(sqBB, occ);
            entry |= ray<Direction::Right>(sqBB, occ);
        }

        KingAttacks[sq] = Bitboard::Empty;
        KingAttacks[sq] |= shift_by<Direction::Up       >(sqBB);
        KingAttacks[sq] |= shift_by<Direction::Down     >(sqBB);
        KingAttacks[sq] |= shift_by<Direction::Left     >(sqBB);
        KingAttacks[sq] |= shift_by<Direction::Right    >(sqBB);
        KingAttacks[sq] |= shift_by<Direction::UpLeft   >(sqBB);
        KingAttacks[sq] |= shift_by<Direction::UpRight  >(sqBB);
        KingAttacks[sq] |= shift_by<Direction::DownLeft >(sqBB);
        KingAttacks[sq] |= shift_by<Direction::DownRight>(sqBB);
    }

    for (Square a = Square::A1; a < Square::None; ++a)
    {
        for (Square b = Square::A1; b < Square::None; ++b)
        {
            BetweenBBs[a][b] = LineBBs[a][b] = Bitboard::Empty;
            Bitboard sqs = square_bb(a) | square_bb(b);

            // You can't have anything between a and b if they are the same
            if (a == b) continue;

            // Orthogonally aligned
            if ((get_rook_attacks(a) & square_bb(b)) != Bitboard::Empty)
            {
                BetweenBBs[a][b] |= get_rook_attacks(a, sqs) & get_rook_attacks(b, sqs);
                LineBBs[a][b] |= get_rook_attacks(a) & get_rook_attacks(b);
            }

            // Diagonally aligned
            if ((get_bishop_attacks(a) & square_bb(b)) != Bitboard::Empty)
            {
                BetweenBBs[a][b] |= get_bishop_attacks(a, sqs) & get_bishop_attacks(b, sqs);
                LineBBs[a][b] |= get_bishop_attacks(a) & get_bishop_attacks(b);
            }
        }
    }
}