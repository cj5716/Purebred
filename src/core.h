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
#include <regex>
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

        static constexpr Score kDraw = 0;
        static constexpr Score kMate = 32000;
        static constexpr Score kInf  = 32001;
        static constexpr Score kNone = 32002;
        static constexpr Score kMateInMaxPly = kMate - kMaxPly;
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

        [[nodiscard]] static constexpr Colour from_char(char c) {
            for (usize i = 0; i < kNumTypes; ++i) {
                if (colourToChar[i] == c) return Colour{i};
            }
            return Colour{kNoneIdx};
        }

        [[nodiscard]] constexpr char to_char() const {
            if (mData == kNoneIdx) return ' ';
            return colourToChar[mData];
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

        static constexpr utils::MDArray<char, kNumTypes> colourToChar = {'w', 'b'};

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

    class Square {

        enum : u8 {
            kA1Idx, kB1Idx, kC1Idx, kD1Idx, kE1Idx, kF1Idx, kG1Idx, kH1Idx,
            kA2Idx, kB2Idx, kC2Idx, kD2Idx, kE2Idx, kF2Idx, kG2Idx, kH2Idx,
            kA3Idx, kB3Idx, kC3Idx, kD3Idx, kE3Idx, kF3Idx, kG3Idx, kH3Idx,
            kA4Idx, kB4Idx, kC4Idx, kD4Idx, kE4Idx, kF4Idx, kG4Idx, kH4Idx,
            kA5Idx, kB5Idx, kC5Idx, kD5Idx, kE5Idx, kF5Idx, kG5Idx, kH5Idx,
            kA6Idx, kB6Idx, kC6Idx, kD6Idx, kE6Idx, kF6Idx, kG6Idx, kH6Idx,
            kA7Idx, kB7Idx, kC7Idx, kD7Idx, kE7Idx, kF7Idx, kG7Idx, kH7Idx,
            kA8Idx, kB8Idx, kC8Idx, kD8Idx, kE8Idx, kF8Idx, kG8Idx, kH8Idx,
            kNoneIdx
        };

    public:

        static constexpr usize kNumTypes = static_cast<usize>(kNoneIdx);

        [[nodiscard]] constexpr Square() = default;
        [[nodiscard]] constexpr bool operator==(const Square &) const = default;

        constexpr Square(auto i) {
            assert(i <= kNoneIdx);
            mData = i;
        }

        constexpr Square(auto r, auto f) {
            assert(0 <= r && r < 8);
            assert(0 <= f && f < 8);
            mData = (r << 3 | f);
        }

        [[nodiscard]] static constexpr Square from_str(std::string &str) {

            // As regex is not yet implemented constexpr in C++20, do the matching manually instead
            bool match = [&]() {
                if (str.size() != 2) return false;
                if (str[0] < 'a' || str[0] > 'h') return false;
                if (str[1] < '1' || str[1] > '8') return false;
                return true;
            }();

            if (!match) return Square{kNoneIdx};

            return Square{str[1] - '1', str[0] - 'a'};
        }

        [[nodiscard]] constexpr std::string to_str() const {
            assert(mData != kNoneIdx);

            std::string str;
            str += 'a' + this->file();
            str += '1' + this->rank();

            return str;
        }

        [[nodiscard]] constexpr u8 rank() const {
            assert(mData != kNoneIdx);
            return mData >> 3;
        }

        [[nodiscard]] constexpr u8 file() const {
            assert(mData != kNoneIdx);
            return mData & 0x7;
        }

        [[nodiscard]] constexpr Square orient(Colour c) const {
            assert(mData != kNoneIdx);
            return Square{mData ^ (c.raw() * 0b111'000)};
        }

        [[nodiscard]] constexpr Square flip() const {
            return this->orient(Colours::kBlack);
        }

        [[nodiscard]] constexpr Square mirror() const {
            assert(mData != kNoneIdx);
            return Square{mData ^ 0b000'111};
        }

        [[nodiscard]] constexpr operator bool() const {
            return mData != kNoneIdx;
        }

        [[nodiscard]] constexpr operator usize() const {
            return static_cast<usize>(mData);
        }

    private:
        u8 mData;

        friend struct Squares;
    };

    struct Squares {
        constexpr Squares() = delete;

        static constexpr utils::MDArray<Square, Square::kNumTypes> all = []() {
            utils::MDArray<Square, Square::kNumTypes> inner;
            for (usize i = 0; i < Square::kNumTypes; ++i) inner[i] = Square{i};
            return inner;
        }();

        static constexpr Square kA1 = all[Square::kA1Idx], kA2 = all[Square::kA2Idx], kA3 = all[Square::kA3Idx], kA4 = all[Square::kA4Idx];
        static constexpr Square kA5 = all[Square::kA5Idx], kA6 = all[Square::kA6Idx], kA7 = all[Square::kA7Idx], kA8 = all[Square::kA8Idx];
        static constexpr Square kB1 = all[Square::kB1Idx], kB2 = all[Square::kB2Idx], kB3 = all[Square::kB3Idx], kB4 = all[Square::kB4Idx];
        static constexpr Square kB5 = all[Square::kB5Idx], kB6 = all[Square::kB6Idx], kB7 = all[Square::kB7Idx], kB8 = all[Square::kB8Idx];
        static constexpr Square kC1 = all[Square::kC1Idx], kC2 = all[Square::kC2Idx], kC3 = all[Square::kC3Idx], kC4 = all[Square::kC4Idx];
        static constexpr Square kC5 = all[Square::kC5Idx], kC6 = all[Square::kC6Idx], kC7 = all[Square::kC7Idx], kC8 = all[Square::kC8Idx];
        static constexpr Square kD1 = all[Square::kD1Idx], kD2 = all[Square::kD2Idx], kD3 = all[Square::kD3Idx], kD4 = all[Square::kD4Idx];
        static constexpr Square kD5 = all[Square::kD5Idx], kD6 = all[Square::kD6Idx], kD7 = all[Square::kD7Idx], kD8 = all[Square::kD8Idx];
        static constexpr Square kE1 = all[Square::kE1Idx], kE2 = all[Square::kE2Idx], kE3 = all[Square::kE3Idx], kE4 = all[Square::kE4Idx];
        static constexpr Square kE5 = all[Square::kE5Idx], kE6 = all[Square::kE6Idx], kE7 = all[Square::kE7Idx], kE8 = all[Square::kE8Idx];
        static constexpr Square kF1 = all[Square::kF1Idx], kF2 = all[Square::kF2Idx], kF3 = all[Square::kF3Idx], kF4 = all[Square::kF4Idx];
        static constexpr Square kF5 = all[Square::kF5Idx], kF6 = all[Square::kF6Idx], kF7 = all[Square::kF7Idx], kF8 = all[Square::kF8Idx];
        static constexpr Square kG1 = all[Square::kG1Idx], kG2 = all[Square::kG2Idx], kG3 = all[Square::kG3Idx], kG4 = all[Square::kG4Idx];
        static constexpr Square kG5 = all[Square::kG5Idx], kG6 = all[Square::kG6Idx], kG7 = all[Square::kG7Idx], kG8 = all[Square::kG8Idx];
        static constexpr Square kH1 = all[Square::kH1Idx], kH2 = all[Square::kH2Idx], kH3 = all[Square::kH3Idx], kH4 = all[Square::kH4Idx];
        static constexpr Square kH5 = all[Square::kH5Idx], kH6 = all[Square::kH6Idx], kH7 = all[Square::kH7Idx], kH8 = all[Square::kH8Idx];
        static constexpr Square kNone = Square{Square::kNoneIdx};
    };
}