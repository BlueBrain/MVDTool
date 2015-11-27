#ifndef MVD3_MISC_HPP
#define MVD3_MISC_HPP

#include <string>
#include <vector>

#include <highfive/H5DataSet.hpp>

#include "../mvd3.hpp"
#include "../mvd_except.hpp"

using namespace HighFive;


template<typename T>
std::vector<T> resolve_index(DataSet & index, DataSet & data){
    std::vector<size_t> references;
    std::vector<T> full_data, result;

    index.read(references);
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

namespace MVD3 {

MVD3File::MVD3File(const std::string &str) : _filename(str), _hdf5_file(str), _nb_neurons(0) {}

size_t MVD3File::getNbNeuron(){
    if(_nb_neurons == 0){
        try{
            std::vector<size_t> dims = _hdf5_file.getDataSet("/cells/positions").getSpace().getDimensions();
            if(dims.size() < 1){
                throw MVDParserException("Invalid Dataset dimension in MVD3 file");
            }
            _nb_neurons = dims[0];
        }catch(HighFive::Exception & e){
            std::ostringstream ss;
            ss << "Unable to parse "<< _filename << " " << e.what();
            throw MVDParserException(ss.str());
        }
    }
   return _nb_neurons;
}

Positions MVD3File::getPositions(){
    Positions res;
    _hdf5_file.getDataSet("/cells/positions").read(res);
    return res;
}


Rotations MVD3File::getRotations(){
    Rotations res;
    _hdf5_file.getDataSet("/cells/orientations").read(res);
    return res;
}

std::vector<std::string> MVD3File::getMorphologies(){
    DataSet index = _hdf5_file.getDataSet("/cells/properties/morphology");
    DataSet data = _hdf5_file.getDataSet("/library/morphology");
    return resolve_index<std::string>(index, data);
}


std::vector<std::string> MVD3File::getEtypes(){
    DataSet index = _hdf5_file.getDataSet("/cells/properties/etype");
    DataSet data = _hdf5_file.getDataSet("/library/etype");
    return resolve_index<std::string>(index, data);
}


std::vector<std::string> MVD3File::getMtypes(){
    DataSet index = _hdf5_file.getDataSet("/cells/properties/mtype");
    DataSet data = _hdf5_file.getDataSet("/library/mtype");
    return resolve_index<std::string>(index, data);
}

std::vector<std::string> MVD3File::getSynapseClass(){
    DataSet index = _hdf5_file.getDataSet("/cells/properties/synapse_class");
    DataSet data = _hdf5_file.getDataSet("/library/synapse_class");
    return resolve_index<std::string>(index, data);
}



}

#endif // MVD3_MISC_HPP
