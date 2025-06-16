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

#include "../types.h"

namespace purebred::utils {

    // Implements the splitmix64 PRNG by Sebastiano Vigna.
    class RNG {
    public:
        constexpr RNG() = default;
        constexpr RNG(u64 seed) {
            this->mState = seed;
        }

        [[nodiscard]] constexpr u64 next() {
            u64 z = (this->mState += U64C(0x9E3779B97F4A7C15));
            z = (z ^ (z >> 30)) * U64C(0xBF58476D1CE4E5B9);
            z = (z ^ (z >> 27)) * U64C(0x94D049BB133111EB);
            return z ^ (z >> 31);
        }

    private:
        u64 mState{};
    };
}