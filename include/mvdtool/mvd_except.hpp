/*
 * Copyright (C) 2015 Adrien Devresse <adrien.devresse@epfl.ch>
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
#ifndef MVD_EXCEPT_HPP
#define MVD_EXCEPT_HPP

#include <stdexcept>
#include <string>

///
/// \brief Generic MVD exception
///
class MVDException : public std::runtime_error{
public:
    MVDException(const std::string & str) : std::runtime_error(str) {}
};

///
/// \brief Exception related to MVD file parsing
///
class MVDParserException : public MVDException{
public:
    MVDParserException(const std::string & str) : MVDException(str) {}
};


#endif // MVD_EXCEPT_HPP

