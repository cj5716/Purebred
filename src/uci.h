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
#include <iostream>
#include <vector>
#include "types.h"

[[nodiscard]] inline std::vector<std::string> split_string(const std::string &str, const std::string &delim)
{
    size_t pos_start = 0, pos_end, delim_len = delim.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = str.find(delim, pos_start)) != std::string::npos)
    {
        token = str.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(str.substr(pos_start));
    return res;
}

void uci_loop(int argc, const char *argv[])
{
    std::cout << Name << " by " << Author << "\n";

    const auto parse_uci = [&](std::string &line)
    {
        const auto tokens = split_string(line, " ");

        if (line == "isready")
            std::cout << "readyok" << "\n";

        else if (line == "quit")
            return false;

        else if (line == "uci")
        {
            std::cout << "id name " << Name << "\n";
            std::cout << "id author " << Author << "\n";
        }

        return true;
    };

    for (int i = 1; i < argc; ++i)
    {
        std::string cmdstr(argv[i]);
        if (!parse_uci(cmdstr)) return;
    }

    while (true) {
        std::string line;

        // Read a line of input
        // If no input -> quit
        if (!std::getline(std::cin, line)) break;

        // If the line is empty we move on to the next one
        if (line.empty()) continue;

        // Parse the line and carry out the required actions.
        // If quit is called then we quit
        if (!parse_uci(line)) return;
    }
}