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

#include "attacks.h"
#include "bitboard.h"
#include "core.h"
#include "move.h"
#include "types.h"
#include "utils/mdarray.h"
#include "zobrist.h"

#include <concepts>

namespace purebred {
    class Position {
    public:
        constexpr Position() {
            this->mMainKey = this->mPawnKey = 0;
            this->mNonPawnKeys.fill(0);

            this->mOccBBs.fill(Bitboards::kEmpty);
            this->mPieceTypeBBs.fill(Bitboards::kEmpty);
            this->mMailbox.fill(Pieces::kNone);

            this->mKingBlockerBBs.fill(Bitboards::kEmpty);
            this->mPinnerBBs.fill(Bitboards::kEmpty);
            this->mCheckers = Bitboards::kEmpty;

            this->mStm = Colours::kNone;
            this->mGamePly = this->mFiftyMrPly = 0;

            this->mCastlingSquares.fill(Squares::kNone);
            this->mEnPassantSq = Squares::kNone;
        }

        constexpr Position(const Position &) = default;
        [[nodiscard]] constexpr bool operator==(const Position &) const = default;

        [[nodiscard]] constexpr u64 key() const {
            return this->mMainKey;
        }

        [[nodiscard]] constexpr u64 pawn_key() const {
            return this->mPawnKey;
        }

        [[nodiscard]] constexpr Bitboard occupancy_bb(const Colour c) const {
            return this->mOccBBs[c];
        }

        [[nodiscard]] constexpr Bitboard occupancy_bb() const {
            return this->occupancy_bb(Colours::kWhite) | this->occupancy_bb(Colours::kBlack);
        }

        [[nodiscard]] constexpr Bitboard piece_type_bb(const PieceType pt) const {
            return this->mPieceTypeBBs[pt];
        }

        [[nodiscard]] constexpr Bitboard piece_types_bb(const std::same_as<PieceType> auto... pts) const {
            return (this->piece_type_bb(pts) & ...);
        }

        [[nodiscard]] constexpr Bitboard piece_bb(const Colour c, const PieceType pt) const {
            return this->occupancy_bb(c) & this->piece_type_bb(pt);
        }

        [[nodiscard]] constexpr Bitboard piece_bb(const Piece pc) const {
            return piece_bb(pc.colour(), pc.type());
        }

        [[nodiscard]] constexpr Bitboard pieces_bb(const Colour c, const std::same_as<PieceType> auto... pts) const {
            return this->occupancy_bb(c) & this->piece_types_bb(pts...);
        }

        [[nodiscard]] constexpr Bitboard pieces_bb(const std::same_as<Piece> auto... pcs) const {
            return (this->piece_bb(pcs) & ...);
        }

        [[nodiscard]] constexpr Piece piece_on(const Square sq) const {
            return this->mMailbox[sq];
        }

        [[nodiscard]] constexpr Square king_sq(const Colour c) const {
            return this->piece_bb(c, PieceTypes::kKing).lsb();
        }

        [[nodiscard]] constexpr u64 king_blockers_bb(const Colour c) const {
            return this->mKingBlockerBBs[c];
        }

        [[nodiscard]] constexpr u64 checkers_bb() const {
            return this->mCheckers;
        }

        [[nodiscard]] constexpr Colour side_to_move() const {
            return this->mStm;
        }

        [[nodiscard]] constexpr u8 fifty_mr_ply() const {
            return this->mFiftyMrPly;
        }

        [[nodiscard]] constexpr u16 game_ply() const {
            return this->mGamePly;
        }

        [[nodiscard]] constexpr Square en_passant_sq() const {
            return this->mEnPassantSq;
        }

        [[nodiscard]] constexpr Bitboard non_kp_bb() const {
            return ~piece_types_bb(PieceTypes::kPawn, PieceTypes::kKing);
        }

        [[nodiscard]] constexpr bool has_non_kp() const {
            return this->non_kp_bb();
        }

        [[nodiscard]] constexpr bool has_non_kp(const Colour c) const {
            return this->occupancy_bb(c) & this->non_kp_bb();
        }

        [[nodiscard]] constexpr bool is_quiet(const Move move) const {
            const Move::Type moveType = move.type();
            switch (moveType) {
                case Move::Type::kCastling:
                    return true;

                case Move::Type::kPromotion:
                case Move::Type::kEnPassant:
                    return false;

                default:
                    assert(moveType == Move::Type::kNormal);
                    return !this->piece_on(move.to());
            }
        }

        [[nodiscard]] constexpr Bitboard slider_attackers_to(const Square sq, const Bitboard occ) const {
            const Bitboard diag = attacks::get_bishop_attacks(sq, occ) & this->piece_types_bb(PieceTypes::kBishop, PieceTypes::kQueen);
            const Bitboard orth = attacks::get_rook_attacks(sq, occ) & this->piece_types_bb(PieceTypes::kRook, PieceTypes::kQueen);
            return diag | orth;
        }

        [[nodiscard]] constexpr Bitboard slider_attackers_to(const Colour c, const Square sq, const Bitboard occ) const {
            return this->slider_attackers_to(sq, occ) & this->occupancy_bb(c);
        }

        [[nodiscard]] constexpr Bitboard nonpawn_attackers_to(const Square sq, const Bitboard occ) const {
            const Bitboard knight = attacks::get_knight_attacks(sq) & this->piece_type_bb(PieceTypes::kKnight);
            return knight | this->slider_attackers_to(sq, occ);
        }

        [[nodiscard]] constexpr Bitboard nonpawn_attackers_to(const Colour c, const Square sq, const Bitboard occ) const {
            return this->nonpawn_attackers_to(sq, occ) & this->occupancy_bb(c);
        }

        [[nodiscard]] constexpr Bitboard pawn_attackers_to(const Colour c, const Square sq) const {
            return attacks::get_pawn_attacks(c.flip(), sq) & this->piece_bb(c, PieceTypes::kPawn);
        }

        [[nodiscard]] constexpr Bitboard attackers_to(const Square sq, const Bitboard occ) const {
            const Bitboard wp = this->pawn_attackers_to(Colours::kWhite, sq);
            const Bitboard bp = this->pawn_attackers_to(Colours::kBlack, sq);
            const Bitboard rest = this->nonpawn_attackers_to(sq, occ);
            return wp | bp | rest;
        }

        [[nodiscard]] constexpr Bitboard attackers_to(const Colour c, const Square sq, const Bitboard occ) const {
            const Bitboard p = this->pawn_attackers_to(c, sq);
            const Bitboard rest = this->nonpawn_attackers_to(c, sq, occ);
            return p | rest;
        }

        [[nodiscard]] static constexpr Position from_fen(std::string &fen);

    private:
        u64 mMainKey;
        u64 mPawnKey;
        utils::MDArray<u64, Colour::kNumTypes> mNonPawnKeys;

        utils::MDArray<Bitboard, Colour::kNumTypes> mOccBBs;
        utils::MDArray<Bitboard, Piece::kNumTypes> mPieceTypeBBs;
        utils::MDArray<Piece, Square::kNumTypes> mMailbox;

        utils::MDArray<Bitboard, Colour::kNumTypes> mKingBlockerBBs;
        utils::MDArray<Bitboard, Colour::kNumTypes> mPinnerBBs;
        Bitboard mCheckers;

        Colour mStm;
        u16 mGamePly;
        u8 mFiftyMrPly;

        utils::MDArray<Square, Colour::kNumTypes, CastlingSide::kNumTypes> mCastlingSquares;
        Square mEnPassantSq;

        constexpr void toggle_keys(const Piece pc, const Square sq) {
            const u64 delta = zobrist::key(pc, sq);
            this->mMainKey ^= delta;
            if (pc.type() == PieceTypes::kPawn) this->mPawnKey ^= delta;
            else this->mNonPawnKeys[pc.colour()] ^= delta;
        }

        constexpr void toggle_bbs(const Piece pc, const Square sq) {
            this->mOccBBs[pc.colour()].toggle_bit(sq);
            this->mPieceTypeBBs[pc.type()].toggle_bit(sq);
        }

        constexpr void flip_stm();
        constexpr void set_en_passant_sq(const Square sq);
        constexpr void set_castling_squares(const auto &newCastlingSquares);

        constexpr void add_piece(const Piece pc, const Square sq);
        constexpr void remove_piece(const Piece pc, const Square sq);
        constexpr void move_piece(const Piece pc, const Square from, const Square to);

        template <bool kWhite>
        constexpr void calc_pins();
        constexpr void calc_checkers();
        constexpr void calc_masks();

    };
}

