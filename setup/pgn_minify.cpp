//
// Created by aepp2 on 1/26/2020.
//

#include <cstdlib>
#include <iostream>
#include <string>


std::string pgn_line_minify(const std::string& line) {
    std::string minified;
    minified.reserve(line.size());

    int paren_depth = 0;
    for (const auto c : line) {
        if (c == '{' || c == '(') ++paren_depth;
        else if (c == '}' || c == ')') --paren_depth;
        else if (paren_depth != 0) continue;
        else if (c == '?' || c == '!' || c == '+' || c == '#') continue;
        else if (c == ';') break;
        else {
            if (minified.size() > 3 && minified.substr(minified.size() - 3) == "...") {
                // Here we have " [number] ..."
                minified.erase(minified.size() - 3);
                minified.erase(minified.find_last_not_of(" \t") + 1);
                minified.erase(minified.find_last_of(" \t") + 1);
            }

            minified += c;
        }
    }
    return minified;
}

void pgn_minify(std::istream& in, std::ostream& out) {
    std::string line;

    while (std::getline(in, line)) {
        if (!line.empty() && line[0] != '[') {
            out << pgn_line_minify(line) << '\n';
        }
    }
}


int main(int argc, const char** argv) {
    static char cout_buffer[1024*1024*4];
    std::cout.rdbuf()->pubsetbuf(cout_buffer, sizeof(cout_buffer));
    std::cout.tie(nullptr);

    static char cin_buffer[1024*1024*4];
    std::cin.rdbuf()->pubsetbuf(cin_buffer, sizeof(cin_buffer));
    std::cin.tie(nullptr);

    std::ios_base::sync_with_stdio(false);

    pgn_minify(std::cin, std::cout);
    std::cout.flush();

    return EXIT_SUCCESS;
}