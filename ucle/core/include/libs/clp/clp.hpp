#ifndef _CORE_LIBS_CLAP_HPP_
#define _CORE_LIBS_CLAP_HPP_

#include <any>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace clp {

    class cli {
        public:
            cli() {}

            void parse(int argc, const char* argv[]);

        private:
            const char flag_sym = '-';
            std::vector<std::any> args_;

            bool is_long_flag_(std::string_view arg) { return arg.size() > 2 && arg[0] == flag_sym && arg[1] == flag_sym; }
            bool is_flag_(std::string_view arg) { return arg.size() > 1 && arg[0] == flag_sym; }
        };
}

#endif  // _CORE_LIBS_CLAP_HPP_

