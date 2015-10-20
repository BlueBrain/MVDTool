#ifndef MVD2_MISC_HPP
#define MVD2_MISC_HPP

#include <cstdlib>
#include <boost/algorithm/string.hpp>

#include "../mvd2.hpp"
#include "../mvd_except.hpp"


namespace MVD2{


enum DataSet{
    None = 0,
    NeuronLoaded = 1,
    MicroBoxData = 2,
    MiniColumnsPosition = 3,
    CircuitSeeds = 4,
    MorphTypes = 5,
    ElectroTypes = 6
};


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
inline void parseFile(const std::string & filename, Callback & lineParser){
    FILE *data;

    //Look for the file
    if ((data = fopen(filename.c_str(), "r")) == NULL)
        throw MVDParserException("Could not find the mvd file" + filename + "...\n");

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
    Counter(size_t & nb_neuron, size_t & morpho) :
        _nb_neuron(nb_neuron),
        _morpho(morpho){

    }

    void operator()(DataSet type, const char* line){
        (void) line;
        switch(type){
            case NeuronLoaded:{
                _nb_neuron +=1;
                break;
            }
            case MorphTypes:{
                _morpho +=1;
                break;
            }
            default:{
                break;
            }
        }
    }

private:
    size_t & _nb_neuron;
    size_t & _morpho;
};



inline size_t MVD2File::getNbNeuron(){
    init_counter();
    return _nb_morpho;
}



inline size_t MVD2File::getNbMorpho(){
    init_counter();
    return _nb_neuron;
}


inline void MVD2File::init_counter(){
    if(_nb_neuron == 0){
        Counter c(_nb_neuron, _nb_morpho);
        parseFile(_filename, c);
    }
}

}


#endif // MVD2_MISC_HPP
