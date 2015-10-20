#ifndef MVD2_HPP
#define MVD2_HPP

/**
 * @file mvd2.hpp
 *
 * mvd2 file parser
 *
 * */


#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <cstdlib>


///
/// MVD2 parsing / helper functions
///
namespace MVD2{

class MVD2File{
public:
    MVD2File(const std::string & filename) :
        _filename(filename),
        _nb_neuron(0),
        _nb_morpho(0){

    }

    size_t getNbMorpho();

    size_t getNbNeuron();

private:
    std::string _filename;
    size_t _nb_neuron;
    size_t _nb_morpho;

    void init_counter();
};

}


#include "bits/mvd2_misc.hpp"

#endif // MVD2_HPP
