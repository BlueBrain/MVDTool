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
#include "converter.hpp"

#include <cmath>
#include <map>
#include <mvd/mvd2.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"

#include <boost/array.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/integer.hpp>

#pragma GCC diagnostic pop

#include <highfive/H5File.hpp>

void converter_log(const std::string & msg){
static size_t step = 0;
    std::cout << step++ << ": " << msg << std::endl;
}

struct MVD2Infos{
    inline MVD2Infos(const size_t n_neurons) :
        neuron_line(0),
        vec_xyzr(n_neurons),
        prop_hypercolumn(n_neurons),
        prop_minicolumn(n_neurons),
        prop_layer(n_neurons),
        prop_mtype(n_neurons),
        prop_etype(n_neurons),
        morphologies(n_neurons),
        me_combos(n_neurons),
        seeds(4,0)
    {
        for(std::vector< std::vector<double> >::iterator it = vec_xyzr.begin(); it < vec_xyzr.end(); ++it){
            it->resize(4);
        }
    }


    int operator()(MVD2::DataSet type, const char* line){
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
            case CircuitSeeds:{
                MVD2::parseSeedInitLine(line, seeds[0], seeds[1], seeds[2]);
            }
                break;
            default:
                break;
        }
        return 0;
    }

    void parseNeuron(const char* line){
        assert(neuron_line < vec_xyzr.size());
        int unused, hypercolumn, minicolumn, layer, mtype, etype;
        std::string morpho_name, me_combo;
        std::vector<double> xyzr;

        MVD2::parseNeuronLine(line,
                              morpho_name,
                              unused,
                              hypercolumn,
                              minicolumn,
                              layer,
                              mtype,
                              etype,
                              xyzr,
                              me_combo);

        prop_hypercolumn[neuron_line] = hypercolumn;
        prop_minicolumn[neuron_line] = minicolumn;
        prop_layer[neuron_line] = 1 + layer;
        prop_mtype[neuron_line] = mtype;
        prop_etype[neuron_line] = etype;
        morphologies[neuron_line].swap(morpho_name);
        me_combos[neuron_line].swap(me_combo);
        vec_xyzr[neuron_line].swap(xyzr);

        neuron_line +=1;
    }

    void parseElectro(const char* line){
        std::string eletro_type;
        MVD2::parseElectroTypeLine(line, eletro_type);
        etypes.push_back(eletro_type);
    }

    void parseMorphType(const char* line){
        std::string mtype_name, morph_class, synapse_class;
        MVD2::parseMorphTypeLine(line, mtype_name, morph_class, synapse_class);
        mtype_names.push_back(mtype_name);
        mtype_mclass.push_back(morph_class);
        mtype_syn_class.push_back(synapse_class);
    }

    // counter
    size_t neuron_line;

    // mvd2 infos
    std::vector< std::vector<double> > vec_xyzr;
    std::vector<boost::int32_t> prop_hypercolumn;
    std::vector<boost::int32_t> prop_minicolumn;
    std::vector<boost::int32_t> prop_layer;
    std::vector<size_t> prop_mtype;
    std::vector<size_t> prop_etype;
    std::vector<std::string> morphologies;
    std::vector<std::string> me_combos;

    std::vector<double> seeds;

    // etypes
    std::vector<std::string> etypes;

    // mtypes
    std::vector<std::string> mtype_names;
    std::vector<std::string> mtype_mclass;
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

        prop_hypercolumn.resize(n_neurons);
        prop_minicolumn.resize(n_neurons);
        prop_layer.resize(n_neurons);
        prop_etype.resize(n_neurons);
        prop_mtype.resize(n_neurons);
        prop_morpho.resize(n_neurons);
        prop_mclass.resize(n_neurons);
        prop_synclass.resize(n_neurons);
        prop_me_combo.resize(n_neurons);
    }

    // neurons infos
    std::vector<std::vector<double> > position;
    std::vector<std::vector<double> > rotation;

    //properties
    std::vector<int> prop_hypercolumn;
    std::vector<int> prop_minicolumn;
    std::vector<int> prop_layer;
    std::vector<size_t> prop_etype;
    std::vector<size_t> prop_morpho;
    std::vector<size_t> prop_mtype;
    std::vector<size_t> prop_mclass;
    std::vector<size_t> prop_synclass;
    std::vector<size_t> prop_me_combo;

    // infos
    std::vector<std::string> etypes;
    std::vector<std::string> morphologies;
    std::vector<std::string> morph_class;
    std::vector<std::string> synapse_class;
    std::vector<std::string> mtypes;
    std::vector<std::string> me_combos;

    // circuit
    std::vector<double> seeds;
};


void move_coordinates(MVD2Infos & infos, MVD3Infos & result){
    for(size_t i = 0; i < infos.vec_xyzr.size(); ++i){
        for(int j =0; j < 3; ++j){
            result.position[i][j] = infos.vec_xyzr[i][j];
        }

        // MVD2 gives only rotation angle on axe Y and in degree
        // convert to rad and construct quaternion
        const double deg_rad_r = boost::math::constants::pi<double>() / 180.0;
        const double angle_y = infos.vec_xyzr[i][3]*deg_rad_r;

        // quaternion order  (x,y,z,w)
        result.rotation[i][0] = result.rotation[i][2] = 0;
        result.rotation[i][1] = sin(angle_y/2);
        result.rotation[i][3] = cos(angle_y/2);
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

void move_me_combos(MVD2Infos & infos, MVD3Infos & result){
    hdf5_index_create(infos.me_combos, result.prop_me_combo, result.me_combos);
    infos.me_combos.clear();
}

void move_etype(MVD2Infos & infos, MVD3Infos & result){
    result.etypes.swap(infos.etypes);
    result.prop_etype.swap(infos.prop_etype);
}


void move_mtype_all(MVD2Infos & infos, MVD3Infos & result){
    result.mtypes.swap(infos.mtype_names);
    result.prop_mtype.swap(infos.prop_mtype);

    std::vector<size_t> index_mtype_to_mclass(infos.mtype_mclass.size());
    hdf5_index_create(infos.mtype_mclass, index_mtype_to_mclass, result.morph_class);

    std::vector<size_t> index_mtype_to_syn_class(infos.mtype_syn_class.size());
    hdf5_index_create(infos.mtype_syn_class, index_mtype_to_syn_class, result.synapse_class);

    result.prop_mclass.resize(result.prop_mtype.size(), 0);
    result.prop_synclass.resize(result.prop_mtype.size(), 0);

    for(size_t i = 0; i < result.prop_mtype.size(); ++i){
        const size_t index = result.prop_mtype[i];
        assert(index < index_mtype_to_mclass.size());
        assert(index < index_mtype_to_syn_class.size());
        result.prop_mclass[i] = index_mtype_to_mclass[index];
        result.prop_synclass[i] = index_mtype_to_syn_class[index];
    }

    infos.mtype_mclass.clear();
    infos.mtype_syn_class.clear();
}

void move_int_props(MVD2Infos & infos, MVD3Infos & result){
    result.prop_hypercolumn.swap(infos.prop_hypercolumn);
    result.prop_minicolumn.swap(infos.prop_minicolumn);
    result.prop_layer.swap(infos.prop_layer);
}


void transform(MVD2Infos & infos, MVD3Infos & result){
    result.setSize(infos.vec_xyzr.size());

    move_coordinates(infos, result);

    move_morphologies(infos, result);

    move_me_combos(infos, result);

    move_int_props(infos, result);

    move_etype(infos, result);

    move_mtype_all(infos, result);

    result.seeds = infos.seeds;
}

void converter(const std::string & mvd2, const std::string & mvd3){

    using namespace HighFive;
    MVD3Infos mvd3_content;

    {
        converter_log("Open MVD2 file " + mvd2);
        MVD2::MVD2File file(mvd2);

        const size_t n_neuron = file.getNbNeuron();

        std::ostringstream ss;
        ss << "Contains " << n_neuron << " neurons";
        converter_log(ss.str());
        MVD2Infos mvd2_content(n_neuron);

        converter_log("Parse MVD2");
        file.parse(mvd2_content);
        converter_log("Transform data layout");
        transform(mvd2_content, mvd3_content);
    }

    {
        converter_log("Create and write MVD3 "+ mvd3);
        File mvd3_file(mvd3, File::ReadWrite | File::Create | File::Truncate);
        Group cells = mvd3_file.createGroup("cells");
        cells.createDataSet<double>("positions", DataSpace::From(mvd3_content.position)).write(mvd3_content.position);
        cells.createDataSet<double>("orientations", DataSpace::From(mvd3_content.rotation)).write(mvd3_content.rotation);

        Group properties = cells.createGroup("properties");

        properties.createDataSet<int>("hypercolumn", DataSpace::From(mvd3_content.prop_hypercolumn)).write(mvd3_content.prop_hypercolumn);

        properties.createDataSet<int>("minicolumn", DataSpace::From(mvd3_content.prop_minicolumn)).write(mvd3_content.prop_minicolumn);

        properties.createDataSet<int>("layer", DataSpace::From(mvd3_content.prop_layer)).write(mvd3_content.prop_layer);

        properties.createDataSet<size_t>("etype", DataSpace::From(mvd3_content.prop_etype)).write(mvd3_content.prop_etype);

        properties.createDataSet<size_t>("mtype", DataSpace::From(mvd3_content.prop_mtype)).write(mvd3_content.prop_mtype);

        properties.createDataSet<size_t>("morphology", DataSpace::From(mvd3_content.prop_morpho)).write(mvd3_content.prop_morpho);

        properties.createDataSet<size_t>("morph_class", DataSpace::From(mvd3_content.prop_mclass)).write(mvd3_content.prop_mclass);

        properties.createDataSet<size_t>("synapse_class", DataSpace::From(mvd3_content.prop_synclass)).write(mvd3_content.prop_synclass);

        properties.createDataSet<size_t>("me_combo", DataSpace::From(mvd3_content.prop_me_combo)).write(mvd3_content.prop_me_combo);

        Group library = mvd3_file.createGroup("library");

        // create morphologies
        library.createDataSet<std::string>("morphology", DataSpace::From(mvd3_content.morphologies)).write(mvd3_content.morphologies);

        library.createDataSet<std::string>("etype", DataSpace::From(mvd3_content.etypes)).write(mvd3_content.etypes);

        library.createDataSet<std::string>("mtype", DataSpace::From(mvd3_content.mtypes)).write(mvd3_content.mtypes);

        library.createDataSet<std::string>("morph_class", DataSpace::From(mvd3_content.morph_class)).write(mvd3_content.morph_class);

        library.createDataSet<std::string>("synapse_class", DataSpace::From(mvd3_content.synapse_class)).write(mvd3_content.synapse_class);

        library.createDataSet<std::string>("me_combo", DataSpace::From(mvd3_content.me_combos)).write(mvd3_content.me_combos);

        Group circuit = mvd3_file.createGroup("circuit");
        circuit.createDataSet<double>("seeds", DataSpace::From(mvd3_content.seeds)).write(mvd3_content.seeds);
    }

    converter_log("Convert: Done");

}
