#ifndef _UCLE_CORE_FNSIM_CHECKER_HPP_
#define _UCLE_CORE_FNSIM_CHECKER_HPP_

#include <fnsim/base.hpp>

#include <fnsim/simulation.hpp>

#include <libs/fmt/format.h>

#include <util/string.hpp>

#include <cstdlib>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace ucle::fnsim {

    template <typename FunctionalSimulation>
    class checker {

        struct check_descr {
            const char op;
            std::string cls;
            std::string lhs;
            std::string rhs;
        };

        public:
            checker() = delete;
            checker(FunctionalSimulation& sim, std::string filename, int verbosity)
                : sim_ {sim}, filename_ {filename}, verbosity_ {verbosity} {}

            void run(std::vector<std::string>& checks);

        private:
            void output_header_()
            {
                switch (verbosity_) {
                    case 2:
                        fmt::print_colored(fmt::CYAN, "Checking [{}]\n", filename_);
                        fmt::print("========================================\n");
                        return;
                    case 1:
                        fmt::print_colored(fmt::CYAN, "Checking [{}]: ", filename_);
                        return;
                    case 0:
                    default:
                        return;
                }

            }

            void output_footer_(int succeeded, int failed)
            {
                auto total = succeeded + failed;
                auto percent = 100.0 * succeeded / total;

                switch (verbosity_) {
                    case 2:
                        fmt::print("--------------------------------------------------------------------------------\n");
                        fmt::print_colored(fmt::MAGENTA, "{} tests passed, {} failed. Success rate: {:.2f}%\n", succeeded, failed, percent);
                        return;
                    case 1:
                        fmt::print_colored(fmt::MAGENTA, "\nResults: {}/{} passed, {:.2f}%\n", succeeded, total, percent);
                        return;
                    case 0:
                        fmt::print_colored(status_colors_[!failed], "[{}] :: {}/{} passed, {:.2f}%\n", filename_, succeeded, total, percent);
                        return;
                    default:
                        return;
                }
                /*
                if (verbosity_ == 0) {
                    fmt::print_colored(failed > 0 ? fmt::RED : fmt::GREEN, "[{}] :: {}/{} passed, {:.2f}%\n", filename_, succeeded, total, percent);
                } else {
                    fmt::print("--------------------------------------------------------------------------------\n");
                    fmt::print_colored(fmt::MAGENTA, "{} tests passed, {} failed. Success rate: {:.2f}%\n", succeeded, failed, percent);
                }
                */
            }

            check_descr parse_check_(const std::string& check)
            {
                auto op_pos = check.find_first_of("<=>");
                auto dot_pos = check.find('.');

                if (dot_pos == check.npos || op_pos == check.npos || dot_pos >= op_pos)
                    throw malformed_check(fmt::format("Passed check {} is malformed!", check));

                return {
                    check[op_pos],
                    util::trim_copy(check.substr(0, dot_pos)),
                    util::trim_copy(check.substr(dot_pos + 1, op_pos - dot_pos - 1)),
                    util::trim_copy(check.substr(op_pos + 1))
                };
            }

            bool check_register_(const check_descr& cd, register_info& rinfo)
            {
                auto rv = rinfo[cd.lhs];

                dword_t expected;
                if (!util::parse_int<dword_t>(cd.rhs, &expected))
                    throw malformed_check(fmt::format("Cannot parse check rhs: {} not a valid number!\n", cd.rhs));

                bool is_correct = [this, &expected, &cd, &rv]{
                    return std::visit(meta::overloaded {
                        [this, &expected, &cd](bool val) {
                            return cmp_values_(cd.op, val, static_cast<bool>(expected));
                        },
                        [this, &expected, &cd](byte_t val) {
                            return cmp_values_(cd.op, val, static_cast<byte_t>(expected));
                        },
                        [this, &expected, &cd](half_t val) {
                            return cmp_values_(cd.op, val, static_cast<half_t>(expected));
                        },
                        [this, &expected, &cd](word_t val) {
                            return cmp_values_(cd.op, val, static_cast<word_t>(expected));
                        },
                        [this, &expected, &cd](dword_t val) {
                            return cmp_values_(cd.op, val, static_cast<dword_t>(expected));
                        }
                    }, rv.value);
                }();

                switch (verbosity_) {
                    case 2:
                        fmt::print_colored(status_colors_[is_correct], "[{}] ", (is_correct ? "Passed" : "Failed"));
                        fmt::print("{} {}{} {}", cd.lhs, (is_correct ? "" : "!"), cd.op, cd.rhs);

                        if (!is_correct)
                            fmt::print(" ({} = {})\n", cd.lhs, to_string(rv));

                        fmt::print("\n");
                        break;
                    case 1:
                        fmt::print_colored(status_colors_[is_correct], is_correct ? "." : "!");
                        break;
                    default:
                        break;
                }

                return is_correct;
            }

            template <typename T, typename U>
            bool cmp_values_(char op, T x, U y)
            {
                switch (op) {
                    case '=': return x == y;
                    case '<': return x < y;
                    case '>': return x > y;
                    case '/': return x != y;
                    default:  throw malformed_check(fmt::format("Unsupported comparison operand: {}!\n", op));
                }
            }

            FunctionalSimulation& sim_;
            std::string filename_;
            int verbosity_ = 1;

            fmt::Color status_colors_[2] = {fmt::RED, fmt::GREEN};
    };

    template <typename FS>
    void checker<FS>::run(std::vector<std::string>& checks)
    {
        try {
            output_header_();

            auto rinfo = sim_.get_reg_info();
            int succeeded = 0, failed = 0;

            for (const auto& check : checks) {
                auto cd = parse_check_(check);

                bool result = [this, &cd, &rinfo]{
                    if (cd.cls == "r") return check_register_(cd, rinfo);
                    else               throw malformed_check(fmt::format("Unknown check class: {}!\n", cd.cls));
                }();

                succeeded += result;
                failed += !result;
            }

            output_footer_(succeeded, failed);

            if (failed > 0)
                std::exit(1);

        } catch (std::exception& e) {
            fmt::print_colored(fmt::MAGENTA, "An error occurred: {}\n", e.what());
            std::exit(1);
        }
    }

}

#endif  /* _UCLE_CORE_FNSIM_CHECKER_HPP_ */
