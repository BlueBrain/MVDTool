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
#ifndef MVD3_HPP
#define MVD3_HPP

#ifndef H5_USE_BOOST
#define H5_USE_BOOST
#endif

#include <string>

#include <boost/integer.hpp>
#include <highfive/H5File.hpp>

#include "mvd_base.hpp"

namespace MVD3 {


typedef MVD::Positions Positions;
typedef MVD::Rotations Rotations;
typedef MVD::Range Range;


///
/// \brief The MVD3File class
///
/// Represent a MVD 3.0 circuit file
///
class MVD3File : public MVD::MVDFile{
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
    size_t getNbNeuron() const;

    ///
    /// \brief getPositions
    /// \param range: selection range, the default range (0,0) select the entire dataset
    /// \return a double vector of size [N][3] with the position (x,y,z) coordinates
    ///  of each selected neurons ( all by default )
    ///
    Positions getPositions(const Range & range = Range(0,0)) const;

    ///
    /// \brief getPositions
    /// \param range: selection range, a null range (0,0) select the entire dataset
    /// \return a double vector of size [N][4] with the rotation quaternions in the order (x,y,z,w)
    /// of each selected neurons ( all by default )
    ///
    Rotations getRotations(const Range & range = Range(0,0)) const;




    ///
    /// \brief getMorphologies
    /// \param range: selection range, a null range (0,0) select the entire dataset
    /// \return vector of string with the morphology name associated with each neuron
    ///
    std::vector<std::string> getMorphologies(const Range & range = Range(0,0)) const;

    ///
    /// \brief getEtypes
    /// \param range: selection range, a null range (0,0) select the entire dataset
    /// \return vector of string with the eEtype name associated with each neuron
    ///
    std::vector<std::string> getEtypes(const Range & range = Range(0,0)) const;

    ///
    /// \brief getMtypes
    /// \return vector of string with the Mtype name associated with each neuron
    ///
    std::vector<std::string> getMtypes(const Range & range = Range(0,0)) const;

    ///
    /// \brief getRegions
    /// \return vector of string with the region name associated with each neuron
    ///
    std::vector<std::string> getRegions(const Range & range = Range(0,0)) const;

    ///
    /// \brief getHyperColumns
    /// \return vector of int32 with the hyper-column associated with each neuron
    ///
    std::vector<boost::int32_t> getHyperColumns(const Range & range = Range(0,0)) const;

    ///
    /// \brief getMiniColumns
    /// \return vector of int32 with the mini-column associated with each neuron
    ///
    std::vector<boost::int32_t> getMiniColumns(const Range & range = Range(0,0)) const;

    ///
    /// \brief getLayer
    /// \return vector of int32 with the layer associated with each neuron
    ///
    std::vector<boost::int32_t> getLayers(const Range & range = Range(0,0)) const;


    ///
    /// \brief getSynapseClass
    /// \return vector of string with the synapse type associated with each neuron
    ///
    std::vector<std::string> getSynapseClass(const Range & range = Range(0,0)) const;


    // index related infos

    ///
    /// \brief getIndexMorphologies
    /// \param range
    /// \return values of the morphology index for neurons in the range, default: entire dataset
    ///
    std::vector<size_t> getIndexMorphologies(const Range & range = Range(0,0)) const;



    ///
    /// \brief getIndexEtypes
    /// \param range
    /// \return values of the Etypes index for neurons in the range, default: entire dataset
    ///
    std::vector<size_t> getIndexEtypes(const Range & range = Range(0,0)) const;


    ///
    /// \brief getIndexMtypes
    /// \param range
    /// \return values of the Mtypes index for neurons in the range, default: entire dataset
    ///
    std::vector<size_t> getIndexMtypes(const Range & range = Range(0,0)) const;

    ///
    /// \brief getIndexSynapseClass
    /// \param range
    /// \return values of the Synapse class index for neurons in the range, default: entire dataset
    ///
    std::vector<size_t> getIndexSynapseClass(const Range & range = Range(0,0)) const;


    ///
    /// \brief getIndexRegions
    /// \param range
    /// \return values of the Regions index for neurons in the range, default: entire dataset
    ///
    std::vector<size_t> getIndexRegions(const Range & range = Range(0,0)) const;

    // Data related infos

    ///
    /// \brief getDataMorphologies
    /// \return vector of all unique morphologies ( mvd3 /library section )
    ///
    std::vector<std::string> listAllMorphologies() const;

    ///
    /// \brief listAllEtypes
    /// \return vector of all unique Etypes ( mvd3 /library section )
    ///
    std::vector<std::string> listAllEtypes() const;

    ///
    /// \brief listAllMtypes
    /// \return vector of all unique Mtypes ( mvd3 /library section )
    ///
    std::vector<std::string> listAllMtypes() const;

    ///
    /// \brief listAllRegions
    /// \return vector of all unique Regions ( mvd3 /library section )
    ///
    std::vector<std::string> listAllRegions() const;

    ///
    /// \brief listSynapseClass
    /// \return vector of all unique Synapse Class ( mvd3 /library section )
    ///
    std::vector<std::string> listAllSynapseClass() const;

    // circuit infos
    ///
    /// \brief getCircuitSeeds
    /// \return return a vector of the circuit seeds to use for random initialization
    /// the vector has to be at least 4 elements
    ///
    std::vector<double> getCircuitSeeds() const;

private:
    std::string _filename;
    HighFive::File _hdf5_file;
    size_t _nb_neurons;

};

}

#include "bits/mvd3_misc.hpp"

#endif // MVD3_HPP
