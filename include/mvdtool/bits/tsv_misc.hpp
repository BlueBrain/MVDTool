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

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

#include <boost/range/combine.hpp>

#include "../mvd_except.hpp"


namespace TSV {

namespace detail {

inline void split(const std::string& s, char delim, std::vector<std::string>& elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

inline TSVFile::unordered_pair_map readTSVFile(const std::string& filename,
                                               const MEComboEntry::Column& column) {
    std::ifstream file(filename);
    std::string line;
    std::vector<std::string> line_info;
    TSVFile::unordered_pair_map MEComboEntrys;

    // Read first line to calculate the number of columns/info
    int line_index = 0;
    std::getline(file, line);
    line_index++;
    split(line, '\t', line_info);
    if (line_info.size() != 6 && line_info.size() != 8) {
        std::ostringstream ss;
        ss << "Error in " << filename << " line " << line_index << ", unexpected nfields "
           << line_info.size() << "; expecting 6 (hippocampus) or 8 (somatosensory)" << std::endl;
        throw TSVParserException(ss.str());
    }
    line_info.clear();

    while (std::getline(file, line)) {
        line_index++;
        MEComboEntry MEComboEntryLine;
        split(line, '\t', line_info);
        if (line_info.size() != 6 && line_info.size() != 8) {
            std::ostringstream ss;
            ss << "Error in " << filename << " line " << line_index << ". "
               << "Unexpected nfields " << line_info.size() << ". "
               << "Expecting 6 (hippocampus) or 8 (somatosensory)" << std::endl;
            throw TSVParserException(ss.str());
        }
        MEComboEntryLine.morphologyName = line_info[MEComboEntry::MorphologyName];
        MEComboEntryLine.layer = std::stoi(line_info[MEComboEntry::Layer]);
        MEComboEntryLine.fullMType = line_info[MEComboEntry::FullMType];
        MEComboEntryLine.eType = line_info[MEComboEntry::EType];
        MEComboEntryLine.eModel = line_info[MEComboEntry::EModel];
        MEComboEntryLine.comboName = line_info[MEComboEntry::ComboName];
        if (line_info.size() == 6) {
            MEComboEntryLine.thresholdCurrent = 0;
            MEComboEntryLine.holdingCurrent = 0;
        } else if (line_info.size() == 8) {
            MEComboEntryLine.thresholdCurrent = std::stod(line_info[MEComboEntry::ThresholdCurrent]);
            MEComboEntryLine.holdingCurrent = std::stod(line_info[MEComboEntry::HoldingCurrent]);
        }
        MEComboEntrys.insert(
            {{line_info[column], MEComboEntryLine.morphologyName}, MEComboEntryLine});
        line_info.clear();
    }

    return MEComboEntrys;
}

}  // namespace detail


using namespace detail;


// class MEComboEntry
template <>
inline std::string MEComboEntry::get<std::string>(const Column col_id) const {
    switch (col_id) {
    case MorphologyName:
        return morphologyName;
    case FullMType:
        return fullMType;
    case EType:
        return eType;
    case EModel:
        return eModel;
    case ComboName:
        return comboName;
    default:
        throw MVDException(
            std::string("Cannot fetch field ") + std::to_string(col_id) + " as string");
    }
}

template <>
inline double MEComboEntry::get<double>(const Column col_id) const {
    switch (col_id) {
    case ThresholdCurrent:
        return thresholdCurrent;
    case HoldingCurrent:
        return holdingCurrent;
    default:
        throw MVDException(
            std::string("Cannot fetch field ") + std::to_string(col_id) + " as double");
    }
}

template <>
inline int32_t MEComboEntry::get<int32_t>(const Column col_id) const {
    switch (col_id) {
    case Layer:
        return layer;
    default:
        throw MVDException(
            std::string("Cannot fetch field ") + std::to_string(col_id) + " as int32_t");
    }
}



inline std::ostream& operator<<(std::ostream& os, MEComboEntry const& MEComboEntry) {
    return os << MEComboEntry.morphologyName << "\t" << MEComboEntry.layer << "\t"
              << MEComboEntry.fullMType << "\t" << MEComboEntry.eType << "\t" << MEComboEntry.eModel
              << "\t" << MEComboEntry.comboName << "\t" << MEComboEntry.thresholdCurrent << "\t"
              << MEComboEntry.holdingCurrent << std::endl;
}



// class TSVFile

inline TSVFile::TSVFile(const std::string& filename)
    : _filename(filename)
    , tsvFileInfo(readTSVFile(filename, MEComboEntry::ComboName)) {}


inline TSVFile::TSVFile(const std::string& filename, const MEComboEntry::Column& column)
    : _filename(filename)
    , tsvFileInfo(readTSVFile(filename, column)) {}


inline TSVFile::vector_ref TSVFile::getAll() const {
    TSVFile::vector_ref entries;
    entries.reserve(tsvFileInfo.size());
    for (const auto& it_infos: tsvFileInfo) {
        entries.push_back(it_infos.second);
    }
    return entries;
}


inline TSVFile::vector_ref TSVFile::get(const std::vector<std::string>& me_combos,
                                        const std::vector<std::string>& morphologies) const {
    TSVFile::vector_ref entries;
    entries.reserve(me_combos.size());
    for (const auto& mecombo_tuple: boost::combine(me_combos, morphologies)) {
        auto metype = tsvFileInfo.find(std::make_pair(boost::get<0>(mecombo_tuple),
                                                      boost::get<1>(mecombo_tuple)));
        if (metype != tsvFileInfo.end()) {
            entries.push_back(metype->second);
        } else {
            std::ostringstream ss;
            ss << "me_combo " << boost::get<0>(mecombo_tuple) << " or morphology "
               << boost::get<1>(mecombo_tuple) << " not found in " << _filename << std::endl;
            throw TSVException(ss.str());
        }
    }
    return entries;
}


template <typename T>
inline std::vector<T> TSVFile::getField(const std::vector<std::string>& me_combos,
                                        const std::vector<std::string>& morphologies,
                                        const MEComboEntry::Column& column) const {
    std::vector<T> entries;
    entries.reserve(me_combos.size());
    for (const MEComboEntry& item: get(me_combos, morphologies)) {
        entries.push_back(item.get<T>(column));
    }
    return entries;
}


}  // namespace TSV
