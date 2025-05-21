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

#include "bitboard.h"
#include "core.h"
#include "types.h"
#include "utils/mdarray.h"

// We pre-initialise all attack lookups at startup to reduce computation during searching.
namespace purebred::attacks {

    utils::MDArray<Bitboard, Colour::kNumTypes, Square::kNumTypes> pawnAttacks;
    utils::MDArray<Bitboard, Square::kNumTypes> knightAttacks;
    utils::MDArray<Bitboard, Square::kNumTypes> bishopMasks;
    utils::MDArray<Bitboard, Square::kNumTypes> rookMasks;
    utils::MDArray<Bitboard, Square::kNumTypes> kingAttacks;

    // The number of possible arrangements of blockers for each piece type.
    constexpr i32 kBishopRelevantBits = 9;
    constexpr i32 kRookRelevantBits = 12;
    utils::MDArray<Bitboard, 1 << kBishopRelevantBits> bishopAttacks;
    utils::MDArray<Bitboard, 1 << kRookRelevantBits> rookAttacks;

    utils::MDArray<Bitboard, Square::kNumTypes, Square::kNumTypes> lineBB;
    utils::MDArray<Bitboard, Square::kNumTypes, Square::kNumTypes> betweenBB;

    // These 2 arrays of "magic" numbers actually play a big role in fast attack generation;
    // They act as "hashers" to perfectly map all possible arrangements of blockers to the corresponding attack masks.
    // Read more: https://analog-hors.github.io/site/magic-bitboards/
    utils::MDArray<u64, Square::kNumTypes> bishopMagics = {
        U64C(0x0080810410820200), U64C(0x2010520422401000), U64C(0x88A01411A0081800), U64C(0x1001050002610001),
        U64C(0x9000908280000000), U64C(0x20080442A0000001), U64C(0x0221A80045080800), U64C(0x000060200A404000),
        U64C(0x0020100894408080), U64C(0x0800084021404602), U64C(0x0040804100298014), U64C(0x5080201060400011),
        U64C(0x49000620A0000000), U64C(0x8000001200300000), U64C(0x4000008241100060), U64C(0x0000040920160200),
        U64C(0x0042002000240090), U64C(0x000484100420A804), U64C(0x0008000102000910), U64C(0x04880010A8100202),
        U64C(0x0004018804040402), U64C(0x0202100108281120), U64C(0xC201162010101042), U64C(0x0240088022010B80),
        U64C(0x008301600C240814), U64C(0x000028100E142050), U64C(0x0020880000838110), U64C(0x00410800040204A0),
        U64C(0x2012002206008040), U64C(0x004402881900A008), U64C(0x14A80004804C1080), U64C(0xA004814404800F02),
        U64C(0x00C0180230101600), U64C(0x000C905200020080), U64C(0x060400080010404A), U64C(0x00040401080C0100),
        U64C(0x0020121010140040), U64C(0x0000500080000861), U64C(0x8202090241002020), U64C(0x2008022008002108),
        U64C(0x0200402401042000), U64C(0x0002E03210042000), U64C(0x0110040080422400), U64C(0x908404C0584040C0),
        U64C(0x1000204202240408), U64C(0x8002002200200200), U64C(0x2002008101081414), U64C(0x0002080021098404),
        U64C(0x0060110080680000), U64C(0x1080048108420000), U64C(0x0400184014100000), U64C(0x008081A004012240),
        U64C(0x00110080448182A0), U64C(0xA4002000604A4000), U64C(0x0004002811049020), U64C(0x00024A0410A10220),
        U64C(0x0808090089013000), U64C(0x0C80800400805800), U64C(0x0001020100061618), U64C(0x1202820040501008),
        U64C(0x413010050C100405), U64C(0x0004248204042020), U64C(0x0044004408280110), U64C(0x6010220080600502)
    };

    utils::MDArray<u64, Square::kNumTypes> rookMagics = {
        U64C(0x8A80104000800020), U64C(0x0084020100804000), U64C(0x00800A1000048020), U64C(0xC4100020B1000200),
        U64C(0x9400440002080420), U64C(0x0A8004002A801200), U64C(0x0840140C80400100), U64C(0x010000820C412300),
        U64C(0x0910800212400820), U64C(0x0008050190002800), U64C(0x0001080800102000), U64C(0x0041080080201001),
        U64C(0x020820040800890A), U64C(0x0010800200008440), U64C(0x03200800418A0022), U64C(0x0250060600201100),
        U64C(0x4440002400860020), U64C(0x1004402800084000), U64C(0x00041404C0140004), U64C(0x5000400908001400),
        U64C(0x0000020841000830), U64C(0x00830A0101000500), U64C(0x014040A002804040), U64C(0x4400101008854220),
        U64C(0xE008025220022600), U64C(0x0440244008603000), U64C(0x0008024004009000), U64C(0x0801009002100002),
        U64C(0x0400200200010811), U64C(0x3204020044012400), U64C(0x0002100088200100), U64C(0x020800A004091041),
        U64C(0x000210C224200241), U64C(0x00200A0C02040080), U64C(0x004D8028104C0800), U64C(0x813C0A0002900012),
        U64C(0x0008104200208020), U64C(0x240400A000A04080), U64C(0x0802199100100042), U64C(0x062C4C0020100280),
        U64C(0x0020104280800820), U64C(0x20C8010080A80200), U64C(0x1114084080464008), U64C(0x2000025430001805),
        U64C(0x1404C4A100110008), U64C(0x0000008400012008), U64C(0x3045140080022010), U64C(0x8040028410080100),
        U64C(0x0220200310204820), U64C(0x0200082244048202), U64C(0x00090984C0208022), U64C(0x8000110120040900),
        U64C(0x9000402400080084), U64C(0x2402100100038020), U64C(0x0098400600008028), U64C(0x000111000040200C),
        U64C(0x0102402208108102), U64C(0x0440041482204101), U64C(0x4004402000040811), U64C(0x804A000810402002),
        U64C(0x0008000209020401), U64C(0x0440341108009002), U64C(0x0000008825084204), U64C(0x2084002112428402)
    };

    constexpr void init_pawn_attacks() {
        for (Square sq : Squares::kAll) {
            const Bitboard sqBB = Bitboard{sq};
            pawnAttacks[Colours::kWhite][sq] = sqBB.shift<Direction::kUpLeft>() | sqBB.shift<Direction::kUpRight>();
            pawnAttacks[Colours::kBlack][sq] = sqBB.shift<Direction::kDownLeft>() | sqBB.shift<Direction::kDownRight>();
        }
    }

    constexpr Bitboard get_pawn_attacks(Colour c, Square sq) {
        return pawnAttacks[c][sq];
    }

    constexpr void init_knight_attacks() {
        for (Square sq : Squares::kAll) {
            const Bitboard sqBB = Bitboard{sq};

            knightAttacks[sq] = Bitboards::kEmpty;
            knightAttacks[sq] |= sqBB.shift<Direction::kUp>().shift<Direction::kUpLeft>();
            knightAttacks[sq] |= sqBB.shift<Direction::kUp>().shift<Direction::kUpRight>();
            knightAttacks[sq] |= sqBB.shift<Direction::kUpLeft>().shift<Direction::kLeft>();
            knightAttacks[sq] |= sqBB.shift<Direction::kUpRight>().shift<Direction::kRight>();
            knightAttacks[sq] |= sqBB.shift<Direction::kDown>().shift<Direction::kDownLeft>();
            knightAttacks[sq] |= sqBB.shift<Direction::kDown>().shift<Direction::kDownRight>();
            knightAttacks[sq] |= sqBB.shift<Direction::kDownLeft>().shift<Direction::kLeft>();
            knightAttacks[sq] |= sqBB.shift<Direction::kDownRight>().shift<Direction::kRight>();
        }
    }

    constexpr Bitboard get_knight_attacks(Square sq, [[maybe_unused]] Bitboard = Bitboards::kEmpty) {
        return knightAttacks[sq];
    }

    constexpr Bitboard runtime_bishop_attacks(Square sq, Bitboard relevant) {
        const Bitboard sqBB = Bitboard{sq};
        Bitboard temp = Bitboards::kEmpty;

        temp |= sqBB.ray<Direction::kUpLeft>(relevant);
        temp |= sqBB.ray<Direction::kUpRight>(relevant);
        temp |= sqBB.ray<Direction::kDownLeft>(relevant);
        temp |= sqBB.ray<Direction::kDownRight>(relevant);

        return temp;
    }

    constexpr void init_bishop_masks() {
        for (Square sq : Squares::kAll) {
            bishopMasks[sq] = runtime_bishop_attacks(sq, Bitboards::kEmpty);
        }
    }

    constexpr usize get_bishop_index(Square sq, Bitboard relevant) {
        return static_cast<usize>((relevant.raw() * bishopMagics[sq]) >> (64 - kBishopRelevantBits));
    }

    constexpr void init_bishop_lookups() {
        for (Square sq : Squares::kAll) {
            Bitboard occ = Bitboards::kEmpty;
            do {
                bishopAttacks[get_bishop_index(sq, occ)] = runtime_bishop_attacks(sq, occ);
            } while ((occ = bishopMasks[sq].next_subset(occ)));
        }
    }

    constexpr void init_bishop_attacks() {
        init_bishop_masks();
        init_bishop_lookups();
    }

    constexpr Bitboard get_bishop_attacks(Square sq, Bitboard occ = Bitboards::kEmpty) {
        return bishopAttacks[get_bishop_index(sq, occ & bishopMasks[sq])];
    }

    constexpr Bitboard runtime_rook_attacks(Square sq, Bitboard relevant) {
        const Bitboard sqBB = Bitboard{sq};
        Bitboard temp = Bitboards::kEmpty;

        temp |= sqBB.ray<Direction::kUp>(relevant);
        temp |= sqBB.ray<Direction::kDown>(relevant);
        temp |= sqBB.ray<Direction::kLeft>(relevant);
        temp |= sqBB.ray<Direction::kRight>(relevant);

        return temp;
    }

    constexpr void init_rook_masks() {
        for (Square sq : Squares::kAll) {
            rookMasks[sq] = runtime_rook_attacks(sq, Bitboards::kEmpty);
        }
    }

    constexpr usize get_rook_index(Square sq, Bitboard relevant) {
        return static_cast<usize>((relevant.raw() * rookMagics[sq]) >> (64 - kRookRelevantBits));
    }

    constexpr void init_rook_lookups() {
        for (Square sq : Squares::kAll) {
            Bitboard occ = Bitboards::kEmpty;
            do {
                rookAttacks[get_rook_index(sq, occ)] = runtime_rook_attacks(sq, occ);
            } while ((occ = rookMasks[sq].next_subset(occ)));
        }
    }

    constexpr void init_rook_attacks() {
        init_rook_masks();
        init_rook_lookups();
    }

    constexpr Bitboard get_rook_attacks(Square sq, Bitboard occ = Bitboards::kEmpty) {
        return rookAttacks[get_rook_index(sq, occ & rookMasks[sq])];
    }

    constexpr Bitboard get_queen_attacks(Square sq, Bitboard occ = Bitboards::kEmpty) {
        return get_bishop_attacks(sq, occ) | get_rook_attacks(sq, occ);
    }

    constexpr void init_king_attacks() {
        for (Square sq : Squares::kAll) {
            const Bitboard sqBB = Bitboard{sq};

            kingAttacks[sq] |= sqBB.shift<Direction::kUp>();
            kingAttacks[sq] |= sqBB.shift<Direction::kDown>();
            kingAttacks[sq] |= sqBB.shift<Direction::kLeft>();
            kingAttacks[sq] |= sqBB.shift<Direction::kRight>();
            kingAttacks[sq] |= sqBB.shift<Direction::kUpLeft>();
            kingAttacks[sq] |= sqBB.shift<Direction::kUpRight>();
            kingAttacks[sq] |= sqBB.shift<Direction::kDownLeft>();
            kingAttacks[sq] |= sqBB.shift<Direction::kDownRight>();
        }
    }

    constexpr Bitboard get_king_attacks(Square sq, [[maybe_unused]] Bitboard = Bitboards::kEmpty) {
        return kingAttacks[sq];
    }

    constexpr void init_mask_lookups() {
        for (Square s1 : Squares::kAll) {
            for (Square s2 : Squares::kAll) {
                betweenBB[s1][s2] = lineBB[s1][s2] = Bitboards::kEmpty;
                Bitboard sqs = Bitboard{s1} | Bitboard{s2};

                // You can't have anything between s1 and s2 if they are the same
                if (s1 == s2) continue;

                // Diagonally aligned
                if (get_bishop_attacks(s1) & Bitboard{s2}) {
                    betweenBB[s1][s2] |= get_bishop_attacks(s1, sqs) & get_bishop_attacks(s2, sqs);
                    lineBB[s1][s2] |= get_bishop_attacks(s1) & get_bishop_attacks(s2);
                }

                // Orthogonally aligned
                if (get_rook_attacks(s1) & Bitboard{s2}) {
                    betweenBB[s1][s2] |= get_rook_attacks(s1, sqs) & get_rook_attacks(s2, sqs);
                    lineBB[s1][s2] |= get_rook_attacks(s1) & get_rook_attacks(s2);
                }
            }
        }
    }

    constexpr void init() {
        init_pawn_attacks();
        init_knight_attacks();
        init_bishop_attacks();
        init_rook_attacks();
        init_king_attacks();
        init_mask_lookups();
    }
}