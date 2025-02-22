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

    constexpr i32 kScoreDraw = 0;
    constexpr i32 kScoreMate = 32000;
    constexpr i32 kScoreInf = 32001;
    constexpr i32 kScoreNone = 32002;
    constexpr i32 kScoreMateInMaxPly = kScoreMate - kMaxPly;

    class Colour {
    public:
        constexpr Colour() = default;
        constexpr Colour(auto i) {
            assert(i <= kNoneIdx);
            mData = i;
        }

        [[nodiscard]] constexpr bool operator==(const Colour &) const = default; 

        [[nodiscard]] constexpr Colour flip() const {
            return Colour{mData ^ 1};
        }

        [[nodiscard]] constexpr operator bool() const {
            return mData != kNoneIdx;
        }

        [[nodiscard]] constexpr operator usize() const {
            return static_cast<usize>(mData);
        }

    private:
        u8 mData;

        enum : u8 {
            kWhiteIdx = 0,
            kBlackIdx = 1,
            kNoneIdx = 2
        };

        friend struct Colours;
    };

    struct Colours {
        Colours() = delete;

        static constexpr Colour kWhite{Colour::kWhiteIdx};
        static constexpr Colour kBlack{Colour::kBlackIdx};
        static constexpr Colour kNone{Colour::kNoneIdx};
    };
}