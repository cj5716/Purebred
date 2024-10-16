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

#include <cstdint>
#include <string>
#include "position.h"
#include "types.h"

enum class MoveType : uint8_t
{
    Quiet = 0b0000,
    DoublePush = 0b0001,
    KingCastle = 0b0010,
    QueenCastle = 0b0011,
    Capture = 0b0100,
    EnPassant = 0b0101,
    KnightPromo = 0b1000,
    BishopPromo = 0b1001,
    RookPromo = 0b1010,
    QueenPromo = 0b1011
};

[[nodiscard]] constexpr is_valid(MoveType mt) { return mt != static_cast<MoveType>(0b0110) && mt != static_cast<MoveType>(0b0111); }
[[nodiscard]] constexpr MoveType operator&(MoveType a, MoveType b) { return static_cast<MoveType>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b)); }
[[nodiscard]] constexpr MoveType operator|(MoveType a, MoveType b) { return static_cast<MoveType>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b)); }

struct Position;
class PackedMove;
class Move
{
public:
    constexpr Move() { data = 0; }
    constexpr Move(uint32_t d) { data = d; }
    constexpr Move(Square from, Square to, Piece pc, MoveType mt)
    {
        data = static_cast<uint32_t>(from) | static_cast<uint32_t>(to) << 6 | static_cast<uint32_t>(mt) << 12 | static_cast<uint32_t>(pc) << 16;
    }
    constexpr Move(PackedMove &packed, Position &pos);

    constexpr uint32_t raw() const { return data; }
    static constexpr Move None() { return Move(0); }

    constexpr Square from_sq() const { return static_cast<Square>(data & 0x3F); }
    constexpr Square to_sq()   const { return static_cast<Square>((data >> 6) & 0x3F); }
    constexpr Piece  piece()   const { return static_cast<Piece>(data >> 16); }

    constexpr bool is_ep() const { return move_type() == MoveType::EnPassant; }
    constexpr bool is_double_push() const { return move_type() == MoveType::DoublePush; }
    constexpr bool is_castle() const { return move_type() == MoveType::KingCastle || move_type() == MoveType::QueenCastle; }
    constexpr bool is_capture() const { return static_cast<bool>(move_type() & MoveType::Capture); }
    constexpr bool is_promo() const { return static_cast<bool>(move_type() & MoveType::KnightPromo); }
    constexpr bool is_noisy() const { return is_capture() || is_promo(); }

    constexpr PieceType promo_type() const
    {
        assert(is_promo());
        return static_cast<PieceType>(move_type() & static_cast<MoveType>(0b0011)) + PieceType::Knight;
    }

    constexpr std::string as_string() const
    {
        std::string ret{};
        const Square from = from_sq();
        const Square to   = to_sq();

        ret += square_to_string(from);
        ret += square_to_string(to);
        if (is_promo()) ret += piecetype_to_char(promo_type());

        return ret;
    }

    [[nodiscard]] constexpr bool operator==(const Move &move) const { return data == move.raw(); }
    [[nodiscard]] constexpr bool operator!=(const Move &move) const { return data != move.raw(); }

private:
    uint32_t data;
    constexpr MoveType move_type() const { return static_cast<MoveType>((data >> 12) & 0xF); }
};

class PackedMove
{
public:
    constexpr PackedMove() { data = 0; }
    constexpr PackedMove(uint16_t d) { data = d; }
    constexpr PackedMove(Move move) { data = move.raw() & 0xFFFF; }

    constexpr Square from_sq() const { return static_cast<Square>(data & 0x3F); }
    constexpr Square to_sq()   const { return static_cast<Square>((data >> 6) & 0x3F); }

    constexpr uint16_t raw() const { return data; }
    constexpr PackedMove None() const { return PackedMove(0); }

    [[nodiscard]] constexpr bool operator==(const PackedMove &packed) const { return data == packed.raw(); }
    [[nodiscard]] constexpr bool operator!=(const PackedMove &packed) const { return data != packed.raw(); }

private:
    uint16_t data;
};

constexpr Move::Move(PackedMove &packed, Position &pos)
{
    data = static_cast<uint32_t>(packed.raw()) | static_cast<uint32_t>(pos.board().piece_on(packed.to_sq())) << 6;
}