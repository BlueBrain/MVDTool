#include <iostream>
#include <exception>


#include <mvd/mvd2.hpp>

#include "converter.hpp"

using namespace std;

namespace commands{
    const std::string convert = "convert";
}


void help(char** argv){
    std::cout << "Usage: " << argv[0] << " [COMMAND]\n";
    std::cout << "  commands:\n";
    std::cout << "             convert [mvd2_file] [mvd3_file]";
    std::cout << " : Convert a MVD 2.0 file into the MVD 3.0 file format\n";

}


int main(int argc, char** argv){
    if(argc < 4 || commands::convert.compare(argv[1]) != 0){
        help(argv);
        exit(1);
    }

    try{
        converter(argv[2], argv[3]);
    }catch(std::exception & e){
        std::cerr << "mvd-tool Error " << e.what();
        exit(-1);
    }
}

