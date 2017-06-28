#include <mvd/mvd2.hpp>

namespace MVD2{
namespace Data{

struct MVD2ColData{

    typedef struct {
        float x, y, z;
    } xyz_t;

    MVD2ColData():
        neuron_counter(0),
        morph_array(0)
        { }

    void operator ()(MVD2::DataSet type, const char* line){
        using namespace MVD2;
        switch(type){
            case(NeuronLoaded):{
                std::string name;
                int database, column, minicolumn, layer, morph, elec;
                std::vector<float> xyzr;
                std::string metype;
                parseNeuronLine(line, name, database, column, minicolumn, layer, morph, elec, xyzr, metype);
                names.push_back(name);
                metypes.push_back(metype);
                databases.push_back(database);
                columns.push_back(column);
                minicolumns.push_back(minicolumn);
                layers.push_back(layer);
                morphs.push_back(morph);
                elecs.push_back(elec);
                neuron_pos.push_back(((_xyzr*)xyzr.data())->xyz);
                rotations.push_back(((_xyzr*)xyzr.data())->r);

                neuron_counter +=1;
                break;
            }
            case(CircuitSeeds):{
                circuitSeeds.resize(3);
                parseSeedInitLine(line, circuitSeeds[0], circuitSeeds[1], circuitSeeds[2]);
                break;
            }
            // case(MorphTypes):{
            //     std::string name1, name2, name3;
            //     parseMorphTypeLine(line, name1, name2, name3);
            //     if(morph_array ==0){
            //         BOOST_CHECK_EQUAL(name1, "L1_SLAC");
            //         BOOST_CHECK_EQUAL(name2, "INT");
            //         BOOST_CHECK_EQUAL(name3, "INH");
            //     }
            //     morph_array +=1;
            //     break;
            // }
            // case(ElectroTypes):{
            //     std::string str;
            //     parseElectroTypeLine(line, str);
            //     BOOST_CHECK( str.compare("cACint") ==0 || str.compare("cADpyr") ==0);
            //     break;
            // }
            default:{

            }
        }
    }

    ///
    /// \brief getNbNeuron
    /// \return total number of neurons contained in the receipe
    ///
    size_t getNbNeuron() const {
        return neuron_counter;
    }

    ///
    /// \brief getPositions
    /// \param range: selection range, the default range (0,0) select the entire dataset
    /// \return a double vector of size [N][3] with the position (x,y,z) coordinates
    ///  of each selected neurons ( all by default )
    ///
    const std::vector<xyz_t> & getPositions() const {
        return neuron_pos;
    }

    ///
    /// \brief getPositions
    /// \return a double vector of size N with the rotations
    /// of each selected neurons ( all by default )
    ///
    const std::vector<float> & getRotations() const {
        return rotations;
    }

    ///
    /// \brief getMorphologies
    /// \param range: selection range, a null range (0,0) select the entire dataset
    /// \return vector of string with the morphology name associated with each neuron
    ///
    // std::vector<std::string> getMorphologies() const {
    //     return morphs;
    // }

    ///
    /// \brief getMtypes
    /// \return vector of string with the Mtype name associated with each neuron
    ///
    // const std::vector<std::string> getMetypes() const {
    //     return metypes;
    // }


    // ///
    // /// \brief getSynapseClass
    // /// \return vector of string with the synapse type associated with each neuron
    // ///
    // const std::vector<std::string> getSynapseClass() const;
    //
    //
    // // index related infos
    //
    // ///

    /// \brief getIndexMorphologies
    /// \param range
    /// \return values of the morphology index for neurons in the range, default: entire dataset
    ///
    const std::vector<int> & getIndexMorphologies() const {
        return morphs;
    }



    ///
    /// \brief getIndexEtypes
    /// \param range
    /// \return values of the Etypes index for neurons in the range, default: entire dataset
    ///
    const std::vector<int> & getIndexEtypes() const {
        return elecs;
    }


    ///
    /// \brief getIndexMtypes
    /// \param range
    // /// \return values of the Mtypes index for neurons in the range, default: entire dataset
    // ///
    // std::vector<int> getIndexMtypes() const;
    // //
    // ///
    // /// \brief getIndexSynapseClass
    // /// \param range
    // /// \return values of the Synapse class index for neurons in the range, default: entire dataset
    // ///
    // std::vector<size_t> getIndexSynapseClass(const Range & range = Range(0,0)) const;
    //
    // // Data related infos
    //
    // ///
    // /// \brief getDataMorphologies
    // /// \return vector of all unique morphologies ( mvd3 /library section )
    // ///
    // std::vector<std::string> listAllMorphologies() const;

    // ///
    // /// \brief listAllEtypes
    // /// \return vector of all unique Etypes ( mvd3 /library section )
    // ///
    // std::vector<std::string> listAllEtypes() const;
    //
    // ///
    // /// \brief listAllMtypes
    // /// \return vector of all unique Mtypes ( mvd3 /library section )
    // ///
    // std::vector<std::string> listAllMtypes() const;
    //
    // ///
    // /// \brief listSynapseClass
    // /// \return vector of all unique Synapse Class ( mvd3 /library section )
    // ///
    // std::vector<std::string> listAllSynapseClass() const;

    // circuit infos
    ///
    /// \brief getCircuitSeeds
    /// \return return a vector of the circuit seeds to use for random initialization
    /// the vector has to be at least 4 elements
    ///
    const std::vector<double> getCircuitSeeds() const {
        return circuitSeeds;
    }

private:

    typedef struct {
        xyz_t xyz;
        float r;
    }_xyzr;

    size_t neuron_counter;
    size_t morph_array;

    std::vector<std::string> names, metypes;
    std::vector<int> databases, columns, minicolumns, layers, morphs, elecs;
    std::vector<xyz_t> neuron_pos;
    std::vector<float> rotations;
    std::vector<double> circuitSeeds;

};

}; //Data
}; //MVD2
