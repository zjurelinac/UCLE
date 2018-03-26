#include <libs/clap.hpp>

#include <iostream>
#include <string_view>

void clap::cli::parse(int argc, const char* argv[])
{
    // using namespace detail;

    auto i = 1u;
    while (i < argc) {
        std::string_view arg(argv[i]);

        if (is_long_flag_(arg)) {
            std::cout << arg << " :: Long Flag\n";
        } else if (is_flag_(arg)) {
            std::cout << arg << " :: Basic Flag\n";
        } else {
            std::cout << arg << " :: Not a Flag\n";
        }

        ++i;
    }
}

// clap -std=c++17 -Icore/include -Wall -c file1.cpp file2.cpp -o binary

/* Test */
int main(int argc, const char* argv[])
{
    clap::cli argparser;
    // argparser.
    argparser.parse(argc, argv);
}