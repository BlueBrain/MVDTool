/*
 * Copyright (C) 2019, Blue Brain Project, EPFL.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#pragma once

#include <string>
#include <unordered_set>
#include <vector>

namespace MVD {
namespace utils {

// An implementation to drop duplicates without changing order
template <typename T>
inline void vector_remove_dups(std::vector<T>& vec) {
    std::unordered_set<T> set;
    std::size_t pos = 0;
    for (T& v : vec) if(set.insert(v).second) {
        std::swap(vec[pos++], v);  // works even if src-dst are same
    }
    vec.resize(pos);
}

}  // namespace utils
}  // namespace MVD
