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
#include <unordered_map>
#include <vector>

#include <boost/integer.hpp>


namespace MVD3 {

class MVD3File;  // fwd decl

}  // namespace MVD3


namespace TSV {

class TSVFile;  // Fwd decl


///
/// \brief The MEComboEntry class
///
/// Includes all the information of a mecombo entry of the tsv file
///
struct MEComboEntry {
    std::string morphologyName;
    std::string layer;
    std::string fullMType;
    std::string eType;
    std::string eModel;
    std::string comboName;
    double thresholdCurrent;
    double holdingCurrent;

    enum Column {
        MorphologyName,
        Layer,
        FullMType,
        EType,
        EModel,
        ComboName,
        ThresholdCurrent,
        HoldingCurrent
    };

    // operator << is a friend
    friend std::ostream& operator<<(std::ostream& os, MEComboEntry const& MEComboEntry);

  protected:
    friend class TSVFile;

    template <typename T>
    T get(const Column col_id) const;

};


///
/// \brief The TSVFile class
///
/// Represent a tsv me combo file. Reads the information from the mecombo_emodel.tsv file
/// and saves them in a vector of MEComboEntry objects.
///
class TSVFile {
  protected:
    friend class MVD3::MVD3File;
    template <typename T>
    std::vector<T> getField(const std::vector<std::string>& me_combos,
                            const std::vector<std::string>& morphologies,
                            const MEComboEntry::Column& col_id) const;

  public:
    ///
    /// \brief TSVFile
    /// \param filename tsv file name
    ///
    /// Open and read a tsv file format at 'filename' path
    /// throw TSVException, or HighFive::Exception in case of error
    ///
    TSVFile(const std::string& filename);

    ///
    /// \brief TSVFile
    /// \param filename tsv file name
    ///
    /// Open and read a tsv file format at 'filename' path
    /// throw TSVException, or HighFive::Exception in case of error
    ///
    TSVFile(const std::string& filename, const MEComboEntry::Column& column);

    using vector_ref = std::vector<std::reference_wrapper<const MEComboEntry>>;

    ///
    /// \brief TSVFile
    ///
    /// Get all the me types defined in the tsv file
    ///
    vector_ref getAll() const;

    ///
    /// \brief TSVFile
    /// \param me_combo me_combo strings of the neurons me types
    /// \param morphology morphology names of the neurons me types
    ///
    /// Get the info included in the tsv file for the neuron types
    /// with the me_combos and morphology names requested
    /// throw TSVException, if me_combo or morphology don't match
    /// an entry in tsvFileInfo
    ///
    vector_ref get(const std::vector<std::string>& me_combos,
                   const std::vector<std::string>& morphologies) const;

    ///
    /// \brief TSVFile
    /// \param me_combo me_combo strings of the neurons me types
    /// \param morphology morphology names of the neurons me types
    ///
    /// Get the info included in the tsv file for the neuron types
    /// with the me_combos and morphology names requested
    /// throw TSVException, if me_combo or morphology don't match
    /// an entry in tsvFileInfo
    ///
    inline std::vector<std::string> getLayers(const std::vector<std::string>& me_combos,
                                              const std::vector<std::string>& morphologies) const {
        return getField<std::string>(me_combos, morphologies, MEComboEntry::Layer);
    }

    ///
    /// \brief TSVFile
    /// \param me_combo me_combo strings of the neurons me types
    /// \param morphology morphology names of the neurons me types
    ///
    /// Get the info included in the tsv file for the neuron types
    /// with the me_combos and morphology names requested
    /// throw TSVException, if me_combo or morphology don't match
    /// an entry in tsvFileInfo
    ///
    inline std::vector<std::string> getMtypes(const std::vector<std::string>& me_combos,
                                       const std::vector<std::string>& morphologies) const {
        return getField<std::string>(me_combos, morphologies, MEComboEntry::FullMType);
    }

    ///
    /// \brief TSVFile
    /// \param me_combo me_combo strings of the neurons me types
    /// \param morphology morphology names of the neurons me types
    ///
    /// Get the info included in the tsv file for the neuron types
    /// with the me_combos and morphology names requested
    /// throw TSVException, if me_combo or morphology don't match
    /// an entry in tsvFileInfo
    ///
    inline std::vector<std::string> getEtypes(const std::vector<std::string>& me_combos,
                                       const std::vector<std::string>& morphologies) const {
        return getField<std::string>(me_combos, morphologies, MEComboEntry::EType);
    }

    ///
    /// \brief TSVFile
    /// \param me_combo me_combo strings of the neurons me types
    /// \param morphology morphology names of the neurons me types
    ///
    /// Get the info included in the tsv file for the neuron types
    /// with the me_combos and morphology names requested
    /// throw TSVException, if me_combo or morphology don't match
    /// an entry in tsvFileInfo
    ///
    inline std::vector<std::string> getEmodels(const std::vector<std::string>& me_combos,
                                        const std::vector<std::string>& morphologies) const {
        return getField<std::string>(me_combos, morphologies, MEComboEntry::EModel);
    }

    ///
    /// \brief TSVFile
    /// \param me_combo me_combo strings of the neurons me types
    /// \param morphology morphology names of the neurons me types
    ///
    /// Get the info included in the tsv file for the neuron types
    /// with the me_combos and morphology names requested
    /// throw TSVException, if me_combo or morphology don't match
    /// an entry in tsvFileInfo
    ///
    inline std::vector<double> getThresholdCurrents(const std::vector<std::string>& me_combos,
                                             const std::vector<std::string>& morphologies) const {
        return getField<double>(me_combos, morphologies, MEComboEntry::ThresholdCurrent);
    }

    ///
    /// \brief TSVFile
    /// \param me_combo me_combo strings of the neurons me types
    /// \param morphology morphology names of the neurons me types
    ///
    /// Get the info included in the tsv file for the neuron types
    /// with the me_combos and morphology names requested
    /// throw TSVException, if me_combo or morphology don't match
    /// an entry in tsvFileInfo
    ///
    inline std::vector<double> getHoldingCurrents(const std::vector<std::string>& me_combos,
                                           const std::vector<std::string>& morphologies) const {
        return getField<double>(me_combos, morphologies, MEComboEntry::HoldingCurrent);
    }


    struct pair_hash {
        template <class T1, class T2>
        inline std::size_t operator()(const std::pair<T1, T2>& pair) const {
            return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
        }
    };
    using unordered_pair_map =
        std::unordered_map<std::pair<std::string, std::string>, MEComboEntry, pair_hash>;


  private:
    std::string _filename;
    const unordered_pair_map tsvFileInfo;

};

}  // namespace TSV

#include "bits/tsv_misc.hpp"
