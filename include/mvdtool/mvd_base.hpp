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
#include <vector>
#include <algorithm>
#include <boost/multi_array.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/integer.hpp>

#include "mvd_except.hpp"
#include "tsv.hpp"

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


class Range{
public:
    inline Range(const size_t offset_=0, const size_t count_=0) : offset(offset_), count(count_) {}

    inline static Range all() { return Range(0,0); }
    inline size_t adjust_count(const size_t total_range) const { return count > 0 ? count : total_range - offset; }
    inline size_t calculate_end(const size_t total_range) const { return count > 0 ? offset + count : total_range; }
    size_t offset;
    size_t count;
};


class MVDFile {
public:
    inline MVDFile() {}
    inline virtual ~MVDFile() {}
    virtual size_t getNbNeuron() const = 0;
    virtual Positions getPositions(const Range & range = Range::all()) const = 0;
    virtual Rotations getRotations(const Range & range = Range::all()) const = 0;
};


///
/// \brief Higher level interface for functional access
///
class File : public MVDFile {
public:
    inline File() = default;
    inline virtual ~File() = default;
    virtual size_t size() const {
        return getNbNeuron();
    }

    virtual void openComboTsv(const std::string& filename) = 0;

    virtual bool hasRotations() const = 0;

    virtual std::vector<std::string> getMorphologies(const Range& range = Range::all()) const = 0;
    virtual std::vector<std::string> getEtypes(const Range& range = Range::all()) const = 0;
    virtual std::vector<std::string> getMtypes(const Range& range = Range::all()) const = 0;
    virtual std::vector<std::string> getEmodels(const Range& range = Range::all()) const = 0;
    virtual std::vector<std::string> getRegions(const Range& range = Range::all()) const = 0;
    virtual std::vector<std::string> getSynapseClass(const Range& range = Range::all()) const = 0;

    virtual bool hasMiniFrequencies() const = 0;
    virtual std::vector<double> getExcMiniFrequencies(const Range & range = Range::all()) const = 0;
    virtual std::vector<double> getInhMiniFrequencies(const Range & range = Range::all()) const = 0;

    virtual bool hasCurrents() const = 0;
    virtual std::vector<double> getThresholdCurrents(const Range& range = Range::all()) const = 0;
    virtual std::vector<double> getHoldingCurrents(const Range& range = Range::all()) const = 0;

    virtual std::vector<size_t> getIndexEtypes(const Range& range = Range::all()) const = 0;
    virtual std::vector<size_t> getIndexMtypes(const Range& range = Range::all()) const = 0;
    virtual std::vector<size_t> getIndexRegions(const Range& range = Range::all()) const = 0;
    virtual std::vector<size_t> getIndexSynapseClass(const Range& range = Range::all()) const = 0;

    virtual std::vector<std::string> listAllEtypes() const = 0;
    virtual std::vector<std::string> listAllMtypes() const = 0;
    virtual std::vector<std::string> listAllEmodels() const = 0;
    virtual std::vector<std::string> listAllRegions() const = 0;
    virtual std::vector<std::string> listAllSynapseClass() const = 0;
};



inline MVDType::MVDType _mvd_format(const std::string & filename) {
    using boost::algorithm::ends_with;
    // mvd2
    if (ends_with(filename, ".mvd2")) {
        return MVDType::MVD2;
    }
    // mvd3
    if (ends_with(filename, ".mvd3")) {
        return MVDType::MVD3;
    }
    // everything else Sonata for now
    return MVDType::Sonata;
}


}  // namespace MVD
