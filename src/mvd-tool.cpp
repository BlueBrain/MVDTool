#include <iostream>
#include <exception>


#include <mvd/mvd2.hpp>

#include "converter.hpp"

using namespace std;

namespace commands{
    const std::string convert = "convert";
    const std::string help = "help";
    const std::string version = "version";
    const int n_cmd = 3;
}

bool is_valid_command(const char* argv){
    using namespace commands;
    const std::string cmds[] = { convert, help, version };
    return std::find(cmds, cmds+ n_cmd, argv) != cmds+n_cmd;
}

int offset_command(const char* argv){
    using namespace commands;
    const std::string cmds[] = { convert, help, version };
    return std::find(cmds, cmds+ n_cmd, argv) - cmds;
}


void help(char** argv){
    std::cout << "Usage: " << argv[0] << " [COMMAND]\n";
    std::cout << "  List of commands :\n";
    std::cout << "             convert [mvd2_file] [mvd3_file]";
    std::cout << " : Convert a MVD 2.0 file into the MVD 3.0 file format\n";
    std::cout << "             version                        ";
    std::cout << " : Display version of mvd-tool\n";
    std::cout << "             help                           ";
    std::cout << " : Display help of mvd-tool\n";
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

            case(2):{
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
}

