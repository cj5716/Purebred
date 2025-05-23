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

#include "core.h"
#include "types.h"

namespace purebred {

    // Moves are structured as follows:
    // Bits 0-5: From-square index
    // Bits 6-11: To-square index
    // Bits 12-13: Move Type (Normal, EP, Castling, Promotion)
    // Bits 14-15: Promotion type (offset by Knight value, so 0, 1, 2, 3 maps to Knight, Bishop, Rook, Queen)
    class Move {

        // As the from-square is located in the lowest bits, no shifting is needed for it.
        static constexpr i32 kToShift = 6;
        static constexpr i32 kTypeShift = 12;
        static constexpr i32 kPromoShift = 14;

        // As the promotion type is located in the highest bits, no masking is needed for it.
        static constexpr u16 kFromMask = 0x3F;
        static constexpr u16 kToMask = 0x3F << kToShift;
        static constexpr u16 kTypeMask = 0b11 << kTypeShift;

    public:
        // We pre-shift the Move Types in the enum to save on shifting at runtime.
        enum class Type : u16 {
            kNormal = 0b00 << kTypeShift,
            kEnPassant = 0b01 << kTypeShift,
            kCastling = 0b10 << kTypeShift,
            kPromotion = 0b11 << kTypeShift
        };

        explicit constexpr Move() = default;
        explicit constexpr Move(u16 data) {
            mData = data;
        }
        [[nodiscard]] constexpr bool operator==(const Move &) const = default;

        [[nodiscard]] constexpr u16 raw() const {
            return mData;
        }

        template<Type kType>
        [[nodiscard]] static constexpr Move create(Square from, Square to, PieceType promo = PieceTypes::kNone) {
            const u16 baseData = from.raw() | static_cast<u16>(to.raw()) << kToShift | static_cast<u16>(kType);
            if constexpr (kType != Type::kPromotion) {
                return Move{baseData};
            }

            const u16 promoData = baseData | (static_cast<u16>(promo.raw() - PieceTypes::kKnight.raw())) << kPromoShift;
            return Move{promoData};
        }

        [[nodiscard]] constexpr Square from() const {
            return Square{mData & kFromMask};
        }

        [[nodiscard]] constexpr Square to() const {
            return Square{(mData & kToMask) >> kToShift};
        }

        [[nodiscard]] constexpr Type type() const {
            return static_cast<Type>(mData & kTypeMask);
        }

        [[nodiscard]] constexpr PieceType promo_type() const {
            assert(this->type() == Type::kPromotion);
            return PieceType{(mData >> kPromoShift) + PieceTypes::kKnight.raw()};
        }

        [[nodiscard]] constexpr bool castle_is_kingside() const {
            assert(this->type() == Type::kCastling);
            return this->to().raw() > this->from().raw();
        }

        [[nodiscard]] constexpr Square castle_king_to() const {
            assert(this->type() == Type::kCastling);
            return Square{this->from().rank(), this->castle_is_kingside() ? Files::kG : Files::kC};
        }

        [[nodiscard]] constexpr Square castle_rook_to() const {
            assert(this->type() == Type::kCastling);
            return Square{this->from().rank(), this->castle_is_kingside() ? Files::kF : Files::kD};
        }

        template <bool kChess960>
        [[nodiscard]] constexpr std::string to_str() const {
            std::string s;
            s += this->from().to_str();

            if (!kChess960 && this->type() == Type::kCastling)
                s += this->castle_king_to().to_str();
            else
                s += this->to().to_str();

            if (this->type() == Type::kPromotion)
                s += this->promo_type().to_char();

            return s;
        }

    private:
        u16 mData;
    };

    struct Moves {
        constexpr Moves() = delete;

        static constexpr Move kNone{};
    };
}