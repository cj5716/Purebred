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
#include <cassert>
#include <cstdint>
#include <string>

constexpr std::string_view Name   = "Purebred v0.01";
constexpr std::string_view Author = "cj5716";

constexpr std::string_view StartPos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
constexpr int              MaxPly   = 256;

enum class Square : int
{
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    None = 64, NumTypes = 64
};


enum class Colour : int
{
    White, Black,
    None = 2, NumTypes = 2
};

enum class PieceType : int
{
    Pawn, Knight, Bishop, Rook, Queen, King,
    None = 6, NumTypes = 6
};

enum class Piece : int
{
    WhitePawn  , BlackPawn  ,
    WhiteKnight, BlackKnight,
    WhiteBishop, BlackBishop,
    WhiteRook  , BlackRook  ,
    WhiteQueen , BlackQueen ,
    WhiteKing  , BlackKing  ,
    None = 12  , NumTypes = 12
};

enum class Direction : int
{
    Up    = 8,
    Down  = -Up,
    Right = 1,
    Left  = -Right,

    UpLeft    = Up + Left,
    UpRight   = Up + Right,
    DownLeft  = Down + Left,
    DownRight = Down + Right
};

enum class CastlingRights : uint8_t
{
    None  = 0b00,
    King  = 0b01,
    Queen = 0b10,
    All   = King | Queen
};

[[nodiscard]] constexpr Square operator+(Square a, Square b) { return static_cast<Square>(static_cast<int>(a) + static_cast<int>(b)); }
[[nodiscard]] constexpr Square operator-(Square a, Square b) { return static_cast<Square>(static_cast<int>(a) - static_cast<int>(b)); }
[[nodiscard]] constexpr Square operator^(Square a, Square b) { return static_cast<Square>(static_cast<int>(a) ^ static_cast<int>(b)); }
[[nodiscard]] constexpr Square operator|(Square a, Square b) { return static_cast<Square>(static_cast<int>(a) | static_cast<int>(b)); }
[[nodiscard]] constexpr Square operator&(Square a, Square b) { return static_cast<Square>(static_cast<int>(a) & static_cast<int>(b)); }

[[nodiscard]] constexpr Square operator+(Square a, int b)    { return a + static_cast<Square>(b); }
[[nodiscard]] constexpr Square operator-(Square a, int b)    { return a - static_cast<Square>(b); }
[[nodiscard]] constexpr Square operator^(Square a, int b)    { return a ^ static_cast<Square>(b); }
[[nodiscard]] constexpr Square operator|(Square a, int b)    { return a | static_cast<Square>(b); }
[[nodiscard]] constexpr Square operator&(Square a, int b)    { return a & static_cast<Square>(b); }

[[nodiscard]] constexpr Square operator<<(Square a, int b)   { return static_cast<Square>(static_cast<int>(a) << b); }
[[nodiscard]] constexpr Square operator>>(Square a, int b)   { return static_cast<Square>(static_cast<int>(a) >> b); }

[[nodiscard]] constexpr Square operator+(Square sq, Direction dir) { return sq + static_cast<int>(dir); }
[[nodiscard]] constexpr Square operator-(Square sq, Direction dir) { return sq - static_cast<int>(dir); }

[[nodiscard]] constexpr Direction operator+(Direction a, Direction b) { return static_cast<Direction>(static_cast<int>(a) + static_cast<int>(b)); }
[[nodiscard]] constexpr Direction operator-(Direction a, Direction b) { return static_cast<Direction>(static_cast<int>(a) - static_cast<int>(b)); }
[[nodiscard]] constexpr Direction operator*(Direction dir, int mult)  { return static_cast<Direction>(static_cast<int>(dir) * mult); }

[[nodiscard]] constexpr Piece operator+(Piece a, Piece b) { return static_cast<Piece>(static_cast<int>(a) + static_cast<int>(b)); }
[[nodiscard]] constexpr Piece operator-(Piece a, Piece b) { return static_cast<Piece>(static_cast<int>(a) - static_cast<int>(b)); }
constexpr Piece &operator++(Piece &pc) { return pc = pc + static_cast<Piece>(1); }
constexpr Piece &operator--(Piece &pc) { return pc = pc - static_cast<Piece>(1); }

[[nodiscard]] constexpr PieceType operator+(PieceType a, PieceType b) { return static_cast<PieceType>(static_cast<int>(a) + static_cast<int>(b)); }
[[nodiscard]] constexpr PieceType operator-(PieceType a, PieceType b) { return static_cast<PieceType>(static_cast<int>(a) - static_cast<int>(b)); }
constexpr PieceType &operator++(PieceType &pt) { return pt = pt + static_cast<PieceType>(1); }
constexpr PieceType &operator--(PieceType &pt) { return pt = pt - static_cast<PieceType>(1); }

constexpr Square &operator++(Square &sq) { return sq = sq + 1; }
constexpr Square &operator--(Square &sq) { return sq = sq - 1; }

constexpr Colour &operator++(Colour &colour) { return colour = static_cast<Colour>(static_cast<int>(colour) + 1); }
constexpr Colour &operator--(Colour &colour) { return colour = static_cast<Colour>(static_cast<int>(colour) - 1); }

[[nodiscard]] constexpr CastlingRights operator|(CastlingRights a, CastlingRights b) { return static_cast<CastlingRights>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b)); }
[[nodiscard]] constexpr CastlingRights operator&(CastlingRights a, CastlingRights b) { return static_cast<CastlingRights>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b)); }
[[nodiscard]] constexpr CastlingRights operator~(CastlingRights cr) { return static_cast<CastlingRights>(~static_cast<uint8_t>(cr)); }

constexpr CastlingRights &operator|=(CastlingRights &a, CastlingRights b) { return a = a | b; }
constexpr CastlingRights &operator&=(CastlingRights &a, CastlingRights b) { return a = a & b; }

[[nodiscard]] constexpr Colour orient(Colour a, Colour b)
{
    assert(a != Colour::None);
    assert(b != Colour::None);
    return static_cast<Colour>(static_cast<int>(a) ^ static_cast<int>(b));
}

[[nodiscard]] constexpr Colour flip(Colour colour)
{
    return orient(colour, Colour::Black);
}

[[nodiscard]] constexpr Square make_square(int rank, int file)
{
    assert(rank < 8);
    assert(file < 8);
    return static_cast<Square>(rank << 3 | file);
}

[[nodiscard]] constexpr int rank_of(Square square)
{
    assert(square != Square::None);
    return static_cast<int>(square >> 3);
}

[[nodiscard]] constexpr int file_of(Square square)
{
    assert(square != Square::None);
    return static_cast<int>(square & 0b111);
}

[[nodiscard]] constexpr Square orient(Square sq, Colour c)
{
    assert(sq != Square::None);
    return sq ^ (static_cast<int>(c) * 0b111000);
}

[[nodiscard]] constexpr Square flip(Square sq)
{
    assert(sq != Square::None);
    return orient(sq, Colour::Black);
}

[[nodiscard]] constexpr Square mirror(Square sq)
{
    assert(sq != Square::None);
    return sq ^ 0b000111;
}

[[nodiscard]] constexpr Piece make_piece(Colour colour, PieceType pieceType)
{
    assert(colour != Colour::None);
    assert(pieceType != PieceType::None);
    return static_cast<Piece>(static_cast<int>(pieceType) << 1 | static_cast<int>(colour));
}

[[nodiscard]] constexpr Colour colour_of(Piece piece)
{
    assert(piece != Piece::None);
    return static_cast<Colour>(static_cast<int>(piece) & 0b1);
}

[[nodiscard]] constexpr PieceType type_of(Piece piece)
{
    assert(piece != Piece::None);
    return static_cast<PieceType>(static_cast<int>(piece) >> 1);
}

[[nodiscard]] constexpr Piece orient(Piece piece, Colour colour)
{
    assert(piece != Piece::None);
    return make_piece(orient(colour_of(piece), colour), type_of(piece));
}

[[nodiscard]] constexpr Piece flip(Piece piece)
{
    assert(piece != Piece::None);
    return orient(piece, Colour::Black);
}

[[nodiscard]] constexpr Piece piece_from_char(char c)
{
    switch (c)
    {
        case 'P': return Piece::  WhitePawn;
        case 'p': return Piece::  BlackPawn;
        case 'N': return Piece::WhiteKnight;
        case 'n': return Piece::BlackKnight;
        case 'B': return Piece::WhiteBishop;
        case 'b': return Piece::BlackBishop;
        case 'R': return Piece::  WhiteRook;
        case 'r': return Piece::  BlackRook;
        case 'Q': return Piece:: WhiteQueen;
        case 'q': return Piece:: BlackQueen;
        case 'K': return Piece::  WhiteKing;
        case 'k': return Piece::  BlackKing;
        default : return Piece::       None;
    }
}

[[nodiscard]] constexpr char piece_to_char(Piece piece)
{
    switch (piece)
    {
        case Piece::       None: return ' ';
        case Piece::  WhitePawn: return 'P';
        case Piece::  BlackPawn: return 'p';
        case Piece::WhiteKnight: return 'N';
        case Piece::BlackKnight: return 'n';
        case Piece::WhiteBishop: return 'B';
        case Piece::BlackBishop: return 'b';
        case Piece::  WhiteRook: return 'R';
        case Piece::  BlackRook: return 'r';
        case Piece:: WhiteQueen: return 'Q';
        case Piece:: BlackQueen: return 'q';
        case Piece::  WhiteKing: return 'K';
        case Piece::  BlackKing: return 'k';
        default: return ' ';
    }
}

[[nodiscard]] constexpr PieceType piecetype_from_char(char c)
{
    return type_of(piece_from_char(c));
}

[[nodiscard]] constexpr char piecetype_to_char(PieceType pt)
{
    return piece_to_char(make_piece(Colour::White, pt));
}

[[nodiscard]] constexpr std::string square_to_string(Square sq)
{
    if (sq == Square::None) return "--";

    std::string ret{};
    ret += 'a' + file_of(sq);
    ret += '1' + rank_of(sq);
    return ret;
}

[[nodiscard]] constexpr Direction flip(Direction dir)
{
    switch (dir)
    {
        case Direction::Up       : return Direction::Down;
        case Direction::Down     : return Direction::Up;
        case Direction::Left     : return Direction::Right;
        case Direction::Right    : return Direction::Left;
        case Direction::UpLeft   : return Direction::DownLeft;
        case Direction::UpRight  : return Direction::DownRight;
        case Direction::DownLeft : return Direction::UpLeft;
        case Direction::DownRight: return Direction::UpRight;
        default: assert(false);
    }
}

[[nodiscard]] constexpr Direction orient(Direction dir, Colour colour)
{
    assert(colour != Colour::None);
    return colour == Colour::White ? dir
                                   : flip(dir);
}