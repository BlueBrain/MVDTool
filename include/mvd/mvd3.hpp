/*
 * @file mvd3.hpp
 * @brief mvd3 file format parser for neuron circuits
 *
 */

#ifndef MVD3_HPP
#define MVD3_HPP

#include <string>

namespace MVD3 {

class MVD3File{
public:
    MVD3File(const std::string & str) : _filename(str){ }


    size_t getNbNeuron();


private:
    std::string _filename;
};

}

#endif // MVD3_HPP

