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
#include "utils/rng.h"

namespace purebred::zobrist {

    // We use some clever packing to encode the positional keys:
    // Observe that a pawns can never be on the 1st or 8th rank;
    // In addition, as we store white and black pawn keys contiguously,
    // we thus have 16 contiguous unused keys from (white pawn, h1) to (black pawn, a8)
    // which we can reuse to hash the 16 = 2^4 possible castling-rights combinations.
    // The E.P. keys are stored by file; we will use the (white pawn, a1) to (white pawn, a8) slots.
    // The STM key will just be (black pawn, h1).
    // The 50mr keys occupy (black pawn, h2) to (black pawn, h8).
    constexpr usize totalKeys = Piece::kNumTypes * Square::kNumTypes;
    constexpr usize stmOffset = Pieces::kBlackPawn.raw() * Square::kNumTypes + Squares::kH1.raw();
    constexpr usize enPassantOffset = Pieces::kWhitePawn.raw() * Square::kNumTypes + Squares::kA1.raw();
    constexpr usize castlingOffset = Pieces::kWhitePawn.raw() * Square::kNumTypes + Squares::kH1.raw();

    inline utils::MDArray<u64, Piece::kNumTypes * Square::kNumTypes> keys;

    constexpr void init() {

        utils::RNG rng{U64C(0xB913C7EF6E09D5DF)};

        for (Piece pc : Pieces::kAll) {
            for (Square sq : Squares::kAll) {
                keys[pc.raw() * Square::kNumTypes + sq.raw()] = rng.next();
            }
        }
    }

    constexpr u64 key(const Piece pc, const Square sq) {
        return keys[pc.raw() * Square::kNumTypes + sq.raw()];
    }

    constexpr u64 stm_key() {
        return keys[stmOffset];
    }

    constexpr u64 en_passant_key(const Square sq) {
        return sq ? keys[enPassantOffset + sq.file()] : 0;
    }

    constexpr u64 castling_key(const auto &castlingRights) {
        return keys[
               castlingOffset
            +  static_cast<bool>(castlingRights[Colours::kWhite][CastlingSide::kQueen])
            + (static_cast<bool>(castlingRights[Colours::kWhite][CastlingSide::kKing]) << 1)
            + (static_cast<bool>(castlingRights[Colours::kBlack][CastlingSide::kQueen]) << 2)
            + (static_cast<bool>(castlingRights[Colours::kBlack][CastlingSide::kKing]) << 3)
        ];
    }
}