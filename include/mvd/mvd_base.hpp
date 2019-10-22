/*
 * Copyright (C) 2015 Adrien Devresse <adrien.devresse@epfl.ch>
 *               2017 Fernando Pereira <fernando.pereira@epfl.ch>
 *               2019 Matthias Wolf <matthias.wolf@epfl.ch>
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
#include <vector>
#include <algorithm>
#include <boost/multi_array.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "mvd_except.hpp"

namespace MVD {


typedef boost::multi_array<double, 2> Positions;
typedef boost::multi_array<double, 2> Rotations;


namespace MVDType{
enum MVDType{
    UnknownFileType = 0,
    MVD2 = 20,
    MVD3 = 30,
    Sonata = 40
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
/// \brief Higher level interface for functional access
///
class File : public MVDFile {
public:
    inline File() = default;
    inline virtual ~File() = default;
    virtual size_t size() const { return getNbNeuron(); }

    virtual bool hasRotations() const = 0;

    virtual std::vector<std::string> getMorphologies(const Range& range = Range(0, 0)) const = 0;
    virtual std::vector<std::string> getEtypes(const Range& range = Range(0, 0)) const = 0;
    virtual std::vector<std::string> getMtypes(const Range& range = Range(0, 0)) const = 0;
    virtual std::vector<std::string> getRegions(const Range& range = Range(0, 0)) const = 0;
    virtual std::vector<std::string> getSynapseClass(const Range& range = Range(0, 0)) const = 0;

    virtual std::vector<size_t> getIndexEtypes(const Range& range = Range(0, 0)) const = 0;
    virtual std::vector<size_t> getIndexMtypes(const Range& range = Range(0, 0)) const = 0;
    virtual std::vector<size_t> getIndexRegions(const Range& range = Range(0, 0)) const = 0;
    virtual std::vector<size_t> getIndexSynapseClass(const Range& range = Range(0, 0)) const = 0;

    virtual std::vector<std::string> listAllEtypes() const = 0;
    virtual std::vector<std::string> listAllMtypes() const = 0;
    virtual std::vector<std::string> listAllRegions() const = 0;
    virtual std::vector<std::string> listAllSynapseClass() const = 0;
};





inline MVDType::MVDType _mvd_format(const std::string & filename) {
    using boost::algorithm::ends_with;
    // mvd2
    const std::string mvd_ext = ".mvd2";
    if (ends_with(filename, ".mvd2")) {
        return MVDType::MVD2;
    }
    if (ends_with(filename, ".mvd3")) {
        return MVDType::MVD3;
    }
    // everything else Sonata for now
    return MVDType::Sonata;
}



} // ::MVD
