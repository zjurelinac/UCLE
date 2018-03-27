#include <common/structures.hpp>

#include <cassert>
#include <iostream>

using namespace ucle;

void test1()
{
    bitfield<32> bf(7);

    assert(bf == 7);

    assert(bf[0] == 1);
    assert(bf[1] == 1);
    assert(bf[2] == 1);
    assert(bf[3] == 0);

    bf[0] = 0;
    assert(bf == 6);

    bf[3] = 1;
    assert(bf == 14);

    bf[30] = 1;
    assert(bf[30]);

    bf[30] = 0;
    assert(!bf[30]);
    assert(~bf[30]);

    auto bf2 = bf;
    assert(bf2 == 14);

    bitfield<32> bf3(bf2);
    assert(bf3 == 14);

    bitfield<32> bf4(std::move(bf3));
    assert(bf4 == 14);
}

int main()
{
    test1();
}