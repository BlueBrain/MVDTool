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

    ///
    /// \brief MVD3File
    /// \param filename
    ///
    /// Open an MVD3 file format at 'filename' path
    /// throw MVDException, or HighFive::Exception in case of error
    ///
    MVD3File(const std::string & filename);


    ///
    /// \brief getNbNeuron
    /// \return total number of neurons contained in the receipe
    ///
    size_t getNbNeuron();

    ///
    /// \brief getPositions
    /// \return a double vector of size [N][3] with the position of each neuron
    ///
    ///
    Positions getPositions();


    ///
    /// \brief getMorphologies
    /// \return vector of string with the name of each morphology associated with each neuron
    ///
    std::vector<std::string> getMorphologies();


private:
    std::string _filename;
    HighFive::File _hdf5_file;
    size_t _nb_neurons;

};

}

#include "bits/mvd3_misc.hpp"

#endif // MVD3_HPP

