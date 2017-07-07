/*
 * Copyright (C) 2015 Adrien Devresse <adrien.devresse@epfl.ch>
 *               2017 Fernando Pereira <fernando.pereira@epfl.ch>
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
#ifndef MVD2_HPP
#define MVD2_HPP

/**
 * @file mvd2.hpp
 *
 * mvd2 file parser
 *
 * */

#include <vector>
#include <set>
#include <string>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include "mvd_base.hpp"


///
/// MVD2 parsing / helper functions
///
namespace MVD2{

///
/// \brief data type in the current MVD2 line
///
enum DataSet{
    None = 0,
    NeuronLoaded = 1,
    MicroBoxData = 2,
    MiniColumnsPosition = 3,
    CircuitSeeds = 4,
    MorphTypes = 5,
    ElectroTypes = 6
};


///
/// \brief The Counter struct
/// Hold the counts of the different types
///
struct Counter {
    Counter();
    int operator()(DataSet type, const char* line);

    size_t _nb_columns;
    size_t _nb_neuron;
    size_t _nb_morpho_type;
    std::set<std::string> morphos;
};


class MVD2File : public MVD::MVDFile{
public:
    inline MVD2File(const std::string & filename) :
        _filename(filename)
    {    }

    ///
    /// \brief getNbMorpho
    /// \return number of morphologies in MVD file
    size_t getNbMorphoType() const;

    ///
    size_t getNbMorpho() const;

    ///
    /// \brief getNbNeuron
    /// \return number of neurons in this MVD file
    ///
    size_t getNbNeuron() const;

    ///
    /// \brief getNbColumns
    /// \return number of columns in this MVD file
    ///
    size_t getNbColumns() const;


    std::set<std::string> & getUniqueMorphologies() const;


    /// \brief getPositions
    /// \param range: selection range, the default range (0,0) select the entire dataset
    /// \return a double vector of size [N][3] with the position (x,y,z) coordinates
    ///  of each selected neurons ( all by default )
    ///
    MVD::Positions getPositions(const MVD::Range & range = MVD::Range(0,0)) const;


    ///
    /// \brief Rotations
    /// \return a double vector of size N with the rotations
    /// of each selected neurons ( all by default )
    ///
    MVD::Rotations getRotations(const MVD::Range & range = MVD::Range(0,0)) const;



    template <typename Callback>
    void parse(Callback & line_parser) const;


private:
    std::string _filename;

    // Counter is a kind of cache for the totals
    // We set counter to mutable to allow all reader methods to be const
    // It can be safely reconstructed without affecting bahvior, only performance
    mutable Counter counter;
    void init_counter() const;

};


//
// parsing functions for use in callback
//

/// parse a line identified as a neuron information line (NeuronLoaded)
///  The parsing is done in float simple precision for position and rotation
///
void parseNeuronLine(const char* line, std::string & name, int& database, int &column, int &minicolumn, int &layer,
                     int &morphologytype, int &electrophysiology_type,
                     std::vector<float> & xyzr, std::string & metype);

/// parse a line identified as random seed initializer (CircuitSeeds)
void parseSeedInitLine(const char *line, double &seed1, double &seed2, double &seed3);

/// parse a line identified as en cell electrical type (ElectroTypes)
void parseElectroTypeLine(const char *line, std::string &electroType);

/// parse a line identified as morph type (MorphType)
void parseMorphTypeLine(const char* line, std::string & name, std::string & name2, std::string & morphClass);

}


#include "bits/mvd2_misc.hpp"

#endif // MVD2_HPP
