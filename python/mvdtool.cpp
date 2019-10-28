#include <mvdtool/mvd_generic.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

using namespace pybind11::literals;
using namespace MVD;
using namespace MVD3;
using namespace TSV;

class PyFile : public File {
    using File::File;

    void readTSVInfo(const std::string & ) override {
        PYBIND11_OVERLOAD_PURE_NAME(void, File, "read_tsv_info", readTSVInfo);
    }
    size_t getNbNeuron() const override {
        PYBIND11_OVERLOAD_PURE_NAME(size_t, File, "__len__", getNbNeuron);
    }
    Positions getPositions(const Range & = Range(0,0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(Positions, File, "positions", getPositions);
    }
    Rotations getRotations(const Range & = Range(0,0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(Rotations, File,"rotations", getRotations);
    }
    bool hasRotations() const override {
        PYBIND11_OVERLOAD_PURE_NAME(bool, File, "rotated", hasRotations);
    }
    std::vector<std::string> getMorphologies(const Range& = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<std::string>, File, "morphologies", getMorphologies);
    }
    std::vector<std::string> getEtypes(const Range& = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<std::string>, File, "etypes", getEtypes);
    }
    std::vector<std::string> getMtypes(const Range& = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<std::string>, File, "mtypes", getMtypes);
    }
    std::vector<std::string> getEmodels(const Range& = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<std::string>, File, "emodels", getEmodels);
    }
    std::vector<std::string> getRegions(const Range& = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<std::string>, File, "regions", getRegions);
    }
    std::vector<std::string> getSynapseClass(const Range& = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<std::string>, File, "synapse_classes", getSynapseClass);
    }
    bool hasCurrents() const override {
        PYBIND11_OVERLOAD_PURE_NAME(bool, File, "hasCurrents", hasCurrents);
    }
    std::vector<double> getThresholdCurrents(const Range& = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<double>, File, "threshold_currents", getThresholdCurrents);
    }
    std::vector<double> getHoldingCurrents(const Range& = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<double>, File, "holding_currents", getHoldingCurrents);
    }
    std::vector<size_t> getIndexEtypes(const Range& = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<size_t>, File, "raw_etypes", getIndexEtypes);
    }
    std::vector<size_t> getIndexMtypes(const Range& = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<size_t>, File, "raw_mtypes", getIndexMtypes);
    }
    std::vector<size_t> getIndexRegions(const Range& = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<size_t>, File, "raw_regions", getIndexRegions);
    }
    std::vector<size_t> getIndexSynapseClass(const Range& = Range(0, 0)) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<size_t>, File, "raw_synapse_classes", getIndexSynapseClass);
    }
    std::vector<std::string> listAllEtypes() const {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<std::string>, File, "all_etypes", listAllEtypes);
    }
    std::vector<std::string> listAllMtypes() const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<std::string>, File, "all_mtypes", listAllMtypes);
    }
    std::vector<std::string> listAllEmodels() const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<std::string>, File, "all_emodels", listAllEmodels);
    }
    std::vector<std::string> listAllRegions() const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<std::string>, File, "all_regions", listAllRegions);
    }
    std::vector<std::string> listAllSynapseClass() const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<std::string>, File, "all_synapse_classes", listAllSynapseClass);
    }

};


namespace {  // Intenal

template <typename T>
using pyarray = py::array_t<T, py::array::c_style | py::array::forcecast>;

/**
 * Extract several elements given their indices from a function accepting a range
 * NOTE: Indexes must be in ascending order
 */
template <typename T, typename FuncT>
inline std::vector<T> _atIndices(const FuncT& f,
                                 const size_t n_records,
                                 const pyarray<size_t>& idx) {
    const size_t CHUNK_SIZE = 256;
    const auto indices = idx.template unchecked<1>();
    const auto count = static_cast<size_t>(indices.size());
    std::vector<T> v(count);

    size_t offset = 0;
    std::vector<T> chunk;

    for (size_t i=0; i < count; i++) {
        const size_t elem_i = indices[i];
        if(elem_i - offset >= chunk.size()) {
            offset = elem_i;
            chunk = f(Range(offset, std::min(CHUNK_SIZE, n_records - offset)));
        }
        v[i] = chunk[elem_i - offset];
    }
    return v;
}

} // namespace (unnamed)


PYBIND11_MODULE(mvdtool, mvd) {
    mvd.doc() = "Module to read neuron circuits";
    py::module mvd3 = mvd.def_submodule("MVD3", "Support for the MVD3 format");
    py::module sonata = mvd.def_submodule("sonata", "Support for the SONATA format");
    py::module tsv = mvd.def_submodule("tsv", "Support for the TSV format");

    mvd.def("open", &open, "filename"_a, "population"_a = "");

    py::class_<File, PyFile> file(mvd, "__File");
    file
        .def(py::init<>())
        .def("__len__", &File::size)
        .def("read_tsv_info", [](File& f, const std::string & filename) {
                f.readTSVInfo(filename);
             },
             "filename"_a)
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
        .def("etypes", [](const File& f, pyarray<size_t> idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getEtypes(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def("mtypes", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getMtypes(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("mtypes", [](const File& f, pyarray<size_t> idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getMtypes(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def("emodels", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getEmodels(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("emodels", [](const File& f, pyarray<size_t> idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getEmodels(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def("threshold_currents", [](const File& f, int offset, int count) {
                auto res = f.getThresholdCurrents(Range(offset, count));
                return py::array(res.size(), res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("threshold_currents", [](const File& f, pyarray<size_t> idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getThresholdCurrents(r);};
                auto values = _atIndices<double>(func, f.size(), idx);
                return py::array(values.size(), values.data());
             })
        .def("holding_currents", [](const File& f, int offset, int count) {
                auto res = f.getHoldingCurrents(Range(offset, count));
                return py::array(res.size(), res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("holding_currents", [](const File& f, pyarray<size_t> idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getHoldingCurrents(r);};
                auto values = _atIndices<double>(func, f.size(), idx);
                return py::array(values.size(), values.data());
             })
        .def("morphologies", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getMorphologies(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("morphologies", [](const File& f, pyarray<size_t> idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getMorphologies(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def("synapse_classes", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getSynapseClass(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("synapse_classes", [](const File& f, pyarray<size_t> idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getSynapseClass(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def("regions", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getRegions(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("regions", [](const File& f, pyarray<size_t> idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getRegions(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
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
        .def("hasCurrents", &File::hasCurrents)
        .def_property_readonly("rotated", &File::hasRotations)
        .def_property_readonly("all_etypes", &File::listAllEtypes)
        .def_property_readonly("all_mtypes", &File::listAllMtypes)
        .def_property_readonly("all_emodels", &File::listAllEmodels)
        .def_property_readonly("all_regions", &File::listAllRegions)
        .def_property_readonly("all_synapse_classes", &File::listAllSynapseClass)
        ;

    py::class_<MVD3File, std::shared_ptr<MVD3File>>(mvd3, "File", file)
        .def(py::init<const std::string&>())
        .def("raw_morphologies", [](const MVD3File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getIndexMorphologies(r);
                return py::array(res.size(), res.data());
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("me_combos", [](const MVD3File& f, int offset, int count) {
                Range r(offset, count);
                return f.getMECombos(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("me_combos", [](const MVD3File& f, pyarray<size_t> idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getMECombos(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def("layers", [](const MVD3File& f, int offset, int count) {
                return f.getLayers(Range(offset, count));
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("layers", [](const MVD3File& f, pyarray<size_t> idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getLayers(r);};
                return _atIndices<boost::int32_t>(func, f.size(), idx);
             })
        .def_property_readonly("all_layers", &MVD3File::listAllLayers)
        .def_property_readonly("all_morphologies", &MVD3File::listAllMorphologies)
        ;

    py::class_<SonataFile, std::shared_ptr<SonataFile>>(sonata, "File", file)
        .def(py::init<const std::string&>())
        .def("layers", [](const SonataFile& f, int offset, int count) {
                return f.getLayers(Range(offset, count));
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("layers", [](const SonataFile& f, pyarray<size_t> idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getLayers(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def_property_readonly("all_layers", &SonataFile::listAllLayers)
        ;
    py::class_<TSVFile, std::shared_ptr<TSVFile>>(tsv, "File", file)
        .def(py::init<const std::string&>())
        ;
    py::class_<TSVInfo>(tsv, "TSVInfo")
        .def(py::init<>())
        .def_readonly("morphologyName", &TSVInfo::morphologyName)
        .def_readonly("layer", &TSVInfo::layer)
        .def_readonly("fullMType", &TSVInfo::fullMType)
        .def_readonly("eType", &TSVInfo::eType)
        .def_readonly("eModel", &TSVInfo::eModel)
        .def_readonly("comboName", &TSVInfo::comboName)
        .def_readonly("thresholdCurrent", &TSVInfo::thresholdCurrent)
        .def_readonly("holdingCurrent", &TSVInfo::holdingCurrent)
        ;
}
