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
#ifndef MVD_GENERIC_HPP
#define MVD_GENERIC_HPP

#include <string>
#include <algorithm>
#include "mvd_except.hpp"

namespace MVD{

namespace MVDType{

enum MVDType{
    UnknownFileType =0,
    MVD2=20,
    MVD3=30
};

}

///
/// \brief is_mvd_file
/// \param filename mvd file name
/// \return the type of the MVD file or UnknownFileType if not recognized
///
inline MVDType::MVDType is_mvd_file(const std::string & filename){

    // mvd2
    const std::string mvd_ext = ".mvd2";
    if(std::search(filename.rbegin(), filename.rend(), mvd_ext.rbegin(), mvd_ext.rend()) != filename.rend()){
        return MVDType::MVD2;
    }
    // everything else mvd3 for now
    return MVDType::MVD3;
}


} // MVD

#endif // MVD_GENERIC_HPP
