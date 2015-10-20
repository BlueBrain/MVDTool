#ifndef NEURONINFO_HPP
#define NEURONINFO_HPP

#include <cstdint>


struct NeuronInfo{
     int64_t id;

     std::string etype;
     std::string morpho_name;

     double positions[4];

     double rotation[3];

};

#endif // NEURONINFO_HPP

