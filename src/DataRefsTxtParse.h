/*
 * XPL_WorkBench - An X-Plane Plugin host.
 * Copyright (C) 2024-2026 Ben Russell - br@x-plugins.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

//
// Created by br on 11/06/25.
//

#ifndef DATAREFSTXTPARSE_H
#define DATAREFSTXTPARSE_H


#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>


struct DataRefsTxtLine {
    std::string name;
    std::string type;
    std::string writable;
    std::string units;
    std::string comment;
};


class DataRefsTxtParse {
public:
    std::vector<DataRefsTxtLine> m_recs;

    // Helper function to trim leading/trailing whitespace
    static inline void trim(std::string& s) {
        auto wsfront = std::find_if_not(s.begin(), s.end(), ::isspace);
        auto wsback = std::find_if_not(s.rbegin(), s.rend(), ::isspace).base();
        if (wsback <= wsfront)
            s.clear();
        else
            s = std::string(wsfront, wsback);
    }

    std::vector<DataRefsTxtLine> load_and_filter_drefs(const std::string& filename) {
        std::vector<DataRefsTxtLine> results;
        std::ifstream infile(filename);
        std::string line;

        // parser for:
        //  dref_name<ws>type<ws>writeable<ws>units<ws>comment

        bool skip_first = true;
        while (std::getline(infile, line)) {
            if (skip_first) {
                skip_first = false;
                continue;
            }

            if ( line.empty() ) {
                continue;
            }

            std::istringstream iss(line);
            std::string name, type, writable, units, comment;

            // Parse each field (stop after units as comment may have spaces)
            // if( ! (iss >> name >> type >> writable >> units) ){
            if (!(iss >> name >> type >> writable)) {
                // Skip malformed or incomplete lines
                std::cerr << "dref parse Error: " << line << std::endl;
                continue;
            }

            units = "dunno?";

            // Rest of the line is the comment (may contain whitespace)
            std::getline(iss, comment);
            trim(name);
            trim(type);
            trim(writable);
            trim(units);
            trim(comment);

            if (!name.empty()) {
                results.push_back({
                    .name = name,
                    .type = type,
                    .writable = writable,
                    .units = units,
                    .comment = comment
                });
            }
        }
        return results;
    }


    DataRefsTxtParse(const char* filename) {
        m_recs = load_and_filter_drefs(filename);
        std::cout << "xwb/ DataRefsTxtParse: lines: " << m_recs.size() << "\n";
    }
};


#endif //DATAREFSTXTPARSE_H
