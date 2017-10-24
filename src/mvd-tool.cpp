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
#include <iostream>
#include <exception>

#include <mvd/mvd2.hpp>
#include <mvd/mvd3.hpp>

#include "converter.hpp"

using namespace std;

namespace commands{
    const std::string convert = "convert";
    const std::string print = "print";
    const std::string summary = "summary";
    const std::string help = "help";
    const std::string version = "version";
    const int n_cmd = 5;
}

bool is_valid_command(const char* argv){
    using namespace commands;
    const std::string cmds[] = { convert, print, summary, help, version };
    return std::find(cmds, cmds+ n_cmd, argv) != cmds+n_cmd;
}

int offset_command(const char* argv){
    using namespace commands;
    const std::string cmds[] = { convert, print, summary, help, version };
    return std::find(cmds, cmds+ n_cmd, argv) - cmds;
}


void print_csv(const std::string & filename){
    using namespace MVD3;

    const std::string delim = "; ";

    MVD3File file(filename);

    const size_t n_neuron = file.getNbNeuron();

    std::cout << "GID; POSITION_X; POSITION_Y; POSITION_Z; ROTATION_Q0; ROTATION_Q1; ROTATION_Q2; ROTATION_Q3;";
    std::cout << " MORPHO; MTYPE; ETYPE; SYNCLASS;" << "\n";

    size_t offset=0, size_read=0;
    size_t gid=0;
    while(offset < n_neuron){
        size_read = std::min(n_neuron-offset, size_t(200));
        const Range read_range = Range(offset, size_read);
        const Positions positions = file.getPositions(read_range);
        const Rotations rotations = file.getRotations(read_range);
        const std::vector<std::string> morphos = file.getMorphologies(read_range);
        const std::vector<std::string> mtypes = file.getMtypes(read_range);
        const std::vector<std::string> etypes = file.getEtypes(read_range);
        const std::vector<std::string> syn_class = file.getSynapseClass(read_range);

        assert( size_read == positions.shape()[0]
                && size_read == rotations.shape()[0]
                && size_read == morphos.size()
                && size_read == mtypes.size()
                && size_read == etypes.size()
                && size_read == syn_class.size());

        for(size_t i =0; i < size_read; ++i){
            std::cout << gid << delim;
            for(size_t j=0; j < 3; ++j){
                std::cout << positions[i][j] << delim;
            }
            for(size_t j=0; j < 4; ++j){
               std::cout << rotations[i][j] << delim;
            }
            std::cout << morphos[i] << delim;
            std::cout << mtypes[i] << delim;
            std::cout << etypes[i] << delim;
            std::cout << syn_class[i] << delim;
            std::cout << "\n";
            ++gid;

        }
        offset += size_read;
    }
}


void help(char** argv){
    std::cout << "Usage: " << argv[0] << " [COMMAND]\n";
    std::cout << "  List of commands :\n";
    std::cout << "             convert [mvd2_file] [mvd3_file]";
    std::cout << " : Convert a MVD 2.0 file into the MVD 3.0 file format\n";
    std::cout << "             summary [mvd3_file]            ";
    std::cout << " : Print summary of the circuit informations \n";
    std::cout << "             print [mvd3_file]              ";
    std::cout << " : Print in MVD 3.0 in human readable format (default csv) \n";
    std::cout << "             version                        ";
    std::cout << " : Display version of mvd-tool\n";
    std::cout << "             help                           ";
    std::cout << " : Display help of mvd-tool\n";
}

bool has_seeds(MVD3::MVD3File & file){
    try{
        return (file.getCircuitSeeds().size() >= 3);
    }catch(...){
        return false;
    }
}

void summary(const std::string & filename){
    using namespace MVD3;
    MVD3File file(filename);

    std::cout << "Circuit Summary\n";
    std::cout << "circuit_filename: " << filename << "\n";
    std::cout << "number_of_neurons: " << file.getNbNeuron() << "\n";
    std::cout << "number_of_morphologies: " << file.listAllMorphologies().size() << "\n";
    std::cout << "number_of_mtypes: " << file.listAllMtypes().size() << "\n";
    std::cout << "number_of_etypes: " << file.listAllEtypes().size() << "\n";
    std::cout << "number_of_synapse_class: " << file.listAllSynapseClass().size() << "\n";
    std::cout << "has_circuit_seeds: " << ((has_seeds(file))?("true"):("false")) << "\n";


}


std::string version(){
    return std::string( MVD_VERSION_MAJOR "." MVD_VERSION_MINOR );
}


int main(int argc, char** argv){
    if(argc < 2 || is_valid_command(argv[1]) == false){
        help(argv);
        exit(1);
    }

    try{
        switch(offset_command(argv[1])){
            case(0):{
                if(argc <4){
                    help(argv);
                    exit(1);
                }
                converter(argv[2], argv[3]);
                break;
            }

            case(1):{
                if(argc <3){
                    help(argv);
                    exit(1);
                }
               print_csv(argv[2]);
               break;
            }


            case(2):{
                if(argc <3){
                    help(argv);
                    exit(1);
                }
               summary(argv[2]);
               break;
            }

            case(4):{
                    std::cout << "version: " << version() << "\n";
                    exit(1);
                }

            default:{
                help(argv);
                exit(1);
            }

        }

    }catch(std::exception & e){
        std::cerr << "mvd-tool Error " << e.what();
        exit(-1);
    }
    return 0;
}

