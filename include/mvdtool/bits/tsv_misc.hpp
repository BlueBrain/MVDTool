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

#include "../tsv_except.hpp"

namespace TSV {

inline std::vector<TSVInfo> TSVFile::getTSVInfo() {
    std::vector<TSVInfo> tsvFileInfoVec;
    for (auto it_infos: tsvFileInfo) {
        tsvFileInfoVec.push_back(it_infos.second);
    }
    return tsvFileInfoVec;
}

inline TSVInfo TSVFile::getTSVInfo(const std::string& me_combo, const std::string& morphology) {
    auto hasMEType = tsvFileInfo.find(std::make_pair(morphology, me_combo));
    if (hasMEType != tsvFileInfo.end()) {
        return hasMEType->second;
    }
    std::ostringstream ss;
    ss << "me_combo " << me_combo << " or morphology " << morphology << " not found in "
       << _filename << std::endl;
    throw TSVException(ss.str());
}

inline void split(const std::string& s, char delim, std::vector<std::string>& elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

inline unordered_pair_map TSVFile::readTSVFile(const std::string& filename) {
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
        tsvInfoLine.morphologyName = line_info[0];
        tsvInfoLine.layer = std::stoi(line_info[1]);
        tsvInfoLine.fullMType = line_info[2];
        tsvInfoLine.eType = line_info[3];
        tsvInfoLine.eModel = line_info[4];
        tsvInfoLine.comboName = line_info[5];
        if (line_info.size() == 6 ) {
            tsvInfoLine.thresholdCurrent = 0;
            tsvInfoLine.holdingCurrent = 0;
        } else if (line_info.size() == 8) {
            tsvInfoLine.thresholdCurrent = std::stod(line_info[6]);
            tsvInfoLine.holdingCurrent = std::stod(line_info[7]);
        }
        tsvInfos.insert({{tsvInfoLine.morphologyName, tsvInfoLine.comboName}, tsvInfoLine});
        line_info.clear();
    }

    return tsvInfos;
}

}  // namespace TSV
