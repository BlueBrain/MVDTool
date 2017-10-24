/*
 * Copyright (C) 2015 Adrien Devresse <adrien.devresse@epfl.ch>
 *               2017 Fernando Pereira <fernando.pereira@epfl.ch>
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
#ifndef MVD_BASE_HPP
#define MVD_BASE_HPP

#include <string>
#include <algorithm>
#include <boost/multi_array.hpp>

#include "mvd_except.hpp"

namespace MVD {


typedef boost::multi_array<double, 2> Positions;
typedef boost::multi_array<double, 2> Rotations;


namespace MVDType{
enum MVDType{
    UnknownFileType =0,
    MVD2=20,
    MVD3=30
};
}


struct Range{
    inline Range(const size_t offset_=0, const size_t count_=0) : offset(offset_), count(count_) {}

    size_t offset;
    size_t count;
};


class MVDFile {
public:
    inline MVDFile() {}
    inline virtual ~MVDFile() {}
    virtual size_t getNbNeuron() const = 0;
    virtual Positions getPositions(const Range & range = Range(0,0)) const = 0;
    virtual Rotations getRotations(const Range & range = Range(0,0)) const = 0;
};


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



} // ::MVD

#endif
