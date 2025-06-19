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

#include "bitboard.h"
#include "core.h"
#include "position.h"
#include "types.h"
#include "zobrist.h"

#include <sstream>
#include <string>

namespace purebred {
    constexpr void Position::add_piece(const Piece pc, const Square sq) {
        assert(pc != Pieces::kNone);
        assert(this->mMailbox[sq] == Pieces::kNone);
        assert(!this->piece_bb(pc).get_bit(sq));
        this->mMailbox[sq] = pc;
        this->mKeys.toggle_piece(pc, sq);
        this->toggle_bbs(pc, sq);
    }

    constexpr void Position::remove_piece(const Piece pc, const Square sq) {
        assert(pc != Pieces::kNone);
        assert(this->mMailbox[sq] == pc);
        assert(this->piece_bb(pc).get_bit(sq));
        this->mMailbox[sq] = Pieces::kNone;
        this->mKeys.toggle_piece(pc, sq);
        this->toggle_bbs(pc, sq);
    }

    constexpr void Position::move_piece(const Piece pc, const Square from, const Square to) {
        assert(pc != Pieces::kNone);
        assert(this->mMailbox[from] == pc);
        assert(this->piece_bb(pc).get_bit(from));
        assert(this->mMailbox[to] == Pieces::kNone);
        assert(!this->piece_bb(pc).get_bit(to));

        std::swap(this->mMailbox[from], this->mMailbox[to]);
        this->mKeys.move_piece(pc, from, to);
        this->toggle_bbs(pc, from, to);
    }

    Position Position::from_fen(std::string &fen) {

        // The structure of the FEN specification is as follows:
        // <board> <stm> <castling rights> <en passant> <halfmove clock> <fullmove number>
        Position pos{};
        std::istringstream stream(fen);

        // Board
        std::string boardStr; stream >> boardStr;
        i32 s = 56;
        for (char c : boardStr) {
            if (c == '/') s -= 16;
            else if (std::isdigit(c)) {
                s += c - '0';
            }
            else {
                const Square sq = Square{s};
                const Piece pc = Piece::from_char(c);
                pos.add_piece(pc, sq);
                s++;
            }

            if (s < 0) break;
        }

        // STM
        std::string stmStr; stream >> stmStr;
        assert(stmStr.size() == 1);
        pos.set_stm(Colour{stmStr[0]});

        // Castling Rights
        std::string crStr; stream >> crStr;
        for (char cr : crStr) {
            if (cr == '-') break;

            const Colour c = std::isupper(cr) ? Colours::kWhite : Colours::kBlack;
            cr = std::tolower(cr);

            // Un-specialcase the stuff we do in standard chess
            if (cr == 'k') cr = 'h';
            if (cr == 'q') cr = 'a';

            const Square sq = Square{Ranks::k1, cr - 'a'}.orient(c);
            const CastlingSide side = sq.raw() > pos.king_sq(c).raw() ? CastlingSide::kKing : CastlingSide::kQueen;
            pos.set_castling_square(c, side, sq);
        }
        pos.mKeys.toggle_castling_rights(pos.mCastlingSquares);

        // En Passant;
        std::string epStr; stream >> epStr;
        if (epStr != "-") pos.set_en_passant_square(Square::from_str(epStr));

        // Halfmove clock
        stream >> pos.mFiftyMrPly;

        // Fullmove number
        u16 fullmoves; stream >> fullmoves;
        pos.mGamePly = std::max(2 * (fullmoves - 1), 0) + (pos.mStm == Colours::kBlack);

        // Final setup
        pos.update_masks();

        return pos;
    }

    [[nodiscard]] std::string Position::to_pretty_str() const {
        std::string res;

        res += '+';
        for (i32 file = Files::kA; file <= Files::kH; ++file) res += "---+";
        res += '\n';

        for (i32 rank = Ranks::k8; rank >= Ranks::k1; --rank) {

            res += '|';
            for (i32 file = Files::kA; file <= Files::kH; ++file) {
                const Square sq = Square{rank, file};
                res += ' ';
                res += this->piece_on(sq).to_char();
                res += " |";
            }
            res += '\n';

            res += '+';
            for (i32 file = Files::kA; file <= Files::kH; ++file) res += "---+";
            res += '\n';
        }

        if (this->en_passant_square()) {
            res += "EP Square: ";
            res += this->en_passant_square().to_str();
            res += '\n';
        }

        return res;
    }

}