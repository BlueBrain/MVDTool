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

#ifndef H5_USE_BOOST
#define H5_USE_BOOST
#endif

#include <string>
#include <unordered_map>
#include <vector>

#include <boost/integer.hpp>

namespace TSV {

///
/// \brief The TSVInfo class
///
/// Includes all the information of a mecombo entry of the tsv file
///
struct TSVInfo {
    std::string morphologyName;
    boost::int32_t layer;
    std::string fullMType;
    std::string eType;
    std::string eModel;
    std::string comboName;
    double thresholdCurrent;
    double holdingCurrent;

    friend std::ostream& operator<<(std::ostream& os, TSVInfo const& tsvInfo) {
        return os << tsvInfo.morphologyName << "\t" << tsvInfo.layer << "\t" << tsvInfo.fullMType
                  << "\t" << tsvInfo.eType << "\t" << tsvInfo.eModel << "\t" << tsvInfo.comboName
                  << "\t" << tsvInfo.thresholdCurrent << "\t" << tsvInfo.holdingCurrent
                  << std::endl;
    }
};

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

using unordered_pair_map =
    std::unordered_map<std::pair<std::string, std::string>, TSVInfo, pair_hash>;

///
/// \brief The TSVFile class
///
/// Represent a tsv me combo file. Reads the information from the mecombo_emodel.tsv file
/// and saves them in a vector of TSVInfo objects.
///
class TSVFile {
  public:
    ///
    /// \brief TSVFile
    /// \param filename tsv file name
    ///
    /// Open and read a tsv file format at 'filename' path
    /// throw TSVException, or HighFive::Exception in case of error
    ///
    TSVFile(const std::string& filename)
        : _filename(filename)
        , tsvFileInfo(readTSVFile(filename)) {}

    ///
    /// \brief TSVFile
    ///
    /// Get all the me types defined in the tsv file
    ///
    std::vector<TSVInfo> getTSVInfo();

    ///
    /// \brief TSVFile
    /// \param me_combo me_combo string of the neuron me type
    /// \param morphology morphology name of the neuron me type
    ///
    /// Get the info included in the tsv file for the neuron type
    /// with the me_combo and morphology name requested
    /// throw TSVException, if me_combo or morphology don't match
    /// an entry in tsvFileInfo
    ///
    TSVInfo getTSVInfo(const std::string& me_combo, const std::string& morphology);

  private:
    std::string _filename;
    const unordered_pair_map tsvFileInfo;
    unordered_pair_map readTSVFile(const std::string& filename);
};

}  // namespace TSV

#include "bits/tsv_misc.hpp"
