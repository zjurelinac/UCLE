#ifndef _UCLE_CORE_LIBS_PARSLEY_PARSLEY_HPP_
#define _UCLE_CORE_LIBS_PARSLEY_PARSLEY_HPP_

#include <common/types.hpp>

#include <libs/fmt/format.h>

#include <util/string_view.hpp>

#include <any>
#include <cstring>
#include <exception>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace ucle::parsley {
    using size_t = std::string_view::size_type;

    enum class parse_status : bool { success = true, fail = false };

    parse_status operator!(parse_status status) { return status == parse_status::success ? parse_status::fail : parse_status::success; }

    auto to_string(parse_status status) { return status == parse_status::success ? "success" : "fail"; }

    class key_error : public base_exception { using base_exception::base_exception; };

    struct parse_details {
        std::string_view contents;
        std::vector<parse_details> children;
        std::string_view symbol_name;

        bool operator==(const std::string_view& symbol_name) const { return this->symbol_name == symbol_name; }
        bool operator!=(const std::string_view& symbol_name) const { return this->symbol_name != symbol_name; }

        bool empty() const { return contents.empty(); }

        const auto& operator[](size_t index) const { return children[index]; }

        const auto& operator[](std::string_view symbol_name) const
        {
            for (const auto& pi : children)
                if (pi.symbol_name == symbol_name)
                    return pi;

            throw key_error(fmt::format("No child symbol with a given name exists ({}).", symbol_name));
        }

        bool has(std::string_view symbol_name) const
        {
            for (const auto& pi : children)
                if (pi.symbol_name == symbol_name)
                    return true;

            return false;
        }

        size_t count(std::string_view symbol_name) const
        {
            size_t cnt = 0;

            for (const auto& pi : children)
                if (pi.symbol_name == symbol_name)
                    ++cnt;

            return cnt;
        }

        auto all(std::string_view symbol_name) const
        {
            std::vector<std::reference_wrapper<const parse_details>> childs;

            for (const auto& pi : children)
                if (pi.symbol_name == symbol_name)
                    childs.emplace_back(pi);

            return childs;
        }
    };

    struct parse_info {
        parse_status status;
        parse_details details;
    };

    namespace parsers {

        void append_child(std::vector<parse_details>& children, const parse_details& child) {
            if (child.symbol_name.length() > 0)
                children.push_back(child);
            else if (child.children.size() > 0)
                children.insert(children.end(), child.children.begin(), child.children.end());
        }

        class base_parser {
            public:
                virtual ~base_parser() = default;
                virtual parse_info parse(std::string_view) = 0;
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

                parse_info parse(std::string_view input) override
                {
                    std::vector<parse_details> children;

                    size_t pos = 0;
                    for (const auto& item : items_) {
                        if (auto res = item->parse(input.substr(pos)); res.status == parse_status::success) {
                            pos += res.details.contents.length();
                            append_child(children, res.details);
                        } else {
                            return { parse_status::fail, {} };
                        }
                    }

                    return { parse_status::success, { input.substr(0, pos), children, "" } };
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

                parse_info parse(std::string_view input) override
                {
                    for (const auto& item : items_) {
                        if (auto res = item->parse(input); res.status == parse_status::success) {
                            std::vector<parse_details> children;
                            append_child(children, res.details);

                            return { parse_status::success, { res.details.contents, children, "" } };
                        }
                    }

                    return { parse_status::fail, {} };
                }

                void add(base_ptr item) { items_.push_back(std::move(item)); }

            private:
                std::vector<base_ptr> items_;
        };

        class optional_parser : public base_parser {
            public:
                optional_parser(base_ptr optional) : optional_ { std::move(optional) } {}

                parse_info parse(std::string_view input) override
                {
                    auto res = optional_->parse(input);
                    if (res.status == parse_status::success)
                        return res;
                    else
                        return { parse_status::success, {} };
                }

            private:
                base_ptr optional_;
        };

        class kleene_star_parser : public base_parser {
            public:
                kleene_star_parser(base_ptr single) : single_ { std::move(single) } {}

                parse_info parse(std::string_view input) override
                {
                    std::vector<parse_details> children;

                    size_t pos = 0;
                    while (true) {
                        if (auto res = single_->parse(input.substr(pos)); res.status == parse_status::success) {
                            pos += res.details.contents.length();
                            append_child(children, res.details);
                        } else {
                            break;
                        }
                    }

                    return { parse_status::success, { input.substr(0, pos), children, "" } };
                }

            private:
                base_ptr single_;
        };

        class kleene_plus_parser : public base_parser {
            public:
                kleene_plus_parser(base_ptr single) : single_ { std::move(single) } {}

                parse_info parse(std::string_view input) override
                {
                    std::vector<parse_details> children;

                    bool nonce = true;
                    size_t pos = 0;
                    while (true) {
                        if (auto res = single_->parse(input.substr(pos)); res.status == parse_status::success) {
                            nonce = false;
                            pos += res.details.contents.length();
                            append_child(children, res.details);
                        } else {
                            break;
                        }
                    }

                    if (nonce)
                        return { parse_status::fail, {} };
                    else
                        return { parse_status::success, { input.substr(0, pos), children, "" } };
                }

            private:
                base_ptr single_;
        };

        class and_predicate_parser : public base_parser {
            public:
                and_predicate_parser(base_ptr base) : base_ { std::move(base) } {}

                parse_info parse(std::string_view input) override
                {
                    auto res = base_->parse(input);
                    return { res.status, {} };
                }
            private:
                base_ptr base_;
        };

        class not_predicate_parser : public base_parser {
            public:
                not_predicate_parser(base_ptr base) : base_ { std::move(base) } {}

                parse_info parse(std::string_view input) override
                {
                    auto res = base_->parse(input);
                    return { !res.status, {} };
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

        class epsilon_parser : public base_parser {
            parse_info parse(std::string_view) override { return { parse_status::success, {} }; }
        };

        class literal_parser : public base_parser {
            public:
                literal_parser(const char* lit) : lit_ { lit }, lit_len_ { strlen(lit) } {}

                parse_info parse(std::string_view input) override
                {
                    if (util::starts_with(input, lit_))
                        return { parse_status::success, { input.substr(0, lit_len_), {}, "" } };
                    else
                        return { parse_status::fail, {} };
                }

            private:
                const char* lit_;
                std::size_t lit_len_;
        };

        class iliteral_parser : public base_parser {
            public:
                iliteral_parser(const char* lit) : lit_ { lit }, lit_len_ { strlen(lit) } {}

                parse_info parse(std::string_view input) override
                {
                    if (util::istarts_with(input, lit_))
                        return { parse_status::success, { input.substr(0, lit_len_), {}, "" } };
                    else
                        return { parse_status::fail, {} };
                }

            private:
                const char* lit_;
                std::size_t lit_len_;
        };

        class any_parser : public base_parser {
            parse_info parse(std::string_view input) override
            {
                if (input.length() > 0)
                    return { parse_status::success, { input.substr(0, 1), {}, "" } };
                else
                    return { parse_status::fail, {} };
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

                parse_info parse(std::string_view input) override
                {
                    auto pred = [this](char c) {
                        for (auto& r : ranges_)
                            if (r.contains(c))
                                return true;

                        return false;
                    };

                    if (input.length() > 0 && pred(input[0]))
                        return { parse_status::success, { input.substr(0, 1), {}, "" } };
                    else
                        return { parse_status::fail, {} };
                }

            private:
                std::vector<char_range> ranges_;
        };

        class symbol_parser : public base_parser {
            public:
                symbol_parser() {}
                symbol_parser(base_ptr parser, std::string_view name) : parser_ { std::move(parser) }, name_ { name } {}

                parse_info parse(std::string_view input) override
                {
                    auto res = parser_->parse(input);
                    res.details.symbol_name = name_;
                    return res;
                }

                void set_parser(base_ptr parser) { parser_ = std::move(parser); }

            private:
                base_ptr parser_;
                std::string_view name_;
        };

        using symbol_ptr = std::shared_ptr<symbol_parser>;

        auto eps()                       { return std::make_shared<parsers::epsilon_parser>(); }
        auto lit(const char* literal)    { return std::make_shared<parsers::literal_parser>(literal); }
        auto ilit(const char* literal)   { return std::make_shared<parsers::iliteral_parser>(literal); }
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
        auto sym(base_ptr base, std::string_view sym_name) { return std::make_shared<parsers::symbol_parser>(base, sym_name); }

        template <typename T>
        auto cpy(T parser_ptr) { return std::make_shared<typename T::element_type>(*parser_ptr); }

        sequence_ptr operator>>(sequence_ptr s, base_ptr b) { auto ns = cpy(s); ns->add(std::move(b)); return ns; }
        sequence_ptr operator>>(base_ptr b1, base_ptr b2) { return seq({ std::move(b1), std::move(b2) }); }

        choice_ptr operator/(choice_ptr c, base_ptr b) { auto nc = cpy(c); nc->add(std::move(b)); return nc; }
        choice_ptr operator/(base_ptr b1, base_ptr b2) { return cho({ std::move(b1), std::move(b2) }); }

        class multiply_helper {};

        kstar_ptr operator*(base_ptr b, const multiply_helper) { return kst(std::move(b)); }
        kplus_ptr operator+(base_ptr b, const multiply_helper) { return kpl(std::move(b)); }

        and_pred_ptr operator&(base_ptr b) { return anp(std::move(b)); }
        not_pred_ptr operator~(base_ptr b) { return ntp(std::move(b)); }

        symbol_ptr operator>=(base_ptr b, std::string_view sym_name) { return sym(b, sym_name); }
    }

    using parser_ptr = parsers::base_ptr;

    using parsers::eps;
    using parsers::lit;
    using parsers::ilit;
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

    static const parsers::multiply_helper N;

    // struct visit_result {
    //     std::string_view symbol_name;
    //     std::any data;
    // };

    // using visit_results = std::vector<visit_result>;

    // class visitor {
    //     public:
    //         using visitor_type = std::function<std::any(visit_results&)>;
    //         using map_type = std::unordered_map<std::string_view, visitor_type>;

    //         auto& operator[] (const std::string_view& symbol) { return visitors_[symbol]; };

    //         void visit(parse_info res)
    //         {
    //             if (res.status == parse_status::fail)
    //                 return;

    //             visit_(res.details);
    //         }

    //     private:
    //         visit_result visit_(const parse_details& pi)
    //         {
    //             visit_results results;

    //             for (const auto& child : pi.children)
    //                 results.push_back(visit_(child));

    //             if (!pi.symbol_name.empty() && visitors_.count(pi.symbol_name) > 0)
    //                 return { pi.symbol_name, visitors_[pi.symbol_name](results) };

    //             return { pi.symbol_name, results };
    //         }

    //         map_type visitors_;
    // };

}

#endif  /* _UCLE_CORE_LIBS_PARSLEY_PARSLEY_HPP_ */
