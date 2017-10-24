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

namespace{

inline bool is_valid_range(const MVD3::Range & range){
    return (range.count !=0);
}

inline std::vector<size_t> size_to_vec(size_t s){
    return std::vector<size_t>(1, s);
}

inline std::vector<size_t> size2D_to_vec(size_t d1, size_t d2){
    std::vector<size_t> res;
    res.push_back(d1);
    res.push_back(d2);
    return res;
}

template<typename T>
inline std::vector<T> get_data_for_selection(HighFive::DataSet & dataset, const MVD3::Range & range){
    std::vector<T> data_values;

    if(is_valid_range(range)){
        dataset.select(size_to_vec(range.offset), size_to_vec(range.count))
                .read(data_values);
    }else{
        dataset.read(data_values);
    }
    return data_values;
}

template<typename T>
inline std::vector<T> resolve_index(HighFive::DataSet & index, const MVD3::Range & range, HighFive::DataSet & data){
    std::vector<T> full_data, result;
    std::vector<size_t> references = get_data_for_selection<size_t>(index, range);

    const size_t n_elem = data.getSpace().getDimensions()[0];

    data.read(full_data);
    result.reserve(references.size());

    for(std::vector<size_t>::iterator it = references.begin(); it < references.end(); ++it){
        if(*it >= n_elem){
            std::ostringstream ss;
            ss << "Invalid index reference " << *it << " in an dataset of size " << n_elem;
            throw MVDParserException(ss.str());
        }
        result.push_back(full_data[*it]);
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
const std::string did_cells_index_synapse_class = "/cells/properties/synapse_class";
// data
const std::string did_lib_data_morpho = "/library/morphology";
const std::string did_lib_data_etypes = "/library/etype";
const std::string did_lib_data_mtypes = "/library/mtype";
const std::string did_lib_data_syn_class = "/library/synapse_class";
// circuit
const std::string did_lib_circuit_seeds = "/circuit/seeds";
}

namespace MVD3 {

inline MVD3File::MVD3File(const std::string &str) : _filename(str), _hdf5_file(str), _nb_neurons(0)
{ }

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
    if(is_valid_range(range)){
                set.select(size2D_to_vec(range.offset, 0), size2D_to_vec(range.count, 3))
                .read(res);
        return res;
    }
    set.read(res);
    return res;
}


inline Rotations MVD3File::getRotations(const Range & range) const{
    Rotations res;
    HighFive::DataSet set = _hdf5_file.getDataSet(did_cells_rotations);
    if(is_valid_range(range)){
            set.select(size2D_to_vec(range.offset, 0), size2D_to_vec(range.count, 4))
            .read(res);
        return res;
    }
    set.read(res);
    return res;
}


inline std::vector<std::string> MVD3File::getMorphologies(const Range & range) const{
    HighFive::DataSet index = _hdf5_file.getDataSet(did_cells_index_morpho);
    HighFive::DataSet data = _hdf5_file.getDataSet(did_lib_data_morpho);
    return resolve_index<std::string>(index, range, data);
}




inline std::vector<std::string> MVD3File::getEtypes(const Range & range) const{
    HighFive::DataSet index = _hdf5_file.getDataSet(did_cells_index_etypes);
    HighFive::DataSet data = _hdf5_file.getDataSet(did_lib_data_etypes);
    return resolve_index<std::string>(index, range, data);
}


inline std::vector<std::string> MVD3File::getMtypes(const Range & range) const{
    HighFive::DataSet index = _hdf5_file.getDataSet(did_cells_index_mtypes);
    HighFive::DataSet data = _hdf5_file.getDataSet(did_lib_data_mtypes);
    return resolve_index<std::string>(index, range, data);
}

inline std::vector<boost::int32_t> MVD3File::getHyperColumns(const Range & range) const{
    HighFive::DataSet set = _hdf5_file.getDataSet(did_cells_hypercolumn);
    return get_data_for_selection<boost::int32_t>(set, range);
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
    HighFive::DataSet index = _hdf5_file.getDataSet(did_cells_index_synapse_class);
    HighFive::DataSet data = _hdf5_file.getDataSet(did_lib_data_syn_class);
    return resolve_index<std::string>(index, range, data);
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



}

#endif // MVD3_MISC_HPP
