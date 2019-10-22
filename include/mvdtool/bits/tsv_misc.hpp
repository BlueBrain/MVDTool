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

#include "../tsv_except.hpp"

namespace TSV {

inline std::vector<TSVInfo> TSVFile::getTSVInfos() {
    std::vector<TSVInfo> tsvFileInfoVec;
    for (auto it_infos: tsvFileInfo) {
        tsvFileInfoVec.push_back(it_infos.second);
    }
    return tsvFileInfoVec;
}

inline std::vector<TSVInfo> TSVFile::getTSVInfos(const std::vector<std::string>& me_combos, const std::vector<std::string>& morphologies) const {
    std::vector<TSV::TSVInfo> tsvInfos;
    for (const auto& mecombo_tupple: boost::combine(me_combos, morphologies)) {
        auto hasMEType = tsvFileInfo.find(std::make_pair(boost::get<0>(mecombo_tupple), boost::get<1>(mecombo_tupple)));
        if (hasMEType != tsvFileInfo.end()) {
            tsvInfos.push_back(hasMEType->second);
        } else {
            std::ostringstream ss;
            ss << "me_combo " << boost::get<0>(mecombo_tupple) << " or morphology " << boost::get<1>(mecombo_tupple) << " not found in "
               << _filename << std::endl;
            throw TSVException(ss.str());
        }
    }
    return tsvInfos;
}

inline std::vector<boost::int32_t> TSVFile::getLayers(const std::vector<std::string>& me_combos, const std::vector<std::string>& morphologies) const {
    std::vector<boost::int32_t> tsvLayers;
    std::vector<TSV::TSVInfo> tsvInfos = getTSVInfos(me_combos, morphologies);
    for (const auto& tsvInfo: tsvInfos) {
        tsvLayers.push_back(tsvInfo.layer);
    }
    return tsvLayers;
}

inline std::vector<std::string> TSVFile::getMtypes(const std::vector<std::string>& me_combos, const std::vector<std::string>& morphologies) const {
    std::vector<std::string> tsvMtypes;
    std::vector<TSV::TSVInfo> tsvInfos = TSVFile::getTSVInfos(me_combos, morphologies);
    for (const auto& tsvInfo: tsvInfos) {
        tsvMtypes.push_back(tsvInfo.fullMType);
    }
    return tsvMtypes;
}

inline std::vector<std::string> TSVFile::getEtypes(const std::vector<std::string>& me_combos, const std::vector<std::string>& morphologies) const {
    std::vector<std::string> tsvEtypes;
    std::vector<TSV::TSVInfo> tsvInfos = getTSVInfos(me_combos, morphologies);
    for (const auto& tsvInfo: tsvInfos) {
        tsvEtypes.push_back(tsvInfo.eType);
    }
    return tsvEtypes;
}

inline std::vector<std::string> TSVFile::getEmodels(const std::vector<std::string>& me_combos, const std::vector<std::string>& morphologies) const {
    std::vector<std::string> tsvEmodels;
    std::vector<TSV::TSVInfo> tsvInfos = getTSVInfos(me_combos, morphologies);
    for (const auto& tsvInfo: tsvInfos) {
        tsvEmodels.push_back(tsvInfo.eModel);
    }
    return tsvEmodels;
}

inline std::vector<double> TSVFile::getThresholdCurrents(const std::vector<std::string>& me_combos, const std::vector<std::string>& morphologies) const {
    std::vector<double> tsvThresholdCurrents;
    std::vector<TSV::TSVInfo> tsvInfos = getTSVInfos(me_combos, morphologies);
    for (const auto& tsvInfo: tsvInfos) {
        tsvThresholdCurrents.push_back(tsvInfo.thresholdCurrent);
    }
    return tsvThresholdCurrents;
}

inline std::vector<double> TSVFile::getHoldingCurrents(const std::vector<std::string>& me_combos, const std::vector<std::string>& morphologies) const {
    std::vector<double> tsvHoldingCurrents;
    std::vector<TSV::TSVInfo> tsvInfos = getTSVInfos(me_combos, morphologies);
    for (const auto& tsvInfo: tsvInfos) {
        tsvHoldingCurrents.push_back(tsvInfo.holdingCurrent);
    }
    return tsvHoldingCurrents;
}

inline void split(const std::string& s, char delim, std::vector<std::string>& elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

inline unordered_pair_map TSVFile::readTSVFile(const std::string& filename, tsv_columns id_column) {
    std::ifstream file(filename);
    std::string line;
    std::vector<std::string> line_info;
    unordered_pair_map tsvInfos;

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
        TSVInfo tsvInfoLine;
        split(line, '\t', line_info);
        if (line_info.size() != 6 && line_info.size() != 8) {
            std::ostringstream ss;
            ss << "Error in " << filename << " line " << line_index << ", unexpected nfields "
               << line_info.size() << "; expecting 6 (hippocampus) or 8 (somatosensory)"
               << std::endl;
            throw TSVParserException(ss.str());
        }
        tsvInfoLine.morphologyName = line_info[morph_name];
        tsvInfoLine.layer = std::stoi(line_info[layer]);
        tsvInfoLine.fullMType = line_info[fullmtype];
        tsvInfoLine.eType = line_info[etype];
        tsvInfoLine.eModel = line_info[emodel];
        tsvInfoLine.comboName = line_info[combo_name];
        if (line_info.size() == 6 ) {
            tsvInfoLine.thresholdCurrent = 0;
            tsvInfoLine.holdingCurrent = 0;
        } else if (line_info.size() == 8) {
            tsvInfoLine.thresholdCurrent = std::stod(line_info[threshold_current]);
            tsvInfoLine.holdingCurrent = std::stod(line_info[holding_current]);
        }
        tsvInfos.insert({{line_info[id_column], tsvInfoLine.morphologyName}, tsvInfoLine});
        line_info.clear();
    }

    return tsvInfos;
}

}  // namespace TSV
