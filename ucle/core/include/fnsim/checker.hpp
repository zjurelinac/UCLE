#ifndef _UCLE_CORE_FNSIM_CHECKER_HPP_
#define _UCLE_CORE_FNSIM_CHECKER_HPP_

#include <fnsim/simulation.hpp>

#include <libs/fmt/format.h>

#include <util/string.hpp>

#include <cstdlib>
#include <functional>
#include <string>
#include <unordered_map>
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
                if (verbosity_ == 0) return;

                fmt::print("Running checker for [{}]\n", filename_);
                fmt::print("================================================================================\n");
            }

            void output_footer_(int succeeded, int failed)
            {
                auto total = succeeded + failed;
                auto percent = 100.0 * succeeded / total;

                if (verbosity_ == 0) {
                    fmt::print_colored(failed > 0 ? fmt::RED : fmt::GREEN, "[{}] :: {}/{} passed, {:.2f}%\n", filename_, succeeded, total, percent);
                } else {
                    fmt::print("--------------------------------------------------------------------------------\n");
                    fmt::print_colored(fmt::MAGENTA, "{} tests passed, {} failed. Success rate: {:.2f}%\n", succeeded, failed, percent);
                }
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

            bool check_register_(const check_descr& cd, reg_info& rinfo)
            {
                auto expected = std::stoll(cd.rhs, nullptr, 0);
                auto actual = to_int(rinfo[cd.lhs]);

                if (cmps_[cd.op](actual, expected)) {
                    if (verbosity_ == 2)
                        fmt::print_colored(fmt::GREEN, "Passed ({} {} {}) [{} = {}].\n", cd.lhs, cd.op, expected, cd.lhs, actual);
                    else if (verbosity_ == 1)
                        fmt::print_colored(fmt::GREEN, "Passed.\n");
                    return true;
                } else {
                    if (verbosity_ == 2)
                        fmt::print_colored(fmt::RED, "Failed ({} !{} {}) [{} = {}].\n", cd.lhs, cd.op, expected, cd.lhs, actual);
                    else if (verbosity_ == 1)
                        fmt::print_colored(fmt::RED, "Failed!\n");
                    return false;
                }
            }

            std::unordered_map<char, std::function<bool(long long, long long)>> cmps_ = {
                { '=', [](auto x, auto y) { return x == y; } },
                { '<', [](auto x, auto y) { return x < y; } },
                { '>', [](auto x, auto y) { return x > y; } }
            };

            FunctionalSimulation& sim_;
            std::string filename_;
            int verbosity_ = 1;
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

                if (cd.op != '=' && cd.op != '<' && cd.op != '>')
                    throw malformed_check(fmt::format("Unknown comparison operand: {}!\n", cd.op));

                if (cd.cls == "r") {  // registers
                    if (check_register_(cd, rinfo))
                        ++succeeded;
                    else
                        ++failed;
                }
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
