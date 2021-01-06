/*
 * Copyright (C) 2019, Blue Brain Project, EPFL.
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
#pragma once

#ifndef H5_USE_BOOST
#define H5_USE_BOOST
#endif

#include <string>
#include <functional>

#include <highfive/H5File.hpp>

#include <bbp/sonata/nodes.h>

#include "mvd_base.hpp"

namespace MVD {


typedef MVD::Positions Positions;
typedef MVD::Rotations Rotations;
typedef MVD::Range Range;


///
/// \brief The File class
///
/// Represent a Sonata circuit file
///
class SonataFile : public MVD::File {
public:

    ///
    /// \brief MVD3File
    /// \param filename
    ///
    /// Open an MVD3 file format at 'filename' path
    /// throw MVDException, or HighFive::Exception in case of error
    ///
    SonataFile(const std::string& filename, const std::string& pop_name = "");


    ///
    /// \brief openComboTsv
    /// \param filename
    ///
    /// Open an TSV file format at 'filename' path
    /// throw TSVException in case of error
    ///
    void openComboTsv(const std::string& filename) override;

    ///
    /// \brief getNbNeuron
    /// \return total number of neurons contained in the receipe
    ///
    size_t getNbNeuron() const noexcept override { return size_; }

    ///
    /// \brief getPositions
    /// \param range: selection range, the default range (0,0) select the entire dataset
    /// \return a double vector of size [N][3] with the position (x,y,z) coordinates
    ///  of each selected neurons ( all by default )
    ///
    Positions getPositions(const Range & range = Range::all()) const override;

    ///
    /// \brief getRotations
    /// \param range: selection range, a null range (0,0) select the entire dataset
    /// \return a double vector of size [N][4] with the rotation quaternions in the order (x,y,z,w)
    /// of each selected neurons ( all by default )
    ///
    Rotations getRotations(const Range & range = Range::all()) const override;

    ///
    /// \brief getQuaternionRotations
    /// \param range: selection range, a null range (0,0) select the entire dataset
    /// \return a double vector of size [N][4] with the rotation quaternions in the order (x,y,z,w)
    /// of each selected neurons ( all by default )
    ///
    Rotations getQuaternionRotations(const Range & range = Range::all()) const;

    ///
    /// \brief getAngularRotations
    /// \param range: selection range, a null range (0,0) select the entire dataset
    /// \return a double vector of size [N][4] with the rotation quaternions in the order (x,y,z,w)
    /// of each selected neurons ( all by default )
    ///
    Rotations getAngularRotations(const Range & range = Range::all()) const;

    ///
    /// \brief hasRotations
    /// \return if the current file has a rotational dataset
    ///
    bool hasRotations() const override;

    ///
    /// \brief getMorphologies
    /// \param range: selection range, a null range (0,0) select the entire dataset
    /// \return vector of string with the morphology name associated with each neuron
    ///
    std::vector<std::string> getMorphologies(const Range & range = Range::all()) const override;

    ///
    /// \brief getEtypes
    /// \param range: selection range, a null range (0,0) select the entire dataset
    /// \return vector of string with the eEtype name associated with each neuron
    ///
    std::vector<std::string> getEtypes(const Range & range = Range::all()) const override;

    ///
    /// \brief getMtypes
    /// \return vector of string with the Mtype name associated with each neuron
    ///
    std::vector<std::string> getMtypes(const Range & range = Range::all()) const override;

    ///
    /// \brief getEmodels
    /// \return vector of string with the Emodel name associated with each neuron
    ///
    std::vector<std::string> getEmodels(const Range& range = Range::all()) const override;

    ///
    /// \brief getLayers
    /// \param range: selection range, a null range (0,0) select the entire dataset
    /// \return vector of string with the Layer associated with each neuron
    ///
    std::vector<std::string> getLayers(const Range & range = Range::all()) const;

    ///
    /// \brief Checks whether exc_mini_frequency and inh_mini_frequency are available
    /// \return bool whether the two mini frequency properties are available
    ///
    bool hasMiniFrequencies() const override;

    ///
    /// \brief getExcMiniFrequencies
    /// \return vector of double with the excitatory mini frequencies
    ///
    std::vector<double> getExcMiniFrequencies(const Range & range = Range::all()) const override;

    ///
    /// \brief getInhMiniFrequencies
    /// \return vector of double with the inhibitory mini frequencies
    ///
    std::vector<double> getInhMiniFrequencies(const Range & range = Range::all()) const override;

    ///
    /// \brief getRegions
    /// \return vector of string with the region name associated with each neuron
    ///
    std::vector<std::string> getRegions(const Range & range = Range::all()) const override;

    ///
    /// \brief getSynapseClass
    /// \return vector of string with the synapse type associated with each neuron
    ///
    std::vector<std::string> getSynapseClass(const Range & range = Range::all()) const override;

    ///
    /// \brief Checks whether these two extra fields are available
    /// \return bool whether the two current properties are available
    ///
    bool hasCurrents() const override;

    ///
    /// \brief Retrieves the threshold currents
    /// \return a vector<double> with all the neurons threshold currents
    ///
    std::vector<double> getThresholdCurrents(const Range& range = Range::all()) const override;

    ///
    /// \brief Retrieves the holding currents
    /// \return a vector<double> with all the neurons holding currents
    ///
    std::vector<double> getHoldingCurrents(const Range& range = Range::all()) const override;

    // index related infos

    ///
    /// \brief getIndexEtypes
    /// \param range
    /// \return values of the Etypes index for neurons in the range, default: entire dataset
    ///
    std::vector<size_t> getIndexEtypes(const Range & range = Range::all()) const override;


    ///
    /// \brief getIndexMtypes
    /// \param range
    /// \return values of the Mtypes index for neurons in the range, default: entire dataset
    ///
    std::vector<size_t> getIndexMtypes(const Range & range = Range::all()) const override;

    ///
    /// \brief getIndexSynapseClass
    /// \param range
    /// \return values of the Synapse class index for neurons in the range, default: entire dataset
    ///
    std::vector<size_t> getIndexSynapseClass(const Range & range = Range::all()) const override;


    ///
    /// \brief getIndexRegions
    /// \param range
    /// \return values of the Regions index for neurons in the range, default: entire dataset
    ///
    std::vector<size_t> getIndexRegions(const Range & range = Range::all()) const override;

    // Data related infos

    ///
    /// \brief listAllEtypes
    /// \return vector of all unique Etypes ( mvd3 /library section )
    ///
    std::vector<std::string> listAllEtypes() const override;

    ///
    /// \brief listAllMtypes
    /// \return vector of all unique Mtypes ( mvd3 /library section )
    ///
    std::vector<std::string> listAllMtypes() const override;

    ///
    /// \brief listAllLayers
    /// \return vector of all unique Layers ( mvd3 /library section )
    ///
    std::vector<std::string> listAllLayers() const;

    ///
    /// \brief listAllEmodels
    /// \return vector of all unique Emodels ( mvd3 /library section )
    ///
    std::vector<std::string> listAllEmodels() const override;

    ///
    /// \brief listAllRegions
    /// \return vector of all unique Regions ( mvd3 /library section )
    ///
    std::vector<std::string> listAllRegions() const override;

    ///
    /// \brief listSynapseClass
    /// \return vector of all unique Synapse Class ( mvd3 /library section )
    ///
    std::vector<std::string> listAllSynapseClass() const override;

    ///
    /// \brief hasAttribute
    /// \return bool whether the queried attribute exists
    ///
    bool hasAttribute(const std::string& name) const;

    ///
    /// \brief hasDynamicsAttribute
    /// \return bool whether the queried dynamics attribute exists
    ///
    bool hasDynamicsAttribute(const std::string& name) const;

    ///
    /// \brief getAttributeDataType
    /// \return string the data type of the queried attribute
    ///
    std::string getAttributeDataType(const std::string& name) const;

    ///
    /// \brief getAttribute
    /// \return vector of the queried attribute data
    ///
    template <typename T>
    std::vector<T> getAttribute(const std::string& name, const Range& range = Range::all()) const;

private:
    std::unique_ptr<bbp::sonata::NodePopulation> pop_;
    size_t size_;

};

}

#include "bits/sonata_misc.hpp"
