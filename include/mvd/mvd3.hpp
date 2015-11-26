/*
 * @file mvd3.hpp
 * @brief mvd3 file format parser for neuron circuits
 *
 */

#ifndef MVD3_HPP
#define MVD3_HPP

#ifndef H5_USE_BOOST
#define H5_USE_BOOST
#endif

#include <string>
#include <highfive/H5File.hpp>
#include <boost/multi_array.hpp>

namespace MVD3 {


typedef boost::multi_array<double, 2> Positions;

class MVD3File{
public:


    MVD3File(const std::string & str);


    size_t getNbNeuron();


    Positions getPositions();


    std::vector<std::string> getMorphologies();


private:
    std::string _filename;
    HighFive::File _hdf5_file;
    size_t _nb_neurons;

};

}

#include "bits/mvd3_misc.hpp"

#endif // MVD3_HPP

