#ifndef _CORE_UTIL_CLARGS_H_
#define _CORE_UTIL_CLARGS_H_

#include <any>
#include <string>
#include <vector>

namespace cla {

template <typename T>
class arg {
    T value;

    arg() {}

private:
    std::string _name;
    std::string _description;
};

class cli {


private:
    std::vector<std::any> _args;
};

}

#endif  // _CORE_UTIL_CLARGS_H_

