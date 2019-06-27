#include <mvd/mvd3.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

using namespace pybind11::literals;
using namespace MVD3;

PYBIND11_MODULE(mvdtool, toplevel) {
    toplevel.doc() = "Module to read neuron circuits";
    py::module m = toplevel.def_submodule("MVD3", "Support for the MVD3 format");

    py::class_<MVD3File>(m, "File")
        .def(py::init<const std::string&>())
        .def("__len__", &MVD3File::getNbNeuron)
        .def("positions", [](const MVD3File& f, int offset, int count) {
                MVD::Range r(offset, count);
                auto res = f.getPositions(r);
                return py::array({res.shape()[0], res.shape()[1]}, res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("rotations", [](const MVD3File& f, int offset, int count) {
                MVD::Range r(offset, count);
                auto res = f.getRotations(r);
                return py::array({res.shape()[0], res.shape()[1]}, res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)

        .def("etypes", [](const MVD3File& f, int offset, int count) {
                MVD::Range r(offset, count);
                return f.getEtypes(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("layers", [](const MVD3File& f, int offset, int count) {
                MVD::Range r(offset, count);
                return f.getLayers(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("mtypes", [](const MVD3File& f, int offset, int count) {
                MVD::Range r(offset, count);
                return f.getMtypes(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("morphologies", [](const MVD3File& f, int offset, int count) {
                MVD::Range r(offset, count);
                return f.getMorphologies(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("synapse_classes", [](const MVD3File& f, int offset, int count) {
                MVD::Range r(offset, count);
                return f.getSynapseClass(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("regions", [](const MVD3File& f, int offset, int count) {
                MVD::Range r(offset, count);
                return f.getRegions(r);
             },
             "offset"_a = 0,
             "count"_a = 0)

        .def("raw_etypes", [](const MVD3File& f, int offset, int count) {
                MVD::Range r(offset, count);
                auto res = f.getIndexEtypes(r);
                return py::array(res.size(), res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("raw_mtypes", [](const MVD3File& f, int offset, int count) {
                MVD::Range r(offset, count);
                auto res = f.getIndexMtypes(r);
                return py::array(res.size(), res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("raw_morphologies", [](const MVD3File& f, int offset, int count) {
                MVD::Range r(offset, count);
                auto res = f.getIndexMorphologies(r);
                return py::array(res.size(), res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("raw_synapse_classes", [](const MVD3File& f, int offset, int count) {
                MVD::Range r(offset, count);
                auto res = f.getIndexSynapseClass(r);
                return py::array(res.size(), res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("raw_regions", [](const MVD3File& f, int offset, int count) {
                MVD::Range r(offset, count);
                auto res = f.getIndexRegions(r);
                return py::array(res.size(), res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)

        .def_property_readonly("all_etypes", &MVD3File::listAllEtypes)
        .def_property_readonly("all_morphologies", &MVD3File::listAllMorphologies)
        .def_property_readonly("all_mtypes", &MVD3File::listAllMtypes)
        .def_property_readonly("all_regions", &MVD3File::listAllRegions)
        .def_property_readonly("all_synapse_classes", &MVD3File::listAllSynapseClass)
        ;
}
