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

#include <boost/range/combine.hpp>
#include <highfive/H5DataSet.hpp>

#include "../mvd3.hpp"
#include "../mvd_except.hpp"

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
    _tsv_file = std::unique_ptr<TSV::TSVFile>(new TSV::TSVFile(filename));
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
    return get_resolve_field(did_lib_data_etypes, did_cells_index_etypes, range);
}

inline std::vector<std::string> MVD3File::getEmodels(const Range&) const {
    throw MVDException("Emodel not available in mvd3");
}

inline std::vector<std::string> MVD3File::getMtypes(const Range & range) const{
    return get_resolve_field(did_lib_data_mtypes, did_cells_index_mtypes, range);
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

inline bool MVD3File::hasCurrents() const  {
    return false;
}

inline std::vector<double> MVD3File::getThresholdCurrents(const Range&) const {
    throw MVDException("ThresholdCurrents not available in mvd3");
}

inline std::vector<double> MVD3File::getHoldingCurrents(const Range&) const {
    throw MVDException("HoldingCurrents not available in mvd3");
}


inline std::vector<boost::int32_t> MVD3File::getMiniColumns(const Range & range) const{
    HighFive::DataSet set = _hdf5_file.getDataSet(did_cells_minicolmun);
    return get_data_for_selection<boost::int32_t>(set, range);
}

inline std::vector<boost::int32_t> MVD3File::getLayers(const Range & range) const{
    HighFive::DataSet set = _hdf5_file.getDataSet(did_cells_layer);
    return get_data_for_selection<boost::int32_t>(set, range);
}


inline std::vector<std::string> MVD3File::getSynapseClass(const Range & range) const{
    return get_resolve_field(did_lib_data_syn_class, did_cells_index_synapse_class, range);
}


inline std::vector<TSV::TSVInfo> MVD3File::getTSVInfo(const Range& range) const {
    if (_tsv_file == nullptr) {
        std::ostringstream ss;
        ss << "No tsv file is opened with MVD3 file " << _filename << " to extract information from"
           << std::endl;
        throw MVDException(ss.str());
    }
    std::vector<std::string> etypes = getEtypes(range);
    std::vector<boost::int32_t> layers = getLayers(range);
    std::vector<std::string> morphologies = getMorphologies(range);
    std::vector<TSV::TSVInfo> tsvInfos;
    for (const auto& mecombo_tupple: boost::combine(etypes, layers, morphologies)) {
        tsvInfos.push_back(_tsv_file->getTSVInfo(boost::get<0>(mecombo_tupple) + "_" +
                                                     std::to_string(boost::get<1>(mecombo_tupple)) +
                                                     "_" + boost::get<2>(mecombo_tupple),
                                                 boost::get<2>(mecombo_tupple)));
    }
    return tsvInfos;
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
    HighFive::DataSet index = _hdf5_file.getDataSet(did_lib_data_etypes);
    return get_data_for_selection<std::string>(index, Range(0,0));
}

inline std::vector<std::string> MVD3File::listAllMtypes() const{
    HighFive::DataSet index = _hdf5_file.getDataSet(did_lib_data_mtypes);
    return get_data_for_selection<std::string>(index, Range(0,0));
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
