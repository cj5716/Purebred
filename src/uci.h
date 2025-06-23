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
#include "position.h"
#include "types.h"

#include <iostream>
#include <string>

namespace purebred {
    class UCICommunicator {
    public:
        static void report_init() {
            std::cout << kName << " by " << kAuthor << std::endl;
        }

        static void report_info_string() {
        }

        static void report_best_move() {
        }

        static void parse_command(auto &engine) {
            std::string line;
            std::getline(std::cin, line);
            std::istringstream iss(line);

            std::string command; iss >> command;
            if (command == "position") {
                parse_position(iss, engine);
            }
            else if (command == "d") {
                std::cout << engine.get_root_pos().to_pretty_str() << '\n';
            }
            else if (command == "quit") {
                engine.quit();
            }
        }

    private:
        static void parse_position(std::istringstream &iss, auto &engine) {
            std::string token; iss >> token;
            std::string fen;
            if (token == "startpos") {
                fen = kStartPosFen;
                iss >> token;
            }
            else if (token == "fen") {
                while (iss >> token && token != "moves") fen += token + " ";
            }
            else return;

            engine.set_fen(fen);
        }

    };
}