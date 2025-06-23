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

#include "position.h"
#include "types.h"

namespace purebred {

    template <typename Communicator>
    class Engine {
    public:

        constexpr Engine() = default;
        constexpr Engine(i32 argc, char **argv) {
            this->mCommunicator = Communicator{argc, argv};
        }

        inline void set_fen(std::string &fen) {
            this->mPos = Position::from_fen(fen);
        }

        inline const Position get_root_pos() const {
            return this->mPos;
        }

        inline void run() {
            mCommunicator.run(*this);
        }

    private:
        Position mPos;
        Communicator mCommunicator{};
    };
}