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

#include <boost/math/quaternion.hpp>

#include <highfive/H5DataSet.hpp>

#include "../sonata.hpp"
#include "../utils.hpp"

namespace {

using namespace bbp;
using namespace MVD::utils;

// Naming convention
//
// Use constexpr char[] as const std::string is initialized too late for
// static objects using MVD::File in certain cases
constexpr char default_population_name[] = "default";
constexpr char did_layer[] = "layer";
constexpr char did_exc_mini_freq[] = "exc_mini_frequency";
constexpr char did_inh_mini_freq[] = "inh_mini_frequency";
constexpr char did_morpho[] = "morphology";
constexpr char did_etypes[] = "etype";
constexpr char did_mtypes[] = "mtype";
constexpr char did_emodel[] = "model_template";
constexpr char did_regions[] = "region";
constexpr char did_synapse_class[] = "synapse_class";

constexpr char did_threshold_current[] = "threshold_current";
constexpr char did_holding_current[] = "holding_current";
constexpr char did_model_template[] = "model_template";

inline auto select(const MVD::Range& range, size_t size) {
    return sonata::Selection({{range.offset, range.calculate_end(size)}});
}


inline decltype(auto) open_population(const std::string &filename, std::string pop_name) {
    sonata::NodeStorage _storage(filename);
    const auto& populations = _storage.populationNames();
    if (populations.empty()) {
        throw MVDException("Sonata file doesn't contain any population");
    }
    // Attempt to load the default or single population
    if (pop_name.empty()) {
        if (populations.count(default_population_name)) {
            pop_name = default_population_name;
        } else if (populations.size() == 1) {
            pop_name = *populations.begin();
        } else {
            throw MVDException("Multiple populations found in Sonata file. "
                               "Please select one population explicitly.");
        }
    }
    return std::make_unique<sonata::NodePopulation>(filename, "", pop_name);
}


// In case the enumeration is not available, get all values and drop duplicates in order
inline std::vector<std::string> listAllValues(const sonata::NodePopulation* pop,
                                              const std::string& did) {
    try {
        return pop->enumerationValues(did);
    } catch (const std::runtime_error&) {
        auto values = pop->getAttribute<std::string>(did,
            sonata::Selection({{0, pop->size()}}));
        vector_remove_dups(values);
        return values;
    }
}

}  // namespace


namespace MVD {

using namespace bbp::sonata;

inline SonataFile::SonataFile(const std::string &filename, const std::string &pop_name)
        : pop_(open_population(filename, pop_name)), size_(pop_->size()) {}

inline void SonataFile::openComboTsv(const std::string&) {}

inline Positions SonataFile::getPositions(const Range &range) const {
    const auto adjusted_count = range.adjust_count(size_);
    Positions res{boost::extents[adjusted_count][3]};

    auto xs = pop_->getAttribute<double>("x", select(range, size_));
    auto ys = pop_->getAttribute<double>("y", select(range, size_));
    auto zs = pop_->getAttribute<double>("z", select(range, size_));

    // No direct slicing write access from std::vector
    for (size_t i = 0; i < adjusted_count; ++i) {
        res[i][0] = xs[i];
        res[i][1] = ys[i];
        res[i][2] = zs[i];
    }

    return res;
}

inline Rotations SonataFile::getQuaternionRotations(const Range &range) const {
    const auto adjusted_count = range.adjust_count(size_);
    Positions res{boost::extents[adjusted_count][4]};

    auto xs = pop_->getAttribute<double>("orientation_x", select(range, size_));
    auto ys = pop_->getAttribute<double>("orientation_y", select(range, size_));
    auto zs = pop_->getAttribute<double>("orientation_z", select(range, size_));
    auto ws = pop_->getAttribute<double>("orientation_w", select(range, size_));

    // No direct slicing write access from std::vector
    for (size_t i = 0; i < adjusted_count; ++i) {
        res[i][0] = xs[i];
        res[i][1] = ys[i];
        res[i][2] = zs[i];
        res[i][3] = ws[i];
    }

    return res;
}

inline Rotations SonataFile::getAngularRotations(const Range &range) const {
    using Quaternion = boost::math::quaternion<double>;

    const auto adjusted_count = range.adjust_count(size_);
    Positions res{boost::extents[adjusted_count][4]};

    const auto attrs = pop_->attributeNames();
    const bool use_x = attrs.count("rotation_angle_xaxis") > 0;
    const bool use_y = attrs.count("rotation_angle_yaxis") > 0;
    const bool use_z = attrs.count("rotation_angle_zaxis") > 0;

    for (size_t i = 0; i < adjusted_count; ++i) {
        Quaternion rot{1., 0., 0., 0.};
        Range r{range.offset + i, 1};

        if (use_z) {
            auto halfangle = pop_->getAttribute<double>("rotation_angle_zaxis", select(r, size_))[0] * .5;
            rot *= Quaternion{cos(halfangle), 0., 0., sin(halfangle)};
        }
        if (use_y) {
            auto halfangle = pop_->getAttribute<double>("rotation_angle_yaxis", select(r, size_))[0] * .5;
            rot *= Quaternion{cos(halfangle), 0., sin(halfangle), 0.};
        }
        if (use_x) {
            auto halfangle = pop_->getAttribute<double>("rotation_angle_xaxis", select(r, size_))[0] * .5;
            rot *= Quaternion{cos(halfangle), sin(halfangle), 0., 0.};
        }
        res[i][0] = rot.R_component_2();
        res[i][1] = rot.R_component_3();
        res[i][2] = rot.R_component_4();
        res[i][3] = rot.R_component_1();
    }

    return res;
}

inline Rotations SonataFile::getRotations(const Range& range) const{
    const auto attrs = pop_->attributeNames();
    const bool quat = (attrs.count("orientation_x") +
                       attrs.count("orientation_y") +
                       attrs.count("orientation_z") +
                       attrs.count("orientation_w")) == 4;

    if (quat) {
        return getQuaternionRotations(range);
    }
    return getAngularRotations(range);
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

inline std::vector<std::string> SonataFile::getMorphologies(const Range& range) const{
    return pop_->getAttribute<std::string>(did_morpho, select(range, size_));
}

inline std::vector<std::string> SonataFile::getEtypes(const Range& range) const{
    return pop_->getAttribute<std::string>(did_etypes, select(range, size_));
}

inline std::vector<std::string> SonataFile::getMtypes(const Range& range) const{
    return pop_->getAttribute<std::string>(did_mtypes, select(range, size_));
}

inline std::vector<std::string> SonataFile::getLayers(const Range& range) const{
    return pop_->getAttribute<std::string>(did_layer, select(range, size_));
}

inline bool SonataFile::hasMiniFrequencies() const {
    const auto attrs = pop_->attributeNames();
    return attrs.count(did_exc_mini_freq) && attrs.count(did_inh_mini_freq);
}

inline std::vector<double> SonataFile::getExcMiniFrequencies(const Range& range) const{
    return pop_->getAttribute<double>(did_exc_mini_freq, select(range, size_));
}

inline std::vector<double> SonataFile::getInhMiniFrequencies(const Range& range) const{
    return pop_->getAttribute<double>(did_inh_mini_freq, select(range, size_));
}

inline std::vector<std::string> SonataFile::getEmodels(const Range& range) const{
    auto model_tpl = pop_->getAttribute<std::string>(did_emodel, select(range, size_));
    for (auto& model : model_tpl) {
        model = model.substr(model.find(':') + 1);
    }
    return model_tpl;
}

inline std::vector<std::string> SonataFile::getRegions(const Range & range) const{
    return pop_->getAttribute<std::string>(did_regions, select(range, size_));
}

inline std::vector<std::string> SonataFile::getSynapseClass(const Range & range) const{
    return pop_->getAttribute<std::string>(did_synapse_class, select(range, size_));
}

inline bool SonataFile::hasCurrents() const {
    const auto& dynAttrs = pop_->dynamicsAttributeNames();
    return dynAttrs.count(did_threshold_current) && dynAttrs.count(did_holding_current);
}

inline std::vector<double> SonataFile::getThresholdCurrents(const Range& range) const {
    return pop_->getDynamicsAttribute<double>(did_threshold_current, select(range, size_));
}

inline std::vector<double> SonataFile::getHoldingCurrents(const Range& range) const {
    return pop_->getDynamicsAttribute<double>(did_holding_current, select(range, size_));
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
    return listAllValues(pop_.get(), did_etypes);
}

inline std::vector<std::string> SonataFile::listAllMtypes() const{
    return listAllValues(pop_.get(), did_mtypes);
}

inline std::vector<std::string> SonataFile::listAllLayers() const{
    return listAllValues(pop_.get(), did_layer);
}

inline std::vector<std::string> SonataFile::listAllEmodels() const{
    return listAllValues(pop_.get(), did_emodel);
}

inline std::vector<std::string> SonataFile::listAllRegions() const{
    return listAllValues(pop_.get(), did_regions);
}

inline std::vector<std::string> SonataFile::listAllSynapseClass() const{
    return listAllValues(pop_.get(), did_synapse_class);
}

inline bool SonataFile::hasAttribute(const std::string& name) const {
    const auto attrs = pop_->attributeNames();
    return attrs.count(name);
}

inline bool SonataFile::hasDynamicsAttribute(const std::string& name) const {
    const auto dynAttrs = pop_->dynamicsAttributeNames();
    return dynAttrs.count(name);
}

inline std::string SonataFile::getAttributeDataType(const std::string& name) const {
    if (hasDynamicsAttribute(name)) {
        return pop_->_dynamicsAttributeDataType(name);
    }
    return pop_->_attributeDataType(name);
}

template <typename T>
inline std::vector<T> SonataFile::getAttribute(const std::string& name, const Range& range) const {
    if (hasDynamicsAttribute(name)) {
        return pop_->getDynamicsAttribute<T>(name, select(range, size_));
    }
    return pop_->getAttribute<T>(name, select(range, size_));
}

}
