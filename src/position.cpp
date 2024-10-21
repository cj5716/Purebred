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
#include <iostream>
#include <string>
#include "attacks.h"
#include "bitboard.h"
#include "move.h"
#include "position.h"
#include "types.h"
#include "utils.h"

void Position::set_fen(std::string fen)
{
    // Reset boards
    reset();

    // Split the FEN into the 4 main parts
    const std::vector<std::string> tokens = split_string(fen, " ");
    const std::string boardStr  = tokens[0];
    const std::string stmStr    = tokens[1];
    const std::string castleStr = tokens[2];
    const std::string epStr     = tokens[3];

    // Keep track of the 2 optional parts
    const std::string hundredPly = tokens.size() > 4 ? tokens[4] : "0";
    const std::string totalPlies = tokens.size() > 5 ? tokens[5] : "1";

    int currRank = 7;
    const std::vector<std::string> ranks = split_string(boardStr, "/");
    for (const std::string &rank : ranks)
    {
        int currFile = 0;
        for (char c : rank)
        {
            const Piece pc = piece_from_char(c);
            if (pc == Piece::None)
            {
                std::string dummy;
                dummy += c;
                currFile += std::stoi(dummy);
            }
            else
            {
                add_piece(pc, make_square(currRank, currFile));
                currFile++;
            }
        }

        currRank--;
    }

    // Set the side to move
    state().sideToMove = stmStr == "w" ? Colour::White : Colour::Black;

    // Parse castling rights
    castleSquares[CastlingRights::King ][Colour::White] = Square::H1;
    castleSquares[CastlingRights::Queen][Colour::White] = Square::A1;
    castleSquares[CastlingRights::King ][Colour::Black] = Square::H8;
    castleSquares[CastlingRights::Queen][Colour::Black] = Square::A8;

    // Set the relevant rights whenever we detect them within the FEN
    for (char c : castleStr)
    {
        // Convert the castling rights into DFRC form
        if (c == 'K')      c = 'H';
        else if (c == 'k') c = 'h';
        else if (c == 'Q') c = 'A';
        else if (c == 'q') c = 'a';

        // Uppercase indicates white castling rights
        if ('A' <= c && c <= 'H')
        {
            // Consider it a kingside or queenside castle depending on whether it is to the left or the right of the king
            int castleFile = c - 'A';
            CastlingRights side = castleFile < file_of(board().king_sq(Colour::White)) ? CastlingRights::Queen
                                                                                       : CastlingRights::King;
            castleSquares[side][Colour::White] = make_square(0, castleFile);
            board().castlingRights[Colour::White] |= side;
        }
        // Lowercase indicates black castling rights
        else if ('a' <= c && c <= 'h')
        {
            // Consider it a kingside or queenside castle depending on whether it is to the left or the right of the king
            int castleFile = c - 'a';
            CastlingRights side = castleFile < file_of(board().king_sq(Colour::Black)) ? CastlingRights::Queen
                                                                                       : CastlingRights::King;
            castleSquares[side][Colour::Black] = make_square(7, castleFile);
            board().castlingRights[Colour::Black] |= side;
        }
    }

    // Set the EP square if present
    state().enPassant = [&]() {

        // Empty field, EP not legal
        if (epStr == "-") return Square::None;

        // Detect it from the string
        const int file = epStr[0] - 'a';
        const int rank = epStr[1] - '0';
        return make_square(rank, file);
    }();

    // Set the 50 move counter (or 100 ply counter)
    state().hundredPlyCount = std::stoi(hundredPly);

    // Set the total ply counter
    state().totalPlyCount = std::stoi(totalPlies);

    update_masks();
}

void Position::display_board() const
{
    std::cout << "\n";

    for (int rank = 7; rank >= 0; --rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            Square sq = make_square(rank, file);

            if (file == 0)
                std::cout << "  " << rank << " ";

            const Piece pc = board().piece_on(sq);
            std::cout << " " << piece_to_char(pc);
        }

        std::cout << "\n";
    }

    std::cout << "\n     a b c d e f g h\n\n";

    std::cout << "     STM:       ";
    std::cout << (state().sideToMove == Colour::White ? "White"
                                                      : "Black");

    std::cout << "\n";

    std::cout << "     EP SQUARE: ";
    std::cout << square_to_string(state().enPassant);

    std::cout << "\n";

    std::cout << "     CASTLING:  ";

    // White king castle
    if ((board().castlingRights[Colour::White] & CastlingRights::King) != CastlingRights::None)
        std::cout << char('A' + file_of(castleSquares[CastlingRights::King][Colour::White]));

    // White queen castle
    if ((board().castlingRights[Colour::White] & CastlingRights::Queen) != CastlingRights::None)
        std::cout << char('A' + file_of(castleSquares[CastlingRights::Queen][Colour::White]));

    // Black king castle
    if ((board().castlingRights[Colour::Black] & CastlingRights::King) != CastlingRights::None)
        std::cout << char('a' + file_of(castleSquares[CastlingRights::King][Colour::Black]));

    // Black queen castle
    if ((board().castlingRights[Colour::Black] & CastlingRights::Queen) != CastlingRights::None)
        std::cout << char('a' + file_of(castleSquares[CastlingRights::Queen][Colour::Black]));

    std::cout << "\n";
};

constexpr void Position::update_masks()
{
    Colour   stm     = state().sideToMove;
    Colour   nstm    = flip(stm);
    Square   ksq     = board().king_sq(stm);
    Bitboard stmOcc  = board().occupancies[stm];
    Bitboard nstmOcc = board().occupancies[nstm];

    // Take these bitboards by reference for convenience later on
    Bitboard &checkers  = state().checkers;
    Bitboard &checkMask = state().checkMask;
    Bitboard &pinned    = state().pinned;

    // Clear the masks we are about to update
    checkers = checkMask = pinned = Bitboard::Empty;

    // Non-sliding checkers
    Bitboard pawnCheckers   = board().bitboards[make_piece(nstm, PieceType::Pawn)]   & PawnAttacks[stm][ksq];
    Bitboard knightCheckers = board().bitboards[make_piece(nstm, PieceType::Knight)] & KnightAttacks[ksq];
    checkers = checkMask = pawnCheckers | knightCheckers;

    // Sliding checkers/pinners
    // Note: We do not count our own pieces in the occupancy for sliding piece calculation for the time being.
    // This is part of a clever trick so that we can count the number of friendly blockers between the potential attacker and our king.
    Bitboard slidingAttacks = Bitboard::Empty;
    slidingAttacks |= board().bitboards[make_piece(nstm, PieceType::Bishop)] & get_bishop_attacks(ksq, nstmOcc);
    slidingAttacks |= board().bitboards[make_piece(nstm, PieceType::Rook)  ] & get_rook_attacks(ksq, nstmOcc);
    slidingAttacks |= board().bitboards[make_piece(nstm, PieceType::Queen) ] & get_queen_attacks(ksq, nstmOcc);

    while (slidingAttacks != Bitboard::Empty)
    {
        Square sq = get_lsb_index(slidingAttacks);
        Bitboard sqBB = square_bb(sq);
        pop_lsb(slidingAttacks);

        Bitboard betweenBB   = BetweenBBs[sq][ksq];
        Bitboard stmBlockers = betweenBB & stmOcc;

        // No friendly blockers between opponent attacker and our king; we are in check
        if (stmBlockers == Bitboard::Empty)
        {
            checkers |= sqBB;
            checkMask |= sqBB | betweenBB;
        }

        // Exactly 1 friendly blocker between opponent attacker and king, which is the pinned piece
        else if (!has_multiple_bits_set(stmBlockers))
            pinned |= stmBlockers;
    }
};

constexpr bool Position::is_legal(Move move) const
{
    return true;
};