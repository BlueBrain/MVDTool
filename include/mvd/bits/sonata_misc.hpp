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
#pragma once

#include <string>
#include <vector>

#include <boost/math/quaternion.hpp>

#include <highfive/H5DataSet.hpp>

#include "../sonata.hpp"

namespace{

// Naming convention
const std::string did_layer = "layer";
const std::string did_morpho = "morphology";
const std::string did_etypes = "etype";
const std::string did_mtypes = "mtype";
const std::string did_regions = "region";
const std::string did_synapse_class = "synapse_class";

bbp::sonata::Selection select(const MVD::Range& range, size_t size) {
    if (range.count > 0) {
        return bbp::sonata::Selection({{range.offset, range.offset + range.count}});
    }
    return bbp::sonata::Selection({{range.offset, size - range.offset}});
}

}

namespace MVD {

using namespace bbp::sonata;

inline SonataFile::SonataFile(const std::string &filename, const std::string& name)
    : pop_(new NodePopulation(filename, "", name))
    , size_(pop_->size())
{ }

inline Positions SonataFile::getPositions(const Range & range) const{
    Positions res{boost::extents[range.count > 0 ? range.count : size_][3]};

    auto xs = pop_->getAttribute<double>("x", select(range, size_));
    auto ys = pop_->getAttribute<double>("y", select(range, size_));
    auto zs = pop_->getAttribute<double>("z", select(range, size_));

    // No direct slicing write access from std::vector
    for (size_t i = 0; i < (range.count > 0 ? range.count : size_); ++i) {
        res[i][0] = xs[i];
        res[i][1] = ys[i];
        res[i][2] = zs[i];
    }

    return res;
}

inline Rotations SonataFile::getRotations(const Range & range) const{
    using Quaternion = boost::math::quaternion<double>;

    Positions res{boost::extents[range.count > 0 ? range.count : size_][4]};

    const auto attrs = pop_->attributeNames();
    const bool quat = (attrs.count("orientation_x") +
                       attrs.count("orientation_y") +
                       attrs.count("orientation_z") +
                       attrs.count("orientation_w")) == 4;

    if (quat) {
        auto xs = pop_->getAttribute<double>("orientation_x", select(range, size_));
        auto ys = pop_->getAttribute<double>("orientation_y", select(range, size_));
        auto zs = pop_->getAttribute<double>("orientation_z", select(range, size_));
        auto ws = pop_->getAttribute<double>("orientation_w", select(range, size_));

        // No direct slicing write access from std::vector
        for (size_t i = 0; i < (range.count > 0 ? range.count : size_); ++i) {
            res[i][0] = xs[i];
            res[i][1] = ys[i];
            res[i][2] = zs[i];
            res[i][3] = ws[i];
        }
    } else {
        for (size_t i = 0; i < (range.count > 0 ? range.count : size_); ++i) {
            Quaternion rot{1., 0., 0., 0.};

            if (attrs.count("rotation_angle_zaxis") > 0) {
                auto halfangle = pop_->getAttribute<double>("rotation_angle_zaxis", select(range, size_))[0] * .5;
                rot *= Quaternion{cos(halfangle), 0., 0., sin(halfangle)};
            }
            if (attrs.count("rotation_angle_yaxis") > 0) {
                auto halfangle = pop_->getAttribute<double>("rotation_angle_yaxis", select(range, size_))[0] * .5;
                rot *= Quaternion{cos(halfangle), 0., sin(halfangle), 0.};
            }
            if (attrs.count("rotation_angle_xaxis") > 0) {
                auto halfangle = pop_->getAttribute<double>("rotation_angle_xaxis", select(range, size_))[0] * .5;
                rot *= Quaternion{cos(halfangle), sin(halfangle), 0., 0.};
            }
            res[i][0] = rot.R_component_2();
            res[i][1] = rot.R_component_3();
            res[i][2] = rot.R_component_4();
            res[i][3] = rot.R_component_1();
        }
    }

    return res;
}

inline bool SonataFile::hasRotations() const {
    const auto attrs = pop_->attributeNames();
    const bool quat = (attrs.count("orientation_x") +
                       attrs.count("orientation_y") +
                       attrs.count("orientation_z") +
                       attrs.count("orientation_w")) == 4;
    const bool angle = (attrs.count("rotation_angle_xaxis") +
                        attrs.count("rotation_angle_yaxis") +
                        attrs.count("rotation_angle_zaxis")) > 0;
    return quat or angle;
}


inline std::vector<std::string> SonataFile::getMorphologies(const Range & range) const{
    return pop_->getAttribute<std::string>(did_morpho, select(range, size_));
}

inline std::vector<std::string> SonataFile::getEtypes(const Range & range) const{
    return pop_->getAttribute<std::string>(did_etypes, select(range, size_));
}

inline std::vector<std::string> SonataFile::getMtypes(const Range & range) const{
    return pop_->getAttribute<std::string>(did_mtypes, select(range, size_));
}

inline std::vector<std::string> SonataFile::getRegions(const Range & range) const{
    return pop_->getAttribute<std::string>(did_regions, select(range, size_));
}

inline std::vector<std::string> SonataFile::getSynapseClass(const Range & range) const{
    return pop_->getAttribute<std::string>(did_synapse_class, select(range, size_));
}

inline std::vector<size_t> SonataFile::getIndexEtypes(const Range &range) const{
    return pop_->getAttribute<size_t>(did_etypes, select(range, size_));
}

inline std::vector<size_t> SonataFile::getIndexMtypes(const Range &range) const{
    return pop_->getAttribute<size_t>(did_mtypes, select(range, size_));
}

inline std::vector<size_t> SonataFile::getIndexRegions(const Range &range) const{
    return pop_->getAttribute<size_t>(did_regions, select(range, size_));
}

inline std::vector<size_t> SonataFile::getIndexSynapseClass(const Range &range) const{
    return pop_->getAttribute<size_t>(did_synapse_class, select(range, size_));
}

inline std::vector<std::string> SonataFile::listAllEtypes() const{
    return pop_->enumerationValues(did_etypes);
}

inline std::vector<std::string> SonataFile::listAllMtypes() const{
    return pop_->enumerationValues(did_mtypes);
}

inline std::vector<std::string> SonataFile::listAllRegions() const{
    return pop_->enumerationValues(did_regions);
}

inline std::vector<std::string> SonataFile::listAllSynapseClass() const{
    return pop_->enumerationValues(did_synapse_class);
}

}
