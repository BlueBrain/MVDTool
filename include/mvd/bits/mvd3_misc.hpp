#ifndef MVD3_MISC_HPP
#define MVD3_MISC_HPP

#include <string>

#include "../mvd3.hpp"
#include "../mvd_except.hpp"

using namespace HighFive;

/*
template<typename T>
T resolve_index(DataSet & index, DataSet & data, size_t position){

}*/

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
            return dims[0];
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

std::vector<std::string> MVD3File::getMorphologies(){
    std::vector<std::string> res;
    return res;
}


}

#endif // MVD3_MISC_HPP
