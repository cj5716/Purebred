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


#include <iostream>
#include "types.h"

void uci_loop(int argc, const char *argv[]) {
    std::cout << Name << " by " << Author << "\n";
    while (true) {
        std::string line;

        // Read a line of input
        // If no input -> quit
        if (!std::getline(std::cin, line)) break;

        // If the line is empty we move on to the next one
        if (line.empty()) continue;
    }
}