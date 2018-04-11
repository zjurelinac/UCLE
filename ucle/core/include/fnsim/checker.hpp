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

    namespace detail {

        struct check_descr {
            const char op;
            std::string cls;
            std::string lhs;
            std::string rhs;
        };

        check_descr parse_check(const std::string& check)
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

    }

    template <typename FunctionalSimulation>
    void run_checks(std::string pfile, std::vector<std::string>& checks, FunctionalSimulation& sim, bool verbose = false)
    {
        fmt::print("Running checker for [{}]\n", pfile);
        fmt::print("================================================================================\n");

        std::unordered_map<char, std::function<bool(long long, long long)>> cmps = {
            { '=', [](auto x, auto y) { return x == y; } },
            { '<', [](auto x, auto y) { return x < y; } },
            { '>', [](auto x, auto y) { return x > y; } }
        };

        auto regs = sim.get_reg_info();
        // auto exec_info = sim.get_exec_info();

        int success_cnt = 0, fail_cnt = 0;

        try {
            for (const auto& check : checks) {
                auto cd = detail::parse_check(check);

                if (cd.op != '=' && cd.op != '<' && cd.op != '>')
                    throw malformed_check(fmt::format("Unknown comparison operand: {}!\n", cd.op));

                if (cd.cls == "rs") {  // registers
                    auto expected = std::stoll(cd.rhs, nullptr, 0);
                    auto actual = to_int(regs[cd.lhs]);

                    if (cmps[cd.op](actual, expected)) {
                        if (verbose)
                            fmt::print_colored(fmt::GREEN, "Passed ({} {} {}) [{} = {}].\n", cd.lhs, cd.op, expected, cd.lhs, actual);
                        else
                            fmt::print_colored(fmt::GREEN, "Passed.\n");
                        ++success_cnt;
                    } else {
                        if (verbose)
                            fmt::print_colored(fmt::RED, "Failed ({} !{} {}) [{} = {}].\n", cd.lhs, cd.op, expected, cd.lhs, actual);
                        else
                            fmt::print_colored(fmt::RED, "Failed!\n");
                        ++fail_cnt;
                    }
                }
            }

            fmt::print("--------------------------------------------------------------------------------\n");
            fmt::print_colored(fmt::MAGENTA, "{} tests passed, {} failed. Success rate: {:.2f}%\n", success_cnt, fail_cnt, 100.0 * success_cnt / checks.size());

            if (fail_cnt > 0)
                std::exit(1);

        } catch (std::exception& e) {
            fmt::print_colored(fmt::MAGENTA, "An error occurred: {}\n", e.what());
            std::exit(1);
        }

    }

}

#endif  /* _UCLE_CORE_FNSIM_CHECKER_HPP_ */
