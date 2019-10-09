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

#include <stdexcept>
#include <string>

#include "mvd_except.hpp"

///
/// \brief Generic TSV exception
///
class TSVException: public MVDException {
  public:
    TSVException(const std::string& str)
        : MVDException(str) {}
};

///
/// \brief Exception related to TSV file parsing
///
class TSVParserException: public TSVException {
  public:
    TSVParserException(const std::string& str)
        : TSVException(str) {}
};
