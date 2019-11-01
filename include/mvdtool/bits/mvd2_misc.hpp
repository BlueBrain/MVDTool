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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <boost/algorithm/string.hpp>

#include "../mvd2.hpp"
#include "../mvd_except.hpp"


namespace MVD2{



/// parse MVD2 file datatype section
inline DataSet getDataType(const char* line, const DataSet & prev_datatype){
    if (line[0] == '#') {
        return prev_datatype;
    } else if (strncmp("Neurons Loaded", line + 0, 14) == 0) {
        return NeuronLoaded;
    } else if (strncmp("MicroBox Data", line + 0, 13) == 0) {
        return MicroBoxData;
    } else if (strncmp("MiniColumnsPosition", line + 0, 19) == 0) {
        return MiniColumnsPosition;
    } else if (strncmp("CircuitSeeds", line + 0, 12) == 0) {
        return CircuitSeeds;
    } else if (strncmp("MorphTypes", line + 0, 10) == 0) {
        return MorphTypes;
    } else if (strncmp("ElectroTypes", line + 0, 12) == 0) {
        return ElectroTypes;
    }
    return prev_datatype;
}

/// SAX style parser for MVD2
/// provided call back has to be with the signature bool (DataSet type, const char* line)
///
template <typename Callback>
inline void MVD2File::parse(Callback & lineParser) const{
    FILE *data;

    //Look for the file
    if ((data = fopen(_filename.c_str(), "r")) == NULL)
        throw MVDParserException("Could not find the mvd file" + _filename + "...\n");

    char line[1025] = {0};

    // drop header
    if( fgets(line, 255, data) == NULL
        || fgets(line, 255, data) == NULL){
        throw MVDParserException("Invalid header parsing for " + _filename );
    }

    DataSet type = None;
    size_t count = 0;
    while (fgets(line, 1024, data) != NULL) {
        DataSet prev_type = type;
        if( (type=getDataType(line, prev_type)) != prev_type){
            count = 0;
        }

        if(count>0){
            int res = lineParser(type, line);
            if(res == 1) {
                break;
            }
        }
        count++;
    }
    fclose(data);
}



/////////////////////////////////////////
/// Raw parsers
/////////////////////////////////////////

// Precision force to float to maintain compability, need to switch to double in future
inline void parseNeuronLine(const char* line, std::string & name, int& database, int &column, int &minicolumn, int &layer,
                     int &morphologytype, int &electrophysiology_type,
                     std::vector<float> & xyzr, std::string & metype){
    char name_str[1025] = "";
    char metype_str[1024] = "?";

    xyzr.resize(4);
    if( sscanf(line, "%s %d %d %d %d %d %d %f %f %f %f %s", name_str,
           &database, &column, &minicolumn, &layer, &morphologytype, &electrophysiology_type,
            &xyzr[0], &xyzr[1], &xyzr[2], &xyzr[3], metype_str) != 12){
        throw MVDParserException(std::string("Impossible to parse MVD2 neuron line :") + line);
    }

    metype = metype_str;
    name = name_str;
}


// Precision force to float to maintain compability, need to switch to double in future
inline void parseNeuronLine(const char* line, std::string & name, int& database, int &column, int &minicolumn, int &layer,
                     int &morphologytype, int &electrophysiology_type,
                     std::vector<double> & xyzr, std::string & metype){
    char name_str[1025] = "";
    char metype_str[1024] = "?";

    xyzr.resize(4);
    if( sscanf(line, "%s %d %d %d %d %d %d %lf %lf %lf %lf %s", name_str,
           &database, &column, &minicolumn, &layer, &morphologytype, &electrophysiology_type,
            &xyzr[0], &xyzr[1], &xyzr[2], &xyzr[3], metype_str) != 12){
        throw MVDParserException(std::string("Impossible to parse MVD2 neuron line :") + line);
    }

    metype = metype_str;
    name = name_str;
}


inline void parseSeedInitLine(const char *line, double &seed1, double &seed2, double &seed3){
    // TODO: parsing into simple precision done for compatibility, change in future MVD3
    float fseed1, fseed2, fseed3;
    if(sscanf(line, "%f %f %f", &fseed1, &fseed2, &fseed3) != 3){
        throw MVDParserException(std::string("Impossible to parse MVD2 Seed line :") + line);
    }
    seed1 = fseed1; seed2 = fseed2; seed3 = fseed3;
}


inline void parseMorphTypeLine(const char* line, std::string & name, std::string & name2, std::string & morphClass){
    char name_1[1025] = {0}, name_2[1025] = {0}, name_3[1025] = {0};
    if( sscanf(line, "%s %s %s", name_1, name_2, name_3) != 3){
         throw MVDParserException(std::string("Impossible to parse MVD2 MorphType line :") + line);
    }
    name = name_1;
    name2 = name_2;
    morphClass = name_3;
}


inline void parseElectroTypeLine(const char *line, std::string &electroType){
    // dummy parsing, no check needed for now
    electroType = line;
    boost::trim(electroType);

}


/////////////////////////////////////////////
/// members of Counter
////////////////////////////////////////////

inline Counter::Counter() :
        _nb_columns(0),
        _nb_neuron(0),
        _nb_morpho_type(0)
    {    }

inline int Counter::operator()(DataSet type, const char* line){
    (void) line;
    switch(type){
        case NeuronLoaded:{
            _nb_neuron +=1;
            std::string morpho, metype;
            int trash;
            std::vector<float> pos;
            parseNeuronLine(line, morpho, trash, trash, trash, trash, trash, trash, pos, metype);
            morphos.insert(morpho);
            break;
        }
        case MorphTypes:{
            _nb_morpho_type +=1;
            break;
        }
        case(MiniColumnsPosition):{
            _nb_columns +=1;
            break;
        }
        default:{
            break;
        }
    }
    return 0;
}


/////////////////////////////////////////////////////////
/// Structs defining the Callbacks
/// to read Ranges of of positions and rotations
/////////////////////////////////////////////////////////

struct PositionData {

    inline PositionData( MVD::Positions & positions, const MVD::Range & range = MVD::Range()) :
        _positions(positions),
        _range(range),
        _cur_neuron(0),
        _n_skipped(0)
    { }

    inline int operator()(DataSet type, const char* line){
        // Only handle NeuronLoaded
        if( type != NeuronLoaded )
            return 0;

        // Check count limit. _range.count==0 has the speciam meaning of DISABLED.
        if( _range.count > 0 && _cur_neuron>=_range.count ) {
            return 1;
        }

        std::string morpho, metype;
        int trash;
        std::vector<double> pos;
        parseNeuronLine(line, morpho, trash, trash, trash, trash, trash, trash, pos, metype);

        if (_range.offset > _n_skipped) {
            _n_skipped++;
        }
        else {
            std::copy(pos.begin(), pos.begin()+3, _positions[_cur_neuron].begin());
            _cur_neuron += 1;
        }
        return 0;

    }

    MVD::Positions & _positions;

private:
    const MVD::Range _range;
    size_t _cur_neuron;
    size_t _n_skipped;
};


struct RotationData {
    inline RotationData( MVD::Positions & rotations, const MVD::Range & range = MVD::Range() ) :
        _rotations(rotations),
        _range(range),
        _cur_neuron(0),
        _n_skipped(0)
    { }

    inline int operator()(DataSet type, const char* line){
        // Only handle NeuronLoaded
        if( type != NeuronLoaded )
            return 0;

        // Check count limit. _range.count==0 has the speciam meaning of DISABLED.
        if( _range.count > 0 && _cur_neuron>=_range.count ) {
            return 1;
        }

        std::string morpho, metype;
        int trash;
        std::vector<double> pos;
        parseNeuronLine(line, morpho, trash, trash, trash, trash, trash, trash, pos, metype);

        if (_range.offset > _n_skipped) {
            _n_skipped++;
        }
        else {
            _rotations[_cur_neuron][0] = pos[3];
            _cur_neuron += 1;
        }
        return 0;

    }

    MVD::Rotations & _rotations;

private:
    const MVD::Range _range;
    size_t _cur_neuron;
    size_t _n_skipped;
};




/////////////////////////////////////////////////////////
/// Class MVD2File members
/////////////////////////////////////////////////////////

inline size_t MVD2File::getNbNeuron() const {
    init_counter();
    return counter._nb_neuron;
}

inline size_t MVD2File::getNbMorphoType() const {
    init_counter();
    return counter._nb_morpho_type;
}

inline size_t MVD2File::getNbMorpho() const {
    init_counter();
    return counter.morphos.size();
}

inline size_t MVD2File::getNbColumns() const {
    init_counter();
    return counter._nb_columns;
}

inline std::set<std::string> & MVD2File::getUniqueMorphologies() const {
    init_counter();
    return counter.morphos;
}


inline MVD::Positions MVD2File::getPositions(const MVD::Range & range) const {
    init_counter();
    size_t count = (range.count>0 && range.count<counter._nb_neuron)? range.count : counter._nb_neuron;
    MVD::Positions posi_buff(boost::extents[count][3]);
    PositionData posi_data_reader(posi_buff, range);
    parse(posi_data_reader);
    return posi_buff;
}

inline MVD::Rotations MVD2File::getRotations(const MVD::Range & range) const {
    init_counter();
    size_t count = (range.count>0 && range.count<counter._nb_neuron)? range.count : counter._nb_neuron;
    MVD::Rotations rots_buff(boost::extents[count][1]);
    RotationData rots_data_reader(rots_buff, range);
    parse(rots_data_reader);
    return rots_buff;
}


inline void MVD2File::init_counter() const {
    if(counter._nb_neuron == 0){
        parse(counter);
    }
}



} // ::MVD2
