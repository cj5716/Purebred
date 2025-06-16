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
#include "position.h"
#include "types.h"
#include "zobrist.h"

namespace purebred {
    constexpr void Position::add_piece(const Piece pc, const Square sq) {
        assert(this->mMailbox[sq] == Pieces::kNone);
        assert(!this->piece_bb(pc).get_bit(sq));
        this->mMailbox[sq] = pc;
        this->toggle_keys(pc, sq);
        this->toggle_bbs(pc, sq);
    }

    constexpr void Position::remove_piece(const Piece pc, const Square sq) {
        assert(this->mMailbox[sq] == pc);
        assert(this->piece_bb(pc).get_bit(sq));
        this->mMailbox[sq] = Pieces::kNone;
        this->toggle_keys(pc, sq);
        this->toggle_bbs(pc, sq);
    }

    constexpr void Position::move_piece(const Piece pc, const Square from, const Square to) {
        this->remove_piece(pc, from);
        this->add_piece(pc, to);
    }
}