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
#include <string>
#include "bitboard.h"
#include "types.h"
#include "utils.h"

void Board::add_piece(Piece pc, Square sq)
{
    assert(pc != Piece::None);
    assert(sq != Square::None);
    assert(mailbox[sq] == Piece::None);
    assert(bitboards[pc] & square_bb(sq) == 0);
    assert(occupancies[colour_of(pc)] & square_bb(sq) == 0);

    bitboards[pc]              ^= square_bb(sq);
    occupancies[colour_of(pc)] ^= square_bb(sq);
    mailbox[sq]                 = pc;
}

void Board::remove_piece(Piece pc, Square sq)
{
    assert(pc != Piece::None);
    assert(sq != Square::None);
    assert(mailbox[sq] == pc);
    assert(bitboards[pc] & square_bb(sq) != 0);
    assert(occupancies[colour_of(pc)] & square_bb(sq) != 0);

    bitboards[pc]              ^= square_bb(sq);
    occupancies[colour_of(pc)] ^= square_bb(sq);
    mailbox[sq]                 = Piece::None;
}

void Board::move_piece(Piece pc, Square from, Square to)
{
    add_piece(pc, from);
    remove_piece(pc, to);
}

void Position::set_fen(std::string &fen)
{
    // Reset boards
    boards.resize(1);
    boardStates.resize(1);
    state = boardStates.back();
    kingCastleSquares [Colour::White] = Square::None;
    kingCastleSquares [Colour::Black] = Square::None;
    queenCastleSquares[Colour::White] = Square::None;
    queenCastleSquares[Colour::White] = Square::None;

    // Clear current board
    state.reset();

    // Split the FEN into the 4 main parts
    std::vector<std::string> tokens = split_string(fen, " ");
    const std::string pieceStr  = tokens[0];
    const std::string stmStr    = tokens[1];
    const std::string castleStr = tokens[2];
    const std::string epStr     = tokens[3];

    // Keep track of the 2 optional parts
    const std::string hundredPly     = tokens.size() > 4 ? tokens[4] : "0";
    const std::string totalPlies     = tokens.size() > 5 ? tokens[5] : "0";

    int fenCounter = 0;
    for (int rank = 7; rank >= 0; --rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            const Square sq = make_square(rank, file);
            const char currChar = pieceStr[fenCounter];

            // Convert each char in the string to a piece
            if (   ('a' <= currChar && currChar <= 'z')
                || ('A' <= currChar && currChar <= 'Z'))
            {
                fenCounter++;
                const Piece pc = piece_from_char(currChar);
                if (pc != Piece::None)
                    state.add_piece(pc, sq);
            }

            // FENs use numbers to denote empty squares
            if ('0' <= currChar && currChar <= '9')
            {
                // Figure out how many empty squares
                int offset = currChar - '0';

                // If our current square is empty, we incremented 1 during iteration for nothing; subtract 1 from the offset
                const Piece pc = state.piece_on(sq);
                if (pc == Piece::None) offset--;

                // Adjust file coutner based on number of empty squares
                file += offset;
                fenCounter++;
            }

            // '/' Denotes a new rank
            if (pieceStr[fenCounter] == '/') fenCounter++;
        }
    }

    // Set the side to move
    state.sideToMove = stmStr == "w" ? Colour::White : Colour::Black;

    // Parse castling rights
    // TODO: Handle DFRC and FRC using state.
    kingCastleSquares [Colour::White] = Square::H1;
    kingCastleSquares [Colour::Black] = Square::H8;
    queenCastleSquares[Colour::White] = Square::A1;
    queenCastleSquares[Colour::Black] = Square::A8;

    // Set the relevant rights whenever we detect them within the FEN
    for (const char c : castleStr)
    {
        if (c == 'K')      state.board.castlingRights[Colour::White] |= CastlingRights::King;
        else if (c == 'k') state.board.castlingRights[Colour::Black] |= CastlingRights::King;
        else if (c == 'Q') state.board.castlingRights[Colour::White] |= CastlingRights::Queen;
        else if (c == 'q') state.board.castlingRights[Colour::Black] |= CastlingRights::Queen;
    }

    // Set the EP square if present
    state.enPassant = [&]() {

        // Empty field, EP not legal
        if (epStr == "-") return Square::None;

        // Detect it from the string
        const int file = epStr[0] - 'a';
        const int rank = epStr[1] - '0';
        return make_square(rank, file);
    }();

    // Set the 50 move counter (or 100 ply counter)
    state.hundredPlyCount = std::stoi(hundredPly);

    // Set the total ply counter
    state.totalPlyCount = std::stoi(totalPlies);
}

void display_board() const
{
    std::cout << "\n";

    for (int rank = 7; rank >= 0; --rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            Square sq = make_square(rank, file);

            if (file == 0)
                std::cout << "  " << rank << " ";

            const Piece pc = state.piece_on(sq);
        }

        std::cout << "\n";
    }

    std::cout << "\n     a b c d e f g h\n\n";

    std::cout << "     Side:     ";
    std::cout << state.sideToMove == Colour::White ? "White"
                                                   : "Black";
    std::cout << "\n";
};