#include <util/string.hpp>

#include <iostream>
#include <string>

int main() {
    using namespace ucle::util;

    std::string s;
    std::getline(std::cin, s);
    long long i;
    auto v = parse_int(s, &i);
    std::cout << v << " :: " << i << "\n";

}
