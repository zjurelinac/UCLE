#include <initializer_list>
#include <memory>
#include <vector>

struct Base {
    virtual int run() const = 0;
    virtual ~Base() = default;
};

using base_ptr = std::shared_ptr<Base>;

struct A : public Base {
    A(int x) : x_ { x } {}
    int run() const override { return x_; }
    private: int x_;
};

struct S : public Base {
    S() {}
    S(std::initializer_list<base_ptr> bs) { for (auto& b : bs) v_.push_back(b); }
    int run() const override { int r = 0; for (const auto& e : v_) r += e->run(); return r; }
    void add(base_ptr b) { v_.push_back(std::move(b)); }
    private: std::vector<base_ptr> v_;
};

using s_ptr = std::shared_ptr<S>;

struct C : public Base {
    int run() const override { int r = 1; for (const auto& e : v_) r *= e->run(); return r; }
    void add(base_ptr b) { v_.push_back(std::move(b)); }
    private: std::vector<base_ptr> v_;
};

using c_ptr = std::shared_ptr<C>;

auto a(int x) { return std::make_shared<A>(x); }
auto s()      { return std::make_shared<S>(); }
auto s(std::initializer_list<base_ptr> bs) { return std::make_shared<S>(bs); }
auto c()      { return std::make_shared<C>(); }

s_ptr operator>>(s_ptr s1, base_ptr b2)
{
    puts("Chaining S & Base.");
    s1->add(std::move(b2));
    return s1;
}

s_ptr operator>>(base_ptr b1, base_ptr b2)
{
    puts("Chaining Base & Base.");
    return s({ std::move(b1), std::move(b2) });
}

c_ptr operator/(c_ptr c1, base_ptr b2)
{
    puts("Choicing C & Base.");
    c1->add(std::move(b2));
    return c1;
}

c_ptr operator/(base_ptr b1, base_ptr b2)
{
    puts("Choicing Base & Base.");
    auto ch = c();
    ch->add(std::move(b1));
    ch->add(std::move(b2));
    return ch;
}

int test() {
    auto t1 = a(1) >> a(2) >> a(3) >> a(4);
    auto t2 = t1 / a(5) / a(2);
    return t2->run();
}

int main() {
    printf("%d\n", test());
}
