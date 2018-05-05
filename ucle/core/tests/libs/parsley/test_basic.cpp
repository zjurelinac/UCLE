#include <libs/parsley/parsley.hpp>

std::string simple_grammar = R"(
    Additive    <- Multitive '+' Additive / Multitive
    Multitive   <- Primary '*' Multitive / Primary
    Primary     <- '(' Additive ')' / Number
    Number      <- < [0-9]+ >
    %whitespace <- [ \t]*
)";

using namespace ucle;
using namespace ucle::parsley;

void try_parse(const parsers::base_ptr& p, const char* input)
{
    auto res = p->parse(input);
    fmt::print("{} => {} {} [{}]\n", input, to_string(res.status), res.contents, res.symbol_name.length() > 0 ? res.symbol_name : "none");
}

void try_parse(symbol& s, const char* input)
{
    auto res = s.parse(input);
    fmt::print("{} => {} {} [{}]\n", input, to_string(res.status), res.contents, res.symbol_name.length() > 0 ? res.symbol_name : "none");
}

int main() {

    // auto parser1 = lit("symbol") >> lit("<-") >> (lit("letter") / lit("digit")) >> opt(lit("_and_"));
    // try_parse(parser1, "symbol<-letter_and_stuff");

    // auto parser2 = lit("a")*N >> lit("b") >> lit("c")+N;
    // try_parse(parser2, "aaabccab");

    // auto parser3 = &lit("aaa") >> lit("a");
    // try_parse(parser3, "aaabcd");

    // auto parser4 = ~lit("aaa") >> lit("a");
    // try_parse(parser4, "aabcd");

    symbol dec_digit { "dec_digit" }, hex_digit { "hex_digit" }, hex_const { "hex_const" };

    dec_digit <= cls({'0', '9'});
    hex_digit <= cls({{'0', '9'}, {'a', 'f'}, {'A', 'F'}});
    hex_const <= sym(dec_digit) >> kst(sym(hex_digit));

    try_parse(hex_const, "012345");
    try_parse(hex_const, "0ABCD1345");
    try_parse(hex_const, "0abc123def");
    try_parse(hex_const, "012345GHJ");

    // auto spaces = cls(" \t\r\n");
    // try_parse(spaces, "    asdf");
}
