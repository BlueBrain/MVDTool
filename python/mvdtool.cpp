#include <mvd/mvd_generic.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

using namespace pybind11::literals;
using namespace MVD;
using namespace MVD3;

class PyFile : public File {
    using File::File;

    size_t getNbNeuron() const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            size_t,
            File,
            "__len__",
            getNbNeuron,
        );
    }

    Positions getPositions(const Range & range = Range(0,0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            Positions,
            File,
            "positions",
            getPositions
        );
    }
    Rotations getRotations(const Range & range = Range(0,0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            Rotations,
            File,
            "rotations",
            getRotations
        );
    }

    std::vector<std::string> getMorphologies(const Range& range = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<std::string>,
            File,
            "morphologies",
            getMorphologies
        );
    }
    std::vector<std::string> getEtypes(const Range& range = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<std::string>,
            File,
            "etypes",
            getEtypes
        );
    }
    std::vector<std::string> getMtypes(const Range& range = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<std::string>,
            File,
            "mtypes",
            getMtypes
        );
    }
    std::vector<std::string> getRegions(const Range& range = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<std::string>,
            File,
            "regions",
            getRegions
        );
    }
    std::vector<std::string> getSynapseClass(const Range& range = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<std::string>,
            File,
            "synapse_classes",
            getSynapseClass
        );
    }

    std::vector<size_t> getIndexEtypes(const Range& range = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<size_t>,
            File,
            "raw_etypes",
            getIndexEtypes
        );
    }
    std::vector<size_t> getIndexMtypes(const Range& range = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<size_t>,
            File,
            "raw_mtypes",
            getIndexMtypes
        );
    }
    std::vector<size_t> getIndexRegions(const Range& range = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<size_t>,
            File,
            "raw_regions",
            getIndexRegions
        );
    }
    std::vector<size_t> getIndexSynapseClass(const Range& range = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<size_t>,
            File,
            "raw_synapse_classes",
            getIndexSynapseClass
        );
    }

    std::vector<std::string> listAllEtypes() const {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<std::string>,
            File,
            "all_etypes",
            listAllEtypes,
        );
    }
    std::vector<std::string> listAllMtypes() const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<std::string>,
            File,
            "all_mtypes",
            listAllMtypes,
        );
    }
    std::vector<std::string> listAllRegions() const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<std::string>,
            File,
            "all_regions",
            listAllRegions,
        );
    }
    std::vector<std::string> listAllSynapseClass() const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<std::string>,
            File,
            "all_synapse_classes",
            listAllSynapseClass,
        );
    }
};

PYBIND11_MODULE(mvdtool, mvd) {
    mvd.doc() = "Module to read neuron circuits";
    py::module mvd3 = mvd.def_submodule("MVD3", "Support for the MVD3 format");
    py::module sonata = mvd.def_submodule("sonata", "Support for the SONATA format");

    mvd.def("open", &open, "filename"_a);

    py::class_<File, PyFile> file(mvd, "__File");
    file
        .def(py::init<>())
        .def("__len__", &File::size)
        .def("positions", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getPositions(r);
                return py::array({res.shape()[0], res.shape()[1]}, res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("rotations", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getRotations(r);
                return py::array({res.shape()[0], res.shape()[1]}, res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)

        .def("etypes", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getEtypes(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("mtypes", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getMtypes(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("morphologies", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getMorphologies(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("synapse_classes", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getSynapseClass(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("regions", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getRegions(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("raw_etypes", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getIndexEtypes(r);
                return py::array(res.size(), res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("raw_mtypes", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getIndexMtypes(r);
                return py::array(res.size(), res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("raw_synapse_classes", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getIndexSynapseClass(r);
                return py::array(res.size(), res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("raw_regions", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getIndexRegions(r);
                return py::array(res.size(), res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def_property_readonly("all_etypes", &File::listAllEtypes)
        .def_property_readonly("all_mtypes", &File::listAllMtypes)
        .def_property_readonly("all_regions", &File::listAllRegions)
        .def_property_readonly("all_synapse_classes", &File::listAllSynapseClass)
        ;

    py::class_<MVD3File, std::shared_ptr<MVD3File>>(mvd3, "File", file)
        .def(py::init<const std::string&>())
        .def("layers", [](const MVD3File& f, int offset, int count) {
                Range r(offset, count);
                return f.getLayers(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("raw_morphologies", [](const MVD3File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getIndexMorphologies(r);
                return py::array(res.size(), res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def_property_readonly("all_morphologies", &MVD3File::listAllMorphologies)
        ;

    py::class_<SonataFile, std::shared_ptr<SonataFile>>(sonata, "File", file)
        .def(py::init<const std::string&>())
        ;
}
