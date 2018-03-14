#ifndef CLARGS_H_
#define CLARGS_H_

#include <string>
#include <vector>
#include "any.h"

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
    std::vector<linb::any> _args;
};

}

#endif

