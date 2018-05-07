#include <util/simple_shared_ptr.hpp>

#include <cstdio>

struct X {
    int x, y;
    
    X(int i, int j) : x(i), y(j) {}
    
    auto sum() { return x + y; }
};

using X_ptr = ucle::util::simple_shared_ptr<X>;

X_ptr global { nullptr };

void f(X_ptr ptr) {
    printf("f :: %d\n", ptr.count());
    global = ptr;
    printf("f :: %d\n", ptr.count());
}



int main() {
    using namespace ucle::util;

    auto ptr = make_simple_shared<X>(1, 2);
    auto ptr2 = ptr;
    printf("%d %d\n", ptr->sum(), ptr2->sum());
    printf("%d %d\n", ptr == ptr2, ptr != ptr2);
    printf("%d %d\n", ptr.count(), ptr2.count());
    
    f(ptr);
    
    printf("%d %d\n", ptr.count(), ptr2.count());

    f(ptr2);

    printf("%d %d\n", ptr.count(), ptr2.count());

    global.reset();

    printf("%d %d\n", ptr.count(), ptr2.count());
}
