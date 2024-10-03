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
#include <cstdint>
#include "bitboard.h"
#include "types.h"
#include "utils.h"

struct Board
{
    Array<Bitboard, Piece::NumTypes>        bitboards;
    Array<Bitboard, Colour::NumTypes>       occupancies;
    Array<Piece, Square::NumTypes>          mailbox;
    Array<Bitboard, Colour::NumTypes>       threats;
    Array<CastlingRights, Colour::NumTypes> castlingRights;

    inline void reset()
    {
        for (Bitboard       &bb : bitboards)      bb = Bitboard::Empty;
        for (Bitboard       &bb : occupancies)    bb = Bitboard::Empty;
        for (Piece          &pc : mailbox)        pc = Piece::None;
        for (Bitboard       &bb : threats)        bb = Bitboard::Empty;
        for (CastlingRights &cr : castlingRights) cr = CastlingRights::None;
    }

    inline Piece piece_on(Square sq) const { return mailbox[sq]; }

    inline void add_piece(Piece pc, Square sq)
    {
        assert(pc != Piece::None);
        assert(sq != Square::None);
        assert(mailbox[sq] == Piece::None);
        assert((bitboards[pc] & square_bb(sq)) == Bitboard::Empty);
        assert((occupancies[colour_of(pc)] & square_bb(sq)) == Bitboard::Empty);

        bitboards[pc]              ^= square_bb(sq);
        occupancies[colour_of(pc)] ^= square_bb(sq);
        mailbox[sq]                 = pc;
    }

    inline void remove_piece(Piece pc, Square sq)
    {
        assert(pc != Piece::None);
        assert(sq != Square::None);
        assert(mailbox[sq] == pc);
        assert((bitboards[pc] & square_bb(sq)) != Bitboard::Empty);
        assert((occupancies[colour_of(pc)] & square_bb(sq)) != Bitboard::Empty);

        bitboards[pc]              ^= square_bb(sq);
        occupancies[colour_of(pc)] ^= square_bb(sq);
        mailbox[sq]                 = Piece::None;
    }


    inline void move_piece(Piece pc, Square from, Square to)
    {
        add_piece(pc, from);
        remove_piece(pc, to);
    }
};

struct BoardState
{
    Bitboard checkers;
    Bitboard checkMask;
    Bitboard pinned;
    int      totalPlyCount;
    int      hundredPlyCount;
    Square   enPassant;
    Colour   sideToMove;

    inline void add_piece(Piece pc, Square sq){ ; }
    inline void remove_piece(Piece pc, Square sq) { ; }
    inline void move_piece(Piece pc, Square from, Square to)
    {
        add_piece(pc, from);
        remove_piece(pc, to);
    }

    inline void reset()
    {
        checkers        = Bitboard::Empty;
        checkMask       = Bitboard::Empty;
        pinned          = Bitboard::Empty;
        totalPlyCount   = 0;
        hundredPlyCount = 0;
        enPassant       = Square::None;
        sideToMove      = Colour::None;
    }
};

struct Position
{
    ArrayVec<Board, MaxPly>      boards;
    ArrayVec<BoardState, MaxPly> boardStates;

    Array<Square, 2> kingCastleSquares;
    Array<Square, 2> queenCastleSquares;

    void set_fen(std::string fen);
    void display_board() const;
    inline Board &board() { return boards.back(); }
    inline const Board &board() const { return boards.back(); }
    inline BoardState &state() { return boardStates.back(); }
    inline const BoardState &state() const { return boardStates.back(); }
    inline void add_piece(Piece pc, Square sq)
    {
        state().add_piece(pc, sq);
        board().add_piece(pc, sq);
    }

    inline void remove_piece(Piece pc, Square sq)
    {
        state().remove_piece(pc, sq);
        board().remove_piece(pc, sq);
    }

    inline void move_piece(Piece pc, Square from, Square to)
    {
        add_piece(pc, from);
        remove_piece(pc, to);
    }

    inline void reset()
    {
        boards.resize(1);
        boardStates.resize(1);
        board().reset();
        state().reset();
    }
};