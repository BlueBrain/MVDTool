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
#ifndef MVD3_MISC_HPP
#define MVD3_MISC_HPP

#include <string>
#include <vector>

#include <highfive/H5DataSet.hpp>

#include "../mvd3.hpp"
#include "../mvd_except.hpp"
#include "../utils.hpp"

namespace {

inline bool is_empty(const MVD3::Range & range){
    return range.count == 0;
}

template<typename T>
inline std::vector<T> get_data_for_selection(HighFive::DataSet & dataset, const MVD3::Range & range){
    std::vector<T> data_values;

    if(!is_empty(range)){
        dataset.select({range.offset}, {range.count}).read(data_values);
    }else{
        dataset.read(data_values);
    }
    return data_values;
}

template<typename T>
inline std::vector<T> resolve_index(HighFive::DataSet & index,
                                    const MVD3::Range & range,
                                    HighFive::DataSet & data) {
    std::vector<T> values, result;
    std::vector<size_t> references = get_data_for_selection<size_t>(index, range);
    const size_t n_elem = data.getSpace().getDimensions()[0];

    size_t first = n_elem;
    size_t last = 0;
    for(auto i : references){
        if(i >= n_elem){
            std::ostringstream ss;
            ss << "Invalid index reference " << i << " in an dataset of size " << n_elem;
            throw MVDParserException(ss.str());
        }
        first = std::min(first, i);
        last = std::max(last, i);
    }

    if(first == 0 && last == n_elem - 1){
        data.read(values);
    }else{
        data.select({first}, {last - first + 1}).read(values);
    }

    result.reserve(references.size());

    for(auto i : references){
        result.push_back(values[i - first]);
    }
    return result;
}


// cells properties
const std::string did_cells_positions = "/cells/positions";
const std::string did_cells_rotations = "/cells/orientations";

// cells properties namespace
const std::string did_cells_hypercolumn = "/cells/properties/hypercolumn";
const std::string did_cells_minicolmun = "/cells/properties/minicolumn";
const std::string did_cells_layer = "/cells/properties/layer";

// cells index
const std::string did_cells_index_morpho = "/cells/properties/morphology";
const std::string did_cells_index_etypes = "/cells/properties/etype";
const std::string did_cells_index_mtypes = "/cells/properties/mtype";
const std::string did_cells_index_mecombo = "/cells/properties/me_combo";
const std::string did_cells_index_regions = "/cells/properties/region";
const std::string did_cells_index_synapse_class = "/cells/properties/synapse_class";

// data
const std::string did_lib_data_morpho = "/library/morphology";
const std::string did_lib_data_etypes = "/library/etype";
const std::string did_lib_data_mtypes = "/library/mtype";
const std::string did_lib_data_mecombo = "/library/me_combo";
const std::string did_lib_data_regions = "/library/region";
const std::string did_lib_data_syn_class = "/library/synapse_class";

// circuit
const std::string did_lib_circuit_seeds = "/circuit/seeds";

}


namespace MVD3 {

inline MVD3File::MVD3File(const std::string& str)
    : _filename(str)
    , _hdf5_file(str)
    , _nb_neurons(0) {}


inline void MVD3File::readTSVInfo(const std::string& filename) {
    _tsv_file = std::make_unique<TSV::TSVFile>(filename, TSV::tsv_columns::combo_name);
}


template <typename T, typename FuncT>
inline std::vector<T> MVD3File::getTSVdata(const Range& range, const FuncT& tsvGetter) const{
    const size_t CHUNK_SIZE = 256;
    size_t start = range.offset;
    size_t end = range.count;
    std::vector<T> retData;
    if(start == 0 && end == 0){
        end = getNbNeuron();
    }
    for (auto index = start; index < end; index += CHUNK_SIZE) {
        std::vector <std::string> morphologies = getMorphologies(Range(index, std::min(CHUNK_SIZE, end - index)));
        std::vector <std::string> me_combos = getMECombos(Range(index, std::min(CHUNK_SIZE, end - index)));
        auto chunkData = tsvGetter(me_combos, morphologies);
        retData.insert(retData.end(), chunkData.begin(), chunkData.end());
    }
    return retData;
}

template <typename T, typename FuncT>
inline std::vector<T> MVD3File::getDataFromTsvOrMvd3WithIndex(const std::string& did_lib, const std::string& did_index, const Range& range, const FuncT& tsvGetter) const{
    if (_tsv_file) {
        return getTSVdata<T>(range, tsvGetter);
    } else {
        return get_resolve_field(did_lib, did_index, range);
    }
}

template <typename T, typename FuncT>
inline std::vector<T> MVD3File::getDataFromTsvOrMvd3WithoutIndex(const std::string& did, const Range& range, const FuncT& tsvGetter) const{
    if (_tsv_file) {
        return getTSVdata<T>(range, tsvGetter);
    } else {
        HighFive::DataSet set = _hdf5_file.getDataSet(did);
        return get_data_for_selection<boost::int32_t>(set, range);
    }
}

template <typename T, typename FuncT>
inline std::vector<T> MVD3File::getDataFromTsv(const std::string& fieldName, const Range& range, const FuncT& tsvGetter) const{
    if (!_tsv_file) {
        std::ostringstream ss;
        ss << "No tsv file is opened with MVD3 file " << _filename << " to extract " << fieldName << " information from"
           << std::endl;
        throw MVDException(ss.str());
    }
    return getTSVdata<T>(range, tsvGetter);
}

inline size_t MVD3File::getNbNeuron() const{
    if(_nb_neurons == 0){
        try{
            std::vector<size_t> dims = _hdf5_file.getDataSet(did_cells_positions).getSpace().getDimensions();
            if(dims.size() < 1){
                throw MVDParserException("Invalid Dataset dimension in MVD3 file");
            }
            (const_cast<size_t&>(_nb_neurons)) = dims[0]; // cache number of neurons in MVD3 file itself
        }catch(HighFive::Exception & e){
            std::ostringstream ss;
            ss << "Unable to parse "<< _filename << " " << e.what();
            throw MVDParserException(ss.str());
        }
    }
   return _nb_neurons;
}


inline Positions MVD3File::getPositions(const Range & range) const{
    Positions res;
    HighFive::DataSet set = _hdf5_file.getDataSet(did_cells_positions);
    if(!is_empty(range)){
        set.select({range.offset, 0}, {range.count, 3}).read(res);
        return res;
    }
    set.read(res);
    return res;
}


inline Rotations MVD3File::getRotations(const Range & range) const{
    Rotations res;
    HighFive::DataSet set = _hdf5_file.getDataSet(did_cells_rotations);
    if(!is_empty(range)){
        set.select({range.offset, 0}, {range.count, 4}).read(res);
        return res;
    }
    set.read(res);
    return res;
}


inline bool MVD3File::hasRotations() const {
    return _hdf5_file.exist(did_cells_rotations);
}


inline std::vector<std::string> MVD3File::getMorphologies(const Range & range) const {
    return get_resolve_field(did_lib_data_morpho, did_cells_index_morpho, range);
}


inline std::vector<std::string> MVD3File::getEtypes(const Range & range) const{
    const auto& getEtypesLambda = [=](std::vector<std::string>& me_combos, std::vector<std::string>& morphologies){return _tsv_file->getEtypes(me_combos, morphologies);};
    return getDataFromTsvOrMvd3WithIndex<std::string>(did_lib_data_etypes, did_cells_index_etypes, range, getEtypesLambda);
}


inline std::vector<std::string> MVD3File::getEmodels(const Range& range) const{
    const auto& getEmodelsLambda = [=](std::vector<std::string>& me_combos, std::vector<std::string>& morphologies){return _tsv_file->getEmodels(me_combos, morphologies);};
    return getDataFromTsv<std::string>("eModel", range, getEmodelsLambda);
}


inline std::vector<std::string> MVD3File::getMtypes(const Range& range) const{
    const auto& getMtypesLambda = [=](std::vector<std::string>& me_combos, std::vector<std::string>& morphologies){return _tsv_file->getMtypes(me_combos, morphologies);};
    return getDataFromTsvOrMvd3WithIndex<std::string>(did_lib_data_mtypes, did_cells_index_mtypes, range, getMtypesLambda);
}


inline std::vector<std::string> MVD3File::getMECombos(const Range& range) const {
    return get_resolve_field(did_lib_data_mecombo, did_cells_index_mecombo, range);
}


inline std::vector<std::string> MVD3File::getRegions(const Range & range) const{
    return get_resolve_field(did_lib_data_regions, did_cells_index_regions, range);
}


inline std::vector<boost::int32_t> MVD3File::getHyperColumns(const Range & range) const{
    HighFive::DataSet set = _hdf5_file.getDataSet(did_cells_hypercolumn);
    return get_data_for_selection<boost::int32_t>(set, range);
}


inline std::vector<boost::int32_t> MVD3File::getMiniColumns(const Range & range) const{
    HighFive::DataSet set = _hdf5_file.getDataSet(did_cells_minicolmun);
    return get_data_for_selection<boost::int32_t>(set, range);
}


inline std::vector<boost::int32_t> MVD3File::getLayers(const Range& range) const{
    const auto& getLayersLambda = [=](std::vector<std::string>& me_combos, std::vector<std::string>& morphologies){return _tsv_file->getLayers(me_combos, morphologies);};
    return getDataFromTsvOrMvd3WithoutIndex<boost::int32_t>(did_cells_layer, range, getLayersLambda);
}


inline bool MVD3File::hasCurrents() const  {
    return false;
}


inline std::vector<double> MVD3File::getThresholdCurrents(const Range& range) const{
    const auto& getThresholdCurrentsLamba = [=](std::vector<std::string>& me_combos, std::vector<std::string>& morphologies){return _tsv_file->getThresholdCurrents(me_combos, morphologies);};
    return getDataFromTsv<double>("Threshold Current", range, getThresholdCurrentsLamba);
}


inline std::vector<double> MVD3File::getHoldingCurrents(const Range& range) const{
    const auto& getHoldingCurrentsLamba = [=](std::vector<std::string>& me_combos, std::vector<std::string>& morphologies){return _tsv_file->getHoldingCurrents(me_combos, morphologies);};
    return getDataFromTsv<double>("Holding Current", range, getHoldingCurrentsLamba);
}


inline std::vector<std::string> MVD3File::getSynapseClass(const Range & range) const{
    return get_resolve_field(did_lib_data_syn_class, did_cells_index_synapse_class, range);
}


inline std::vector<TSV::TSVInfo> MVD3File::getTSVInfo(const Range& range) const {
    const auto& getTSVInfosLamba = [=](std::vector<std::string>& me_combos, std::vector<std::string>& morphologies){return _tsv_file->getTSVInfos(me_combos, morphologies);};
    return getDataFromTsv<TSV::TSVInfo>("", range, getTSVInfosLamba);
}


inline std::vector<size_t> MVD3File::getIndexMorphologies(const Range & range) const{
    HighFive::DataSet index = _hdf5_file.getDataSet(did_cells_index_morpho);
    return get_data_for_selection<size_t>(index, range);
}


inline std::vector<size_t> MVD3File::getIndexEtypes(const Range &range) const{
    HighFive::DataSet index = _hdf5_file.getDataSet(did_cells_index_etypes);
    return get_data_for_selection<size_t>(index, range);
}

inline std::vector<size_t> MVD3File::getIndexMtypes(const Range &range) const{
    HighFive::DataSet index = _hdf5_file.getDataSet(did_cells_index_mtypes);
    return get_data_for_selection<size_t>(index, range);
}

inline std::vector<size_t> MVD3File::getIndexRegions(const Range &range) const{
    HighFive::DataSet index = _hdf5_file.getDataSet(did_cells_index_regions);
    return get_data_for_selection<size_t>(index, range);
}

inline std::vector<size_t> MVD3File::getIndexSynapseClass(const Range &range) const{
    HighFive::DataSet index = _hdf5_file.getDataSet(did_cells_index_synapse_class);
    return get_data_for_selection<size_t>(index, range);
}


inline std::vector<std::string> MVD3File::listAllMorphologies() const{
    HighFive::DataSet index = _hdf5_file.getDataSet(did_lib_data_morpho);
    return get_data_for_selection<std::string>(index, Range(0,0));
}


inline std::vector<std::string> MVD3File::listAllEtypes() const{
    const auto& getEtypesLambda = [=](std::vector<std::string>& me_combos, std::vector<std::string>& morphologies){return _tsv_file->getEtypes(me_combos, morphologies);};
    auto allEtypes = getDataFromTsvOrMvd3WithIndex<std::string>(did_lib_data_etypes, did_cells_index_etypes, Range(0,0), getEtypesLambda);
    vector_remove_dups(allEtypes);
    return allEtypes;
}

inline std::vector<std::string> MVD3File::listAllMtypes() const{
    const auto& getMtypesLambda = [=](std::vector<std::string>& me_combos, std::vector<std::string>& morphologies){return _tsv_file->getMtypes(me_combos, morphologies);};
    auto allMtypes = getDataFromTsvOrMvd3WithIndex<std::string>(did_lib_data_mtypes, did_cells_index_mtypes, Range(0,0), getMtypesLambda);
    vector_remove_dups(allMtypes);
    return allMtypes;
}

inline std::vector<boost::int32_t> MVD3File::listAllLayers() const{
    const auto& getLayersLambda = [=](std::vector<std::string>& me_combos, std::vector<std::string>& morphologies){return _tsv_file->getLayers(me_combos, morphologies);};
    auto allLayers = getDataFromTsvOrMvd3WithoutIndex<boost::int32_t>(did_cells_layer, Range(0,0), getLayersLambda);
    vector_remove_dups(allLayers);
    return allLayers;
}

inline std::vector<std::string> MVD3File::listAllEmodels() const{
    const auto& getEmodelsLambda = [=](std::vector<std::string>& me_combos, std::vector<std::string>& morphologies){return _tsv_file->getEmodels(me_combos, morphologies);};
    auto allEmodels = getDataFromTsv<std::string>("eModel", Range(0,0), getEmodelsLambda);
    vector_remove_dups(allEmodels);
    return allEmodels;
}

inline std::vector<std::string> MVD3File::listAllRegions() const{
    HighFive::DataSet index = _hdf5_file.getDataSet(did_lib_data_regions);
    return get_data_for_selection<std::string>(index, Range(0,0));
}

inline std::vector<std::string> MVD3File::listAllSynapseClass() const{
    HighFive::DataSet index = _hdf5_file.getDataSet(did_lib_data_syn_class);
    return get_data_for_selection<std::string>(index, Range(0,0));
}

inline std::vector<double> MVD3File::getCircuitSeeds() const{
    std::vector<double> seeds;

    HighFive::DataSet seeds_dataset = _hdf5_file.getDataSet(did_lib_circuit_seeds);
    seeds_dataset.read(seeds);
    if(seeds.size() < 4){
        throw MVDParserException("Invalid MVD3 /circuit/seeds size, MVD3 should provide at least 4 seeds");
    }
    return seeds;
}

inline std::vector<std::string> MVD3File::get_resolve_field(const std::string& field,
                                                            const std::string& library,
                                                            const Range & range) const {
    HighFive::DataSet index = _hdf5_file.getDataSet(library);
    HighFive::DataSet data = _hdf5_file.getDataSet(field);
    return resolve_index<std::string>(index, range, data);
}

}  // namespace MVD3

#endif // MVD3_MISC_HPP
