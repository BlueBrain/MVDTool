#ifndef MVD3_MISC_HPP
#define MVD3_MISC_HPP

#include <string>

#include "../mvd3.hpp"
#include "../mvd_except.hpp"

namespace MVD3 {

MVD3File::MVD3File(const std::string &str) : _filename(str), _hdf5_file(str) {}

size_t MVD3File::getNbNeuron(){
    try{
        std::vector<size_t> dims = _hdf5_file.getDataSet("/cells/positions").getSpace().getDimensions();
        if(dims.size() < 1){
            throw MVDParserException("Invalid Dataset dimension in MVD3 file");
        }
        return dims[0];
    }catch(HighFive::Exception & e){
        std::ostringstream ss;
        ss << "Unable to parse "<< _filename << " " << e.what();
        throw MVDParserException(ss.str());
    }
}


}

#endif // MVD3_MISC_HPP
