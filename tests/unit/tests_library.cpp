#include <mvdtool/mvd_generic.hpp>

#include "tests_library.hpp"

size_t lib::Counter::size() {
    return MVD::open(MVD3_FILENAME)->size();
}
