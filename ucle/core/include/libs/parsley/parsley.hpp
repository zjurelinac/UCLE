#ifndef _UCLE_CORE_LIBS_PARSLEY_PARSLEY_HPP_
#define _UCLE_CORE_LIBS_PARSLEY_PARSLEY_HPP_

#include <libs/fmt/format.h>

#include <util/string_view.hpp>

#include <any>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// TODO: Allow comments later

namespace ucle::parsley {
    using size_t = std::string_view::size_type;

    enum class parse_status : bool { success = true, fail = false };

    parse_status operator!(parse_status status) { return status == parse_status::success ? parse_status::fail : parse_status::success; }

    auto to_string(parse_status status) { return status == parse_status::success ? "success" : "fail"; }

    struct parse_result {
        parse_status status;
        size_t length;

        std::string_view contents;
        std::string_view symbol_name;
        std::any data;
    };

    namespace parsers {
        class base_parser {
            public:
                virtual ~base_parser() = default;
                virtual parse_result parse(std::string_view) = 0;
        };

        using base_ptr = std::shared_ptr<base_parser>;
        using base_ptr_list = std::initializer_list<base_ptr>;

        class sequence_parser : public base_parser {
            public:
                sequence_parser() {}
                sequence_parser(base_ptr_list items)
                {
                    for (auto& item : items)
                        items_.push_back(item);
                }

                parse_result parse(std::string_view input) override
                {
                    size_t pos = 0;
                    for (const auto& item : items_) {
                        if (auto res = item->parse(input.substr(pos)); res.status == parse_status::success)
                            pos += res.length;
                        else
                            return { parse_status::fail, 0 };
                    }

                    return { parse_status::success, pos, input.substr(0, pos) };
                }
                void add(base_ptr item) { items_.push_back(std::move(item)); }

            private:
                std::vector<base_ptr> items_;
        };

        class choice_parser : public base_parser {
            public:
                choice_parser() {}
                choice_parser(base_ptr_list items)
                {
                    for (auto& item : items)
                        items_.push_back(item);
                }

                parse_result parse(std::string_view input) override
                {
                    for (const auto& item : items_) {
                        if (auto res = item->parse(input); res.status == parse_status::success)
                            return res;
                    }

                    return { parse_status::fail, 0 };
                }

                void add(base_ptr item) { items_.push_back(std::move(item)); }

            private:
                std::vector<base_ptr> items_;
        };

        class optional_parser : public base_parser {
            public:
                optional_parser(base_ptr optional) : optional_ { std::move(optional) } {}

                parse_result parse(std::string_view input) override
                {
                    auto res = optional_->parse(input);
                    if (res.status == parse_status::success)
                        return res;
                    else
                        return { parse_status::success, 0 };
                }
            private:
                base_ptr optional_;
        };

        class kleene_star_parser : public base_parser {
            public:
                kleene_star_parser(base_ptr single) : single_ { std::move(single) } {}

                parse_result parse(std::string_view input) override
                {
                    size_t pos = 0;
                    while (true) {
                        if (auto res = single_->parse(input.substr(pos)); res.status == parse_status::success)
                            pos += res.length;
                        else
                            break;
                    }

                    return { parse_status::success, pos, input.substr(0, pos) };

                }
            private:
                base_ptr single_;
        };

        class kleene_plus_parser : public base_parser {
            public:
                kleene_plus_parser(base_ptr single) : single_ { std::move(single) } {}

                parse_result parse(std::string_view input) override
                {
                    bool nonce = true;
                    size_t pos = 0;
                    while (true) {
                        if (auto res = single_->parse(input.substr(pos)); res.status == parse_status::success) {
                            nonce = false;
                            pos += res.length;
                        } else {
                            break;
                        }
                    }

                    if (nonce)
                        return { parse_status::fail, 0 };
                    else
                        return { parse_status::success, pos, input.substr(0, pos) };

                }
            private:
                base_ptr single_;
        };

        class and_predicate_parser : public base_parser {
            public:
                and_predicate_parser(base_ptr base) : base_ { std::move(base) } {}

                parse_result parse(std::string_view input) override
                {
                    auto res = base_->parse(input);
                    return { res.status, 0 };
                }
            private:
                base_ptr base_;
        };

        class not_predicate_parser : public base_parser {
            public:
                not_predicate_parser(base_ptr base) : base_ { std::move(base) } {}

                parse_result parse(std::string_view input) override
                {
                    auto res = base_->parse(input);
                    return { !res.status, 0 };
                }
            private:
                base_ptr base_;
        };

        using sequence_ptr = std::shared_ptr<sequence_parser>;
        using choice_ptr   = std::shared_ptr<choice_parser>;
        using kstar_ptr    = std::shared_ptr<kleene_star_parser>;
        using kplus_ptr    = std::shared_ptr<kleene_plus_parser>;
        using and_pred_ptr = std::shared_ptr<and_predicate_parser>;
        using not_pred_ptr = std::shared_ptr<not_predicate_parser>;

        class literal_parser : public base_parser {
            public:
                literal_parser(const char* lit) : lit_ { lit }, lit_len_ { strlen(lit) } {}

                parse_result parse(std::string_view input) override
                {
                    if (util::starts_with(input, lit_))
                        return { parse_status::success, lit_len_, input.substr(0, lit_len_) };
                    else
                        return { parse_status::fail, 0 };
                }

            private:
                const char* lit_;
                std::size_t lit_len_;
        };

        class any_parser : public base_parser {
            parse_result parse(std::string_view input) override
            {
                if (input.length() > 0)
                    return { parse_status::success, 1, input.substr(0, 1) };
                else
                    return { parse_status::fail, 0 };
            }
        };

        struct char_range {
            char start, end;
            bool contains(char c) { return start <= c && c <= end; }
        };

        using char_range_list = std::initializer_list<char_range>;

        class class_parser : public base_parser {
            public:
                class_parser(char_range range) : ranges_ { range } {};

                class_parser(char_range_list ranges)
                {
                    for (const auto& range : ranges)
                        ranges_.push_back(range);
                }

                class_parser(std::string_view chars)
                {
                    for (auto ch : chars)
                        ranges_.push_back({ ch, ch });
                }

                parse_result parse(std::string_view input) override
                {
                    auto pred = [this](char c) {
                        for (auto& r : ranges_)
                            if (r.contains(c))
                                return true;

                        return false;
                    };

                    auto plen = util::take_while(input, pred).length();
                    return { plen > 0 ? parse_status::success : parse_status::fail, plen, input.substr(0, plen) };
                }

            private:
                std::vector<char_range> ranges_;
        };

        class symbol : public base_parser {
            public:
                symbol() {}
                symbol(std::string name) : name_ { name } {}
                symbol(std::string name, base_ptr parser) : name_ { name }, parser_ { std::move(parser) } {}

                symbol(const symbol&) = default;

                parse_result parse(std::string_view input) override
                {
                    auto res = parser_->parse(input);
                    res.symbol_name = name_;
                    return res;
                }

                void set_parser(base_ptr parser) { parser_ = std::move(parser); }
                auto ptr() { return std::shared_ptr<symbol>(this); }

            private:
                std::string name_;
                base_ptr parser_;
        };

        using symbol_ptr = std::shared_ptr<symbol>;

        auto lit(const char* literal)    { return std::make_shared<parsers::literal_parser>(literal); }
        auto seq(base_ptr_list items)    { return std::make_shared<parsers::sequence_parser>(items); }
        auto cho(base_ptr_list items)    { return std::make_shared<parsers::choice_parser>(items); }
        auto opt(base_ptr optional)      { return std::make_shared<parsers::optional_parser>(optional); }
        auto kst(base_ptr single)        { return std::make_shared<parsers::kleene_star_parser>(single); }
        auto kpl(base_ptr single)        { return std::make_shared<parsers::kleene_plus_parser>(single); }
        auto anp(base_ptr base)          { return std::make_shared<parsers::and_predicate_parser>(base); }
        auto ntp(base_ptr base)          { return std::make_shared<parsers::not_predicate_parser>(base); }
        auto any()                       { return std::make_shared<parsers::any_parser>(); }
        auto cls(char_range range)       { return std::make_shared<parsers::class_parser>(range); }
        auto cls(char_range_list ranges) { return std::make_shared<parsers::class_parser>(ranges); }
        auto cls(std::string_view chars) { return std::make_shared<parsers::class_parser>(chars); }
        auto sym(symbol s)               { return std::make_shared<parsers::symbol>(s); }

        sequence_ptr operator>>(sequence_ptr s, base_ptr b) { s->add(std::move(b)); return s; }
        sequence_ptr operator>>(base_ptr b1, base_ptr b2) { return seq({ std::move(b1), std::move(b2) }); }

        choice_ptr operator/(choice_ptr c, base_ptr b) { c->add(std::move(b)); return c; }
        choice_ptr operator/(base_ptr b1, base_ptr b2) { return cho({ std::move(b1), std::move(b2) }); }

        class multiply_helper {};

        kstar_ptr operator*(base_ptr b, const multiply_helper) { return kst(std::move(b)); }
        kplus_ptr operator+(base_ptr b, const multiply_helper) { return kpl(std::move(b)); }

        and_pred_ptr operator&(base_ptr b) { return anp(std::move(b)); }
        not_pred_ptr operator~(base_ptr b) { return ntp(std::move(b)); }

        void operator<=(symbol& s, base_ptr b) { s.set_parser(b); }

    }

    using parsers::lit;
    using parsers::seq;
    using parsers::cho;
    using parsers::opt;
    using parsers::kst;
    using parsers::kpl;
    using parsers::anp;
    using parsers::ntp;
    using parsers::any;
    using parsers::cls;
    using parsers::sym;

    using parsers::symbol;

    static const parsers::multiply_helper N;

    // namespace detail {

    //     class quoted_parser : public base_parser {
    //         public:
    //             int consume(std::string_view input) override
    //             {
    //                 if (!is_quote_(input[0]))
    //                     return 0;

    //                 auto len = input.length();
    //                 bool escaped = false;

    //                 for (size_t i = 1; i < len; ++i) {
    //                     if (escaped)
    //                         escaped = false;
    //                     else if (input[i] == '\\')
    //                         escaped = true;
    //                     else if (is_quote_(input[i]))
    //                         return i + 1;
    //                 }

    //                 return 0;
    //             }

    //         private:
    //             bool is_quote_(const char c) const { return c == '\'' || c == '\"'; }
    //     };

    // };

};

#endif  /* _UCLE_CORE_LIBS_PARSLEY_PARSLEY_HPP_ */
