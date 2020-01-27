//
// Created by aepp2 on 1/26/2020.
//

#include <string_view>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <experimental/filesystem>

#include "progress-cpp/progress_bar.hpp"

namespace fs = std::experimental::filesystem;

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
            if (minified.size() > 3 && minified.substr(minified.size() - 3) == "...")
                minified.erase(minified.size() - 3);

            minified += c;
        }
    }
    return minified;
}

void pgn_minify(const fs::path& in_path, const fs::path& out_path) {
    std::string line;

    const size_t bufsize = 1024*1024;
    char buf[bufsize];
    std::ifstream in_stream(in_path, std::ios::in);
    in_stream.rdbuf()->pubsetbuf(buf, bufsize);

    std::ofstream out_stream(out_path, std::ios::out);

    {
        ProgressBar<long> bar(fs::file_size(in_path), 50);
        while (std::getline(in_stream, line)) {
            if (line[0] != '[' && line[0] != '\n') {
                out_stream << pgn_line_minify(line);
            }

            bar.update(in_stream.tellg() - bar.get());
        }
    }
}


int main(int argc, const char** argv) {
    if (argc != 2) return EXIT_FAILURE;

    fs::path pgn_in, pgn_out;
    const auto base_path = fs::path(argv[1]);
    for (const auto& entry : fs::directory_iterator(base_path)) {
        if (entry.path().extension() == ".bz2") {
            std::cout << "Extracting " << entry.path() << std::endl;
            system((std::string("lbzip2 -d ") + entry.path().string()).c_str());
            pgn_in = entry.path(); pgn_in.replace_extension("");
        }
        else if (entry.path().extension() == ".pgn"
              && entry.path().stem().extension().string() != ".minified") {
            pgn_in = entry.path();
        }
        else {
            continue;
        }

        pgn_out = pgn_in; pgn_out.replace_extension(".minified.pgn");

        std::cout << pgn_in << " -> " << pgn_out << std::endl;
        pgn_minify(pgn_in, pgn_out);
        fs::remove(pgn_in);
    }

    return EXIT_SUCCESS;
}