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
#ifndef MVD2_MISC_HPP
#define MVD2_MISC_HPP

#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <cstdio>
#include <set>

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
/// provided call back has to be with the signature void (DataSet type, const char* line)
///
template <typename Callback>
inline void MVD2File::parse(Callback & lineParser){
    FILE *data;

    //Look for the file
    if ((data = fopen(_filename.c_str(), "r")) == NULL)
        throw MVDParserException("Could not find the mvd file" + _filename + "...\n");

    char line[1025] = {0};

    (void) fgets(line, 255, data);
    (void) fgets(line, 255, data);
    // drop header

    DataSet type = None;
    size_t count = 0;
    while (fgets(line, 1024, data) != NULL) {
        DataSet prev_type = type;
        if( (type= getDataType(line, prev_type)) != prev_type){
            count = 0;
        }

        if(count>0){
            lineParser(type, line);
        }
        count++;
    }
    fclose(data);
}



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


struct Counter{
    Counter(size_t & nb_neuron, size_t & nb_morpho_type) :
        _nb_neuron(nb_neuron),
        _nb_morpho_type(nb_morpho_type){

    }

    void operator()(DataSet type, const char* line){
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
            default:{
                break;
            }
        }
    }

    size_t & _nb_neuron;
    size_t & _nb_morpho_type;
    std::set<std::string> morphos;
};



inline size_t MVD2File::getNbNeuron(){
    init_counter();
    return _nb_neuron;
}



inline size_t MVD2File::getNbMorphoType(){
    init_counter();
    return _nb_morpho_type;
}


inline size_t MVD2File::getNbMorpho(){
    init_counter();
    return _nb_morpho;
}


inline void MVD2File::init_counter(){
    if(_nb_neuron == 0){
        Counter c(_nb_neuron, _nb_morpho_type);
        parse(c);
        _nb_morpho = c.morphos.size();
    }
}

}


#endif // MVD2_MISC_HPP
