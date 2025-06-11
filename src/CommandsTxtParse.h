//
// Created by br on 11/06/25.
//

#ifndef COMMANDSTXTPARSE_H
#define COMMANDSTXTPARSE_H


#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>


struct CommandsTxtLine {
    std::string name;
    std::string comment;
};



class CommandsTxtParse {
    public:



    // Helper function to trim leading/trailing whitespace
    static inline void trim(std::string& s) {
        auto wsfront = std::find_if_not(s.begin(), s.end(), ::isspace);
        auto wsback = std::find_if_not(s.rbegin(), s.rend(), ::isspace).base();
        if (wsback <= wsfront)
            s.clear();
        else
            s = std::string(wsfront, wsback);
    }

    std::vector<CommandsTxtLine> load_and_filter_commands(const std::string& filename) {
        std::vector<CommandsTxtLine> results;
        std::ifstream infile(filename);
        std::string line;

        while (std::getline(infile, line)) {
            std::string name, comment;
            size_t pos = 0;

            // Skip leading whitespace
            while (pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos]))) ++pos;

            // Find first whitespace after command name
            size_t name_end = pos;
            while (name_end < line.size() && !std::isspace(static_cast<unsigned char>(line[name_end]))) ++name_end;

            name = line.substr(pos, name_end - pos);

            // Skip whitespace between name and comment
            size_t comment_start = name_end;
            while (comment_start < line.size() && std::isspace(static_cast<unsigned char>(line[comment_start]))) ++comment_start;

            comment = (comment_start < line.size()) ? line.substr(comment_start) : "";

            trim(name);
            trim(comment);

            if (!name.empty()) {
                results.push_back({name, comment});
            }
        }
        return results;
    }




    CommandsTxtParse( const char* filename ) {

        auto lines = load_and_filter_commands(filename);
        std::cout << "CommandsTxtParse: lines: " << lines.size() << "\n";

    };

};



#endif //COMMANDSTXTPARSE_H
