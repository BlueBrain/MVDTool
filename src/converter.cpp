#include "converter.hpp"

#include <map>
#include <mvd/mvd2.hpp>
#include <highfive/H5File.hpp>



struct MVD2Infos{
    inline MVD2Infos(const size_t n_neurons) :
        neuron_line(0),
        vec_xyzr(n_neurons),
        prop_mtype(n_neurons),
        prop_etype(n_neurons),
        morphologies(n_neurons)
    {
        for(std::vector< std::vector<double> >::iterator it = vec_xyzr.begin(); it < vec_xyzr.end(); ++it){
            it->resize(4);
        }
    }


    void operator()(MVD2::DataSet type, const char* line){
        using namespace MVD2;
        switch(type){
            case NeuronLoaded:{
                    parseNeuron(line);
                }
                break;
            case ElectroTypes:{
                    parseElectro(line);
                }
                break;
            case MorphTypes:{
                    parseMorphType(line);
            }
            break;
            default:
                break;
        }
    }

    void parseNeuron(const char* line){
        assert(neuron_line < vec_xyzr.size());
        int unused, mtype, etype;
        std::string morpho_name, metype;
        std::vector<double> xyzr;

        MVD2::parseNeuronLine(line,
                              morpho_name,
                              unused,
                              unused,
                              unused,
                              unused,
                              mtype,
                              etype,
                              xyzr,
                              metype);

        prop_mtype[neuron_line] = mtype;
        prop_etype[neuron_line] = etype;
        morphologies[neuron_line].swap(morpho_name);
        vec_xyzr[neuron_line].swap(xyzr);

        neuron_line +=1;
    }

    void parseElectro(const char* line){
        std::string eletro_type;
        MVD2::parseElectroTypeLine(line, eletro_type);
        etypes.push_back(eletro_type);
    }

    void parseMorphType(const char* line){
        std::string mtype_name, mtype_name2, mtype_class;
        MVD2::parseMorphTypeLine(line, mtype_name, mtype_name2, mtype_class);
        mtype_names.push_back(mtype_name);
        mtype_syn_class.push_back(mtype_class);
    }

    // counter
    size_t neuron_line;

    // mvd2 infos
    std::vector< std::vector<double> > vec_xyzr;
    std::vector<size_t> prop_mtype;
    std::vector<size_t> prop_etype;
    std::vector<std::string> morphologies;

    // etypes
    std::vector<std::string> etypes;

    // mtypes
    std::vector<std::string> mtype_names;
    std::vector<std::string> mtype_syn_class;




};


struct MVD3Infos{

    void setSize(const size_t n_neurons){
        position.resize(n_neurons);
        for(std::vector< std::vector< double > >::iterator it= position.begin(); it < position.end(); ++it){
            it->resize(3, 0);
        }
        rotation.resize(n_neurons);
        for(std::vector< std::vector< double > >::iterator it= rotation.begin(); it < rotation.end(); ++it){
            it->resize(4, 0);

        }

        prop_etype.resize(n_neurons);
        prop_mtype.resize(n_neurons);
        prop_morpho.resize(n_neurons);
        prop_synclass.resize(n_neurons);
    }

    // neurons infos
    std::vector<std::vector<double> > position;
    std::vector<std::vector<double> > rotation;
    //properties
    std::vector<size_t> prop_etype;
    std::vector<size_t> prop_morpho;
    std::vector<size_t> prop_mtype;
    std::vector<size_t> prop_synclass;

    // infos
    std::vector<std::string> etypes;
    std::vector<std::string> morphologies;
    std::vector<std::string> synapse_class;
    std::vector<std::string> mtypes;
};


void move_coordinates(MVD2Infos & infos, MVD3Infos & result){
    for(size_t i = 0; i < infos.vec_xyzr.size(); ++i){
        for(int j =0; j < 3; ++j){
            result.position[i][j] = infos.vec_xyzr[i][j];
        }
        // set Y axis rotation value
        result.rotation[i][1] = infos.vec_xyzr[i][3];
    }
    infos.vec_xyzr.clear();
}

template<typename Input, typename OutputIndex, typename ValueVector>
void hdf5_index_create(const Input & input_data, OutputIndex & index, ValueVector & vec_value){
    typedef typename std::map<typename Input::value_type, size_t> MapIndexType;
    MapIndexType mapIndex;

    for(size_t i = 0; i < input_data.size(); ++i){
        typename MapIndexType::iterator elem = mapIndex.insert(std::make_pair(input_data[i], vec_value.size())).first;
        index[i] = elem->second;
        if(vec_value.size() == elem->second){
            vec_value.push_back(elem->first);
        }
    }
}

void move_morphologies(MVD2Infos & infos, MVD3Infos & result){
    hdf5_index_create(infos.morphologies, result.prop_morpho, result.morphologies);
    infos.morphologies.clear();
}

void move_etype(MVD2Infos & infos, MVD3Infos & result){
    hdf5_index_create(infos.etypes, result.prop_etype, result.etypes);
    infos.etypes.clear();
}


void move_mtype_and_syn_class(MVD2Infos & infos, MVD3Infos & result){
    result.mtypes.swap(infos.mtype_names);
    result.prop_mtype.swap(infos.prop_mtype);

    std::vector<size_t> index_morph_to_syn_class(infos.mtype_syn_class.size());
    hdf5_index_create(infos.mtype_syn_class, index_morph_to_syn_class, result.synapse_class);

    result.prop_synclass.resize(result.prop_mtype.size(), 0);


    for(size_t i = 0; i < result.prop_mtype.size(); ++i){
        const size_t index = result.prop_mtype[i];
        assert(index < index_morph_to_syn_class.size());
        result.prop_synclass[i] = index_morph_to_syn_class[index];
    }

    infos.mtype_syn_class.clear();
}




void transform(MVD2Infos & infos, MVD3Infos & result){
    result.setSize(infos.vec_xyzr.size());

    move_coordinates(infos, result);

    move_morphologies(infos, result);

    move_etype(infos, result);

    move_mtype_and_syn_class(infos, result);
}

void converter(const std::string & mvd2, const std::string & mvd3){

    using namespace HighFive;
    MVD3Infos mvd3_content;

    {
        MVD2::MVD2File file(mvd2);

        const size_t n_neuron = file.getNbNeuron();
        MVD2Infos mvd2_content(n_neuron);

        file.parse(mvd2_content);
        transform(mvd2_content, mvd3_content);
    }

    File mvd3_file(mvd3, File::ReadWrite | File::Create | File::Truncate);

    // create morphologies
    mvd3_file.createDataSet< std::vector<std::string> >("/morphology", mvd3_content.morphologies).write(mvd3_content.morphologies);



}
