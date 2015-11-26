/*
 * @file mvd3.hpp
 * @brief mvd3 file format parser for neuron circuits
 *
 */

#ifndef MVD3_HPP
#define MVD3_HPP

#include <string>
#include <highfive/H5File.hpp>

namespace MVD3 {

class MVD3File{
public:
    MVD3File(const std::string & str);


    size_t getNbNeuron();


private:
    std::string _filename;
    HighFive::File _hdf5_file;

};

}

#include "bits/mvd3_misc.hpp"

#endif // MVD3_HPP

