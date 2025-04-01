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

#include "types.h"
#include "utils/mdarray.h"

#include <cassert>
#include <string_view>

namespace purebred {

    constexpr std::string_view kName = "Purebred v0.01";
    constexpr std::string_view kAuthor = "cj5716";

    constexpr std::string_view kStartPosFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    constexpr usize kMaxPly = 256;
    constexpr usize kMaxMoves = 256;

    using Score = i32;

    struct Scores {
        constexpr Scores() = delete;

        constexpr Score kDraw = 0;
        constexpr Score kMate = 32000;
        constexpr Score kInf  = 32001;
        constexpr Score kNone = 32002;
        constexpr Score kMateInMaxPly = kMate - kMaxPly;
    };

    class Colour {

        enum : u8 {
            kWhiteIdx = 0,
            kBlackIdx = 1,
            kNoneIdx = 2
        };

    public:

        static constexpr usize kNumTypes = static_cast<usize>(kNoneIdx);

        [[nodiscard]] constexpr Colour() = default;
        [[nodiscard]] constexpr bool operator==(const Colour &) const = default;

        constexpr Colour(auto i) {
            assert(i <= kNoneIdx);
            mData = i;
        }

        [[nodiscard]] constexpr Colour flip() const {
            return Colour{mData ^ 1};
        }

        [[nodiscard]] constexpr usize raw() const {
            return static_cast<usize>(mData);
        }

        [[nodiscard]] constexpr operator bool() const {
            return mData != kNoneIdx;
        }

        [[nodiscard]] constexpr operator usize() const {
            return this->raw();
        }

    private:
        u8 mData;

        friend struct Colours;
    };

    struct Colours {
        constexpr Colours() = delete;

        static constexpr Colour kWhite{Colour::kWhiteIdx};
        static constexpr Colour kBlack{Colour::kBlackIdx};
        static constexpr Colour kNone{Colour::kNoneIdx};
    };

    class PieceType {

        enum : u8 {
            kPawnIdx = 0,
            kKnightIdx = 1,
            kBishopIdx = 2,
            kRookIdx = 3,
            kQueenIdx = 4,
            kKingIdx = 5,
            kNoneIdx = 6
        };

    public:

        static constexpr usize kNumTypes = static_cast<usize>(kNoneIdx);

        [[nodiscard]] constexpr PieceType() = default;
        [[nodiscard]] constexpr bool operator==(const PieceType &) const = default;

        constexpr PieceType(auto i) {
            assert(i <= kNoneIdx);
            mData = i;
        }

        [[nodiscard]] static constexpr PieceType from_char(char c) {
            for (usize i = 0; i < kNumTypes; ++i) {
                if (ptToChar[i] == c) return PieceType{i};
            }
            return PieceType{kNoneIdx};
        }

        [[nodiscard]] constexpr char to_char() const {
            if (mData == kNoneIdx) return ' ';
            return ptToChar[mData];
        }

        [[nodiscard]] constexpr bool is_major() const {
            return mData == kRookIdx || mData == kQueenIdx;
        }

        [[nodiscard]] constexpr bool is_minor() const {
            return mData == kKnightIdx || mData == kBishopIdx;
        }

        [[nodiscard]] constexpr bool can_be_promoted_to() const {
            return mData >= kKnightIdx && mData <= kQueenIdx;
        }

        [[nodiscard]] constexpr usize raw() const {
            return static_cast<usize>(mData);
        }

        [[nodiscard]] constexpr operator bool() const {
            return mData != kNoneIdx;
        }

        [[nodiscard]] constexpr operator usize() const {
            return raw();
        }

    private:
        u8 mData;

        static constexpr utils::MDArray<char, kNumTypes> ptToChar = {'p', 'n', 'b', 'r', 'q', 'k'};

        friend struct PieceTypes;
    };

    struct PieceTypes {
        constexpr PieceTypes() = delete;

        static constexpr PieceType kPawn{PieceType::kPawnIdx};
        static constexpr PieceType kKnight{PieceType::kKnightIdx};
        static constexpr PieceType kBishop{PieceType::kBishopIdx};
        static constexpr PieceType kRook{PieceType::kRookIdx};
        static constexpr PieceType kQueen{PieceType::kQueenIdx};
        static constexpr PieceType kKing{PieceType::kKingIdx};
        static constexpr PieceType kNone{PieceType::kNoneIdx};
    };

    class Piece {

        enum : u8 {
            kWhitePawnIdx = 0,
            kBlackPawnIdx = 1,
            kWhiteKnightIdx = 2,
            kBlackKnightIdx = 3,
            kWhiteBishopIdx = 4,
            kBlackBishopIdx = 5,
            kWhiteRookIdx = 6,
            kBlackRookIdx = 7,
            kWhiteQueenIdx = 8,
            kBlackQueenIdx = 9,
            kWhiteKingIdx = 10,
            kBlackKingIdx = 11,
            kNoneIdx = 12
        };

    public:

        static constexpr usize kNumTypes = static_cast<usize>(kNoneIdx);

        [[nodiscard]] constexpr Piece() = default;
        [[nodiscard]] constexpr bool operator==(const Piece &) const = default;

        constexpr Piece(auto i) {
            assert(i <= kNoneIdx);
            mData = i;
        }

        constexpr Piece(Colour colour, PieceType pt) {
            assert(colour != Colours::kNone);
            assert(pt != PieceTypes::kNone);
            mData = colour.raw() | (pt.raw() << 1);
        }

        [[nodiscard]] static constexpr Piece from_char(char c) {
            for (usize i = 0; i < kNumTypes; ++i) {
                if (pcToChar[i] == c) return Piece{i};
            }
            return Piece{kNoneIdx};
        }

        [[nodiscard]] constexpr char to_char() const {
            if (mData == kNoneIdx) return ' ';
            return pcToChar[mData];
        }

        [[nodiscard]] constexpr Colour colour() const {
            return Colour{mData & 0x1};
        }

        [[nodiscard]] constexpr PieceType type() const {
            return PieceType{mData >> 1};
        }

        [[nodiscard]] constexpr Piece flip() const {
            return Piece{mData ^ 0x1};
        }

        [[nodiscard]] constexpr bool is_major() const {
            return this->type().is_major();
        }

        [[nodiscard]] constexpr bool is_minor() const {
            return this->type().is_minor();
        }

        [[nodiscard]] constexpr operator bool() const {
            return mData != kNoneIdx;
        }

        [[nodiscard]] constexpr operator usize() const {
            return static_cast<usize>(mData);
        }

    private:
        u8 mData;

        static constexpr utils::MDArray<char, kNumTypes> pcToChar = {'P', 'p', 'N', 'n', 'B', 'b', 'R', 'r', 'Q', 'q', 'K', 'k'};

        friend struct Pieces;
    };

    struct Pieces {
        constexpr Pieces() = delete;

        static constexpr Piece kWhitePawn{Piece::kWhitePawnIdx};
        static constexpr Piece kBlackPawn{Piece::kBlackPawnIdx};
        static constexpr Piece kWhiteKnight{Piece::kWhiteKnightIdx};
        static constexpr Piece kBlackKnight{Piece::kBlackKnightIdx};
        static constexpr Piece kWhiteBishop{Piece::kWhiteBishopIdx};
        static constexpr Piece kBlackBishop{Piece::kBlackBishopIdx};
        static constexpr Piece kWhiteRook{Piece::kWhiteRookIdx};
        static constexpr Piece kBlackRook{Piece::kBlackRookIdx};
        static constexpr Piece kWhiteQueen{Piece::kWhiteQueenIdx};
        static constexpr Piece kBlackQueen{Piece::kBlackQueenIdx};
        static constexpr Piece kWhiteKing{Piece::kWhiteKingIdx};
        static constexpr Piece kBlackKing{Piece::kBlackKingIdx};
        static constexpr Piece kNone{Piece::kNoneIdx};
    };
}