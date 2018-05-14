#include <util/string_view.hpp>

#include <cassert>
#include <cstdio>

int main() {
    using namespace ucle;

    assert(util::take_while("   asdf ghj  ", isspace) == "   ");
    assert(util::drop_while("   asdf ghj  ", isspace) == "asdf ghj  ");

    assert(util::take_while_r("   asdf ghj  ", isspace) == "  ");
    assert(util::drop_while_r("   asdf ghj  ", isspace) == "   asdf ghj");
    assert(util::trim("   asdf ghj  ") == "asdf ghj");

    assert(util::take_while("   asdf ghj  ", [](std::string_view v) { return !util::starts_with(v, "df"); }) == "   as");
    assert(util::drop_while("   asdf ghj  ", [](std::string_view v) { return !util::starts_with(v, "sdf"); }) == "sdf ghj  ");

    assert(util::take_while_r("   asdf ghj  ", [](std::string_view v) { return !util::ends_with(v, "df"); }) == " ghj  ");
    assert(util::drop_while_r("   asdf ghj  ", [](std::string_view v) { return !util::ends_with(v, "sdf"); }) == "   asdf");

    assert(util::starts_with("input", "in") == true);
    assert(util::starts_with("input", "inr") == false);
    assert(util::starts_with("input", 'i') == true);
    assert(util::ends_with("input", "put") == true);
    assert(util::ends_with("input", "rut") == false);
    assert(util::ends_with("input", 't') == true);

    auto parts = util::split("hello, it is I,,and not you.", [](auto c){ return c == ','; });
    assert(parts.size() == 3);
    assert(parts[0] == "hello");
    assert(parts[1] == " it is I");
    assert(parts[2] == "and not you.");

    puts("All tests passed.");
}
