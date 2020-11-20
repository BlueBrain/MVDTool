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

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <set>
#include <string>
#include <vector>

#include <highfive/H5DataSet.hpp>

#include "../mvd3.hpp"
#include "../mvd_except.hpp"
#include "../utils.hpp"

namespace {

template <typename T>
inline std::vector<T> get_data_for_selection(const HighFive::DataSet& dataset,
                                             const MVD3::Range& range) {
    std::vector<T> data_values;
    const size_t n_elem = dataset.getSpace().getDimensions()[0];

    dataset.select({range.offset}, {range.adjust_count(n_elem)}).read(data_values);
    return data_values;
}

template <typename T>
inline std::vector<T> resolve_index(const HighFive::DataSet& index,
                                    const MVD3::Range& range,
                                    const HighFive::DataSet& data) {
    std::vector<T> values, result;
    std::vector<size_t> references = get_data_for_selection<size_t>(index, range);
    const size_t n_elem = data.getSpace().getDimensions()[0];

    size_t first = n_elem;
    size_t last = 0;
    for (auto i: references) {
        if (i >= n_elem) {
            std::ostringstream ss;
            ss << "Invalid index reference " << i << " in an dataset of size " << n_elem;
            throw MVDParserException(ss.str());
        }
        first = std::min(first, i);
        last = std::max(last, i);
    }

    if (first == 0 && last == n_elem - 1) {
        data.read(values);
    } else {
        data.select({first}, {last - first + 1}).read(values);
    }

    result.reserve(references.size());

    for (auto i: references) {
        result.push_back(values[i - first]);
    }
    return result;
}

template <typename T, typename FuncT>
std::vector<T> tsv_get_chunked(const MVD3::MVD3File& mvd,
                               const FuncT& f,
                               const MVD3::Range& range) {
    const size_t CHUNK_SIZE = 256;
    std::vector<T> output;
    const size_t total_size = mvd.getNbNeuron();
    const size_t end = range.calculate_end(total_size);
    output.reserve(range.adjust_count(total_size));

    for (auto index = range.offset; index < end; index += CHUNK_SIZE) {
        const MVD::Range subrange(index, std::min(CHUNK_SIZE, end - index));
        auto chunk = f(mvd.getMECombos(subrange), mvd.getMorphologies(subrange));
        std::move(chunk.begin(), chunk.end(), std::back_inserter(output));
    }
    return output;
}

// Use constexpr char[] as const std::string is initialized too late for
// static objects using MVD::File in certain cases

// cells properties
constexpr char did_cells_positions[] = "/cells/positions";
constexpr char did_cells_rotations[] = "/cells/orientations";

// cells properties namespace
constexpr char did_cells_hypercolumn[] = "/cells/properties/hypercolumn";
constexpr char did_cells_minicolmun[] = "/cells/properties/minicolumn";
constexpr char did_cells_layer[] = "/cells/properties/layer";
constexpr char did_cells_exc_mini_freq[] = "/cells/properties/exc_mini_frequency";
constexpr char did_cells_inh_mini_freq[] = "/cells/properties/inh_mini_frequency";

// cells index
constexpr char did_cells_index_morpho[] = "/cells/properties/morphology";
constexpr char did_cells_index_etypes[] = "/cells/properties/etype";
constexpr char did_cells_index_mtypes[] = "/cells/properties/mtype";
constexpr char did_cells_index_mecombo[] = "/cells/properties/me_combo";
constexpr char did_cells_index_regions[] = "/cells/properties/region";
constexpr char did_cells_index_synapse_class[] = "/cells/properties/synapse_class";

// data
constexpr char did_lib_data_morpho[] = "/library/morphology";
constexpr char did_lib_data_etypes[] = "/library/etype";
constexpr char did_lib_data_mtypes[] = "/library/mtype";
constexpr char did_lib_data_mecombo[] = "/library/me_combo";
constexpr char did_lib_data_regions[] = "/library/region";
constexpr char did_lib_data_syn_class[] = "/library/synapse_class";

constexpr char did_lib_NONE[] = "";
const MVD::Range range_ALL(0, 0);

// circuit
constexpr char did_lib_circuit_seeds[] = "/circuit/seeds";

using vec_string = std::vector<std::string>;

} // namespace


namespace MVD3 {

using namespace MVD::utils;


inline MVD3File::MVD3File(const std::string& str)
    : _filename(str)
    , _hdf5_file(str)
    , _nb_neurons(0) {}


inline void MVD3File::openComboTsv(const std::string& filename) {
    _tsv_file = std::make_unique<TSV::TSVFile>(filename, TSVColumn::ComboName);
}


inline size_t MVD3File::getNbNeuron() const {
    if (_nb_neurons == 0) {
        try {
            std::vector<size_t> dims =
                _hdf5_file.getDataSet(did_cells_positions).getSpace().getDimensions();
            if (dims.size() < 1) {
                throw MVDParserException("Invalid Dataset dimension in MVD3 file");
            }
            (const_cast<size_t&>(_nb_neurons)) = dims[0];  // cache number of neurons in MVD3 file
                                                           // itself
        } catch (HighFive::Exception& e) {
            std::ostringstream ss;
            ss << "Unable to parse " << _filename << " " << e.what();
            throw MVDParserException(ss.str());
        }
    }
    return _nb_neurons;
}


inline Positions MVD3File::getPositions(const Range& range) const {
    Positions res;
    HighFive::DataSet set = _hdf5_file.getDataSet(did_cells_positions);
    const auto size = set.getSpace().getDimensions()[0];
    set.select({range.offset, 0}, {range.adjust_count(size), 3}).read(res);
    return res;
}


inline Rotations MVD3File::getRotations(const Range& range) const {
    Rotations res;
    HighFive::DataSet set = _hdf5_file.getDataSet(did_cells_rotations);
    const auto size = set.getSpace().getDimensions()[0];
    set.select({range.offset, 0}, {range.adjust_count(size), 4}).read(res);
    return res;
}


inline bool MVD3File::hasRotations() const {
    return _hdf5_file.exist(did_cells_rotations);
}


inline std::vector<std::string> MVD3File::getMorphologies(const Range& range) const {
    return getDataFromMVD(did_cells_index_morpho, did_lib_data_morpho, range);
}


inline std::vector<std::string> MVD3File::getEtypes(const Range& range) const {
    return getDataFromTSVorMVD(
        did_cells_index_etypes, did_lib_data_etypes, TSVColumn::EType, range);
}


inline std::vector<std::string> MVD3File::getEmodels(const Range& range) const {
    return getDataFromTSV(TSVColumn::EModel, range);
}


inline std::vector<std::string> MVD3File::getMtypes(const Range& range) const {
    return getDataFromTSVorMVD(
        did_cells_index_mtypes, did_lib_data_mtypes, TSVColumn::FullMType, range);
}


inline std::vector<std::string> MVD3File::getMECombos(const Range& range) const {
    return getDataFromMVD(did_cells_index_mecombo, did_lib_data_mecombo, range);
}


inline std::vector<std::string> MVD3File::getRegions(const Range& range) const {
    return getDataFromMVD(did_cells_index_regions, did_lib_data_regions, range);
}


inline std::vector<int32_t> MVD3File::getHyperColumns(const Range& range) const {
    return getDataFromMVD<int32_t>(did_cells_hypercolumn, did_lib_NONE, range);
}


inline std::vector<int32_t> MVD3File::getMiniColumns(const Range& range) const {
    return getDataFromMVD<int32_t>(did_cells_minicolmun, did_lib_NONE, range);
}


inline std::vector<std::string> MVD3File::getLayers(const Range& range) const {
    const auto raw_data = _hdf5_file.getDataSet(did_cells_layer);
    HighFive::DataType dset_type = raw_data.getDataType();
    if (dset_type == HighFive::AtomicType<std::string>())
        return getDataFromMVD<std::string>(did_cells_layer, did_lib_NONE, range);
    else {
        auto vec_int = getDataFromMVD<int32_t>(did_cells_layer, did_lib_NONE, range);
        std::vector<std::string> res;
        std::transform(std::begin(vec_int),
                       std::end(vec_int),
                       std::back_inserter(res),
                       [](double d) { return boost::lexical_cast<std::string>(d); });
        return res;
    }
}

inline bool MVD3File::hasMiniFrequencies() const {
    return _hdf5_file.exist(did_cells_exc_mini_freq) && _hdf5_file.exist(did_cells_inh_mini_freq);
}

inline std::vector<double> MVD3File::getExcMiniFrequencies(const Range& range) const {
    return getDataFromMVD<double>(did_cells_exc_mini_freq, did_lib_NONE, range);
}


inline std::vector<double> MVD3File::getInhMiniFrequencies(const Range& range) const {
    return getDataFromMVD<double>(did_cells_inh_mini_freq, did_lib_NONE, range);
}


inline bool MVD3File::hasCurrents() const {
    return static_cast<bool>(_tsv_file);
}


inline std::vector<double> MVD3File::getThresholdCurrents(const Range& range) const {
    return getDataFromTSV<double>(TSVColumn::ThresholdCurrent, range);
}


inline std::vector<double> MVD3File::getHoldingCurrents(const Range& range) const {
   return getDataFromTSV<double>(TSVColumn::HoldingCurrent, range);
}


inline std::vector<std::string> MVD3File::getSynapseClass(const Range& range) const {
    return getDataFromMVD(did_cells_index_synapse_class, did_lib_data_syn_class, range);
}


inline std::vector<size_t> MVD3File::getIndexMorphologies(const Range& range) const {
    return getDataFromMVD<size_t>(did_cells_index_morpho, did_lib_NONE, range);
}


inline std::vector<size_t> MVD3File::getIndexEtypes(const Range& range) const {
    return getDataFromMVD<size_t>(did_cells_index_etypes, did_lib_NONE, range);
}

inline std::vector<size_t> MVD3File::getIndexMtypes(const Range& range) const {
    return getDataFromMVD<size_t>(did_cells_index_mtypes, did_lib_NONE, range);
}

inline std::vector<size_t> MVD3File::getIndexRegions(const Range& range) const {
    return getDataFromMVD<size_t>(did_cells_index_regions, did_lib_NONE, range);
}

inline std::vector<size_t> MVD3File::getIndexSynapseClass(const Range& range) const {
    return getDataFromMVD<size_t>(did_cells_index_synapse_class, did_lib_NONE, range);
}

// list ALL group

inline std::vector<std::string> MVD3File::listAllMorphologies() const {
    return getDataFromMVD(did_lib_data_morpho, did_lib_NONE, range_ALL);
}

inline std::vector<std::string> MVD3File::listAllEtypes() const {
    return getDataFromMVD(did_lib_data_etypes, did_lib_NONE, range_ALL);
}

inline std::vector<std::string> MVD3File::listAllMtypes() const {
    return getDataFromMVD(did_lib_data_mtypes, did_lib_NONE, range_ALL);
}

inline std::vector<std::string> MVD3File::listAllRegions() const {
    return getDataFromMVD(did_lib_data_regions, did_lib_NONE, range_ALL);
}

inline std::vector<std::string> MVD3File::listAllSynapseClass() const {
    return getDataFromMVD(did_lib_data_syn_class, did_lib_NONE, range_ALL);
}

// Emodels are only avail within TSV
inline std::vector<std::string> MVD3File::listAllEmodels() const {
    if(!_tsv_file) {
        throw MVDException("No TSV file is opened with MVD3 to extract all the emodels.");
    }
    std::vector<std::string> emodels;
    const auto all_entries = _tsv_file->getAll();
    emodels.reserve(all_entries.size());
    for (const TSV::MEComboEntry& entry : all_entries) {
        emodels.push_back(entry.eModel);
    }
    vector_remove_dups(emodels);
    return emodels;
}


inline std::vector<double> MVD3File::getCircuitSeeds() const {
    std::vector<double> seeds;

    HighFive::DataSet seeds_dataset = _hdf5_file.getDataSet(did_lib_circuit_seeds);
    seeds_dataset.read(seeds);
    if (seeds.size() < 4) {
        throw MVDParserException(
            "Invalid MVD3 /circuit/seeds size, MVD3 should provide at least 4 seeds");
    }
    return seeds;
}


inline TSV::TSVFile::vector_ref MVD3File::getTSVInfo(const Range& range) const {
    if(!_tsv_file) {
        throw MVDException("No TSV file is opened with MVD3. Unable to get the TSVInfo");
    }
    const auto& f = [&](const vec_string& combos, const vec_string& morphos) {
        return  _tsv_file->get(combos, morphos);
    };
    using T = std::reference_wrapper<const TSV::MEComboEntry>;
    return tsv_get_chunked<T>(*this, f, range);
}



// Protected

template <typename T>
inline std::vector<T> MVD3File::getDataFromTSV(const TSVColumn& col,
                                               const Range& range) const {
    if (!_tsv_file) {
        throw MVDException("No TSV file is opened with MVD3. Unable to extract col #"
                           + std::to_string(col));
    }
    const auto& f = [&](const vec_string& combos, const vec_string& morphos) {
       return  _tsv_file->getField<T>(combos, morphos, col);
    };
    return tsv_get_chunked<T>(*this, f, range);
}


template <typename T>
inline std::vector<T> MVD3File::getDataFromMVD(const std::string& did_ds,
                                               const std::string& did_lib,
                                               const Range& range) const {
    const auto raw_data = _hdf5_file.getDataSet(did_ds);
    if (did_lib.empty()) {
        return get_data_for_selection<T>(raw_data, range);
    }
    return resolve_index<T>(raw_data, range, _hdf5_file.getDataSet(did_lib));
}


template <typename T>
inline std::vector<T> MVD3File::getDataFromTSVorMVD(const std::string& did_ds,
                                                    const std::string& did_lib,
                                                    const TSVColumn& col,
                                                    const Range& range) const {
    if (_tsv_file) {
        return getDataFromTSV<T>(col, range);
    }
    return getDataFromMVD<T>(did_ds, did_lib, range);
}


}  // namespace MVD3
