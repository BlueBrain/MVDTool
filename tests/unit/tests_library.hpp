#pragma once

#include <cstdlib>

namespace lib {

struct Counter {
    size_t n;

    Counter() : n(size()) {};

    static size_t size();
};

}
