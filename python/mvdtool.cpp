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

    void openComboTsv(const std::string & ) override {
        PYBIND11_OVERLOAD_PURE_NAME(void, File, "open_combo_tsv", openComboTsv);
    }
    size_t getNbNeuron() const override {
        PYBIND11_OVERLOAD_PURE_NAME(size_t, File, "__len__", getNbNeuron);
    }
    Positions getPositions(const Range & = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(Positions, File, "positions", getPositions);
    }
    Rotations getRotations(const Range & = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(Rotations, File,"rotations", getRotations);
    }
    bool hasRotations() const override {
        PYBIND11_OVERLOAD_PURE_NAME(bool, File, "rotated", hasRotations);
    }
    std::vector<std::string> getMorphologies(const Range& = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<std::string>, File, "morphologies", getMorphologies);
    }
    std::vector<std::string> getEtypes(const Range& = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<std::string>, File, "etypes", getEtypes);
    }
    std::vector<std::string> getMtypes(const Range& = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<std::string>, File, "mtypes", getMtypes);
    }
    std::vector<std::string> getEmodels(const Range& = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<std::string>, File, "emodels", getEmodels);
    }
    std::vector<std::string> getRegions(const Range& = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<std::string>, File, "regions", getRegions);
    }
    std::vector<std::string> getSynapseClass(const Range& = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<std::string>, File, "synapse_classes", getSynapseClass);
    }
    bool hasMiniFrequencies() const override {
        PYBIND11_OVERLOAD_PURE_NAME(bool, File, "hasMiniFrequencies", hasMiniFrequencies);
    }
    std::vector<double> getExcMiniFrequencies(const Range& = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<double>, File, "exc_mini_frequencies", getExcMiniFrequencies);
    }
    std::vector<double> getInhMiniFrequencies(const Range& = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<double>, File, "inh_mini_frequencies", getInhMiniFrequencies);
    }
    bool hasCurrents() const override {
        PYBIND11_OVERLOAD_PURE_NAME(bool, File, "hasCurrents", hasCurrents);
    }
    std::vector<double> getThresholdCurrents(const Range& = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<double>, File, "threshold_currents", getThresholdCurrents);
    }
    std::vector<double> getHoldingCurrents(const Range& = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<double>, File, "holding_currents", getHoldingCurrents);
    }
    std::vector<size_t> getIndexEtypes(const Range& = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<size_t>, File, "raw_etypes", getIndexEtypes);
    }
    std::vector<size_t> getIndexMtypes(const Range& = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<size_t>, File, "raw_mtypes", getIndexMtypes);
    }
    std::vector<size_t> getIndexRegions(const Range& = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(std::vector<size_t>, File, "raw_regions", getIndexRegions);
    }
    std::vector<size_t> getIndexSynapseClass(const Range& = Range::all()) const override {
        PYBIND11_OVERLOAD_PURE_NAME(
            std::vector<size_t>, File, "raw_synapse_classes", getIndexSynapseClass);
    }
    std::vector<std::string> listAllEtypes() const override{
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


/// Generic routine to copy data item
template <typename T>
inline void copy_element(T& dst, const T& src) {
    dst = src;
}

/// Copy routine for boost multi_array view to std::array
template <typename T, size_t Width, typename T2, typename = decltype(&T2::origin)>
inline void copy_element(std::array<T, Width>& dst, const T2& src) {
    std::memcpy(dst.data(), src.origin(), sizeof(T) * Width);
}

/**
 * Extract several elements given their indices from a function accepting a range
 * NOTE: Indexes must be in ascending order
 */
template <typename T, typename FuncT>
inline std::vector<T>_atIndices(const FuncT& f,
                                const size_t n_records,
                                const pyarray<size_t>& idx) {
    constexpr size_t CHUNK_SIZE = 128u;  // 1KB of doubles, more for strings
    const auto indices = idx.template unchecked<1>();
    const auto count = static_cast<size_t>(indices.size());
    std::vector<T> out_v(count);

    for (size_t i=0; i < count;) {
        const size_t offset = indices[i];
        const auto limit = std::min(CHUNK_SIZE, n_records - offset);
        const auto& chunk = f(Range(offset, limit));
        const auto high_i = offset + limit;
        for(size_t elem_i=offset; i < count && (elem_i=indices[i]) < high_i; ++i) {
            copy_element(out_v[i], chunk[elem_i - offset]);
        }
    }
    return out_v;
}

} // namespace (unnamed)


PYBIND11_MODULE(mvdtool, mvd) {
    mvd.doc() = "Module to read neuron circuits";
    py::module mvd3 = mvd.def_submodule("MVD3", "Support for the MVD3 format");
    py::module sonata = mvd.def_submodule("sonata", "Support for the SONATA format");
    py::module tsv = mvd.def_submodule("tsv", "Support for the TSV format");

    mvd.def("open", &open, "filename"_a, "population"_a = "");
    constexpr size_t POSITION_WIDTH = 3;
    constexpr size_t ROTATION_WIDTH = 4;

    py::class_<File, PyFile> file(mvd, "__File");
    file
        .def(py::init<>())
        .def("__len__", &File::size)
        .def("open_combo_tsv", [](File& f, const std::string & filename) {
                f.openComboTsv(filename);
             })
        .def("positions", [](const File& f) {
                auto res = f.getPositions(Range::all());
                return py::array({res.shape()[0], POSITION_WIDTH}, res.data());
             })
        .def("positions", [](const File& f, int offset) {
                Range r(offset, 1);
                auto res = f.getPositions(r);
                return py::array({res.shape()[0], POSITION_WIDTH}, res.data());
             })
        .def("positions", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getPositions(r);
                return py::array({res.shape()[0], POSITION_WIDTH}, res.data());
             })
        .def("positions", [](const File& f, const pyarray<size_t>& idx) {
                using position_t = std::array<typename Positions::element, POSITION_WIDTH>;
                const auto& func = [&f](const MVD::Range& r){return f.getPositions(r);};
                const auto res = _atIndices<position_t>(func, f.size(), idx);
                return py::array({res.size(), POSITION_WIDTH}, res.empty()? nullptr: res.front().data());
             })
        .def("rotations", [](const File& f) {
                auto res = f.getRotations(Range::all());
                return py::array({res.shape()[0], ROTATION_WIDTH}, res.data());
             })
        .def("rotations", [](const File& f, int offset) {
                Range r(offset, 1);
                auto res = f.getRotations(r);
                return py::array({res.shape()[0], ROTATION_WIDTH}, res.data());
             })
        .def("rotations", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getRotations(r);
                return py::array({res.shape()[0], ROTATION_WIDTH}, res.data());
             })
        .def("rotations", [](const File& f, const pyarray<size_t>& idx) {
                using rotation_t = std::array<typename Rotations::element, ROTATION_WIDTH>;
                const auto& func = [&f](const MVD::Range& r){return f.getRotations(r);};
                const auto res = _atIndices<rotation_t>(func, f.size(), idx);
                return py::array({res.size(), ROTATION_WIDTH}, res.empty()? nullptr: res.front().data());
             })
        .def("etypes", [](const File& f) {
                return f.getEtypes(Range::all());
             })
        .def("etypes", [](const File& f, int offset) {
                Range r(offset, 1);
                return f.getEtypes(r)[0];
             })
        .def("etypes", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getEtypes(r);
             })
        .def("etypes", [](const File& f, const pyarray<size_t>& idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getEtypes(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def("mtypes", [](const File& f) {
                return f.getMtypes(Range::all());
             })
        .def("mtypes", [](const File& f, int offset) {
                Range r(offset, 1);
                return f.getMtypes(r)[0];
             })
        .def("mtypes", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getMtypes(r);
             })
        .def("mtypes", [](const File& f, const pyarray<size_t>& idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getMtypes(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def("emodels", [](const File& f) {
                return f.getEmodels(Range::all());
             })
        .def("emodels", [](const File& f, int offset) {
                Range r(offset, 1);
                return f.getEmodels(r)[0];
             })
        .def("emodels", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getEmodels(r);
             })
        .def("emodels", [](const File& f, const pyarray<size_t>& idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getEmodels(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def("threshold_currents", [](const File& f) {
                auto res = f.getThresholdCurrents(Range::all());
                return py::array(res.size(), res.data());
             })
        .def("threshold_currents", [](const File& f, int offset) {
                Range r(offset, 1);
                return f.getThresholdCurrents(r)[0];
             })
        .def("threshold_currents", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getThresholdCurrents(r);
                return py::array(res.size(), res.data());
             })
        .def("threshold_currents", [](const File& f, const pyarray<size_t>& idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getThresholdCurrents(r);};
                auto values = _atIndices<double>(func, f.size(), idx);
                return py::array(values.size(), values.data());
             })
        .def("holding_currents", [](const File& f) {
                auto res = f.getHoldingCurrents(Range::all());
                return py::array(res.size(), res.data());
             })
        .def("holding_currents", [](const File& f, int offset) {
                Range r(offset, 1);
                return f.getHoldingCurrents(r)[0];
             })
        .def("holding_currents", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getHoldingCurrents(r);
                return py::array(res.size(), res.data());
             })
        .def("holding_currents", [](const File& f, const pyarray<size_t>& idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getHoldingCurrents(r);};
                auto values = _atIndices<double>(func, f.size(), idx);
                return py::array(values.size(), values.data());
             })
        .def("morphologies", [](const File& f) {
                return f.getMorphologies(Range::all());
             })
        .def("morphologies", [](const File& f, int offset) {
                Range r(offset, 1);
                return f.getMorphologies(r)[0];
             })
        .def("morphologies", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getMorphologies(r);
             })
        .def("morphologies", [](const File& f, const pyarray<size_t>& idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getMorphologies(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def("synapse_classes", [](const File& f) {
                return f.getSynapseClass(Range::all());
             })
        .def("synapse_classes", [](const File& f, int offset) {
                Range r(offset, 1);
                return f.getSynapseClass(r)[0];
             })
        .def("synapse_classes", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getSynapseClass(r);
             })
        .def("synapse_classes", [](const File& f, const pyarray<size_t>& idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getSynapseClass(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def("exc_mini_frequencies", [](const File& f) {
                auto res = f.getExcMiniFrequencies(Range::all());
                return py::array(res.size(), res.data());
             })
        .def("exc_mini_frequencies", [](const File& f, int offset) {
                Range r(offset, 1);
                return f.getExcMiniFrequencies(r)[0];
             })
        .def("exc_mini_frequencies", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getExcMiniFrequencies(r);
                return py::array(res.size(), res.data());
             })
        .def("exc_mini_frequencies", [](const File& f, const pyarray<size_t>& idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getExcMiniFrequencies(r);};
                auto values = _atIndices<double>(func, f.size(), idx);
                return py::array(values.size(), values.data());
             })
        .def("inh_mini_frequencies", [](const File& f) {
                auto res = f.getInhMiniFrequencies(Range::all());
                return py::array(res.size(), res.data());
             })
        .def("inh_mini_frequencies", [](const File& f, int offset) {
                Range r(offset, 1);
                return f.getInhMiniFrequencies(r)[0];
             })
        .def("inh_mini_frequencies", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getInhMiniFrequencies(r);
                return py::array(res.size(), res.data());
             })
        .def("inh_mini_frequencies", [](const File& f, const pyarray<size_t>& idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getInhMiniFrequencies(r);};
                auto values = _atIndices<double>(func, f.size(), idx);
                return py::array(values.size(), values.data());
             })
        .def("regions", [](const File& f) {
                return f.getRegions(Range::all());
             })
        .def("regions", [](const File& f, int offset) {
                Range r(offset, 1);
                return f.getRegions(r)[0];
             })
        .def("regions", [](const File& f, int offset, int count) {
                Range r(offset, count);
                return f.getRegions(r);
             },
             "offset"_a = 0,
             "count"_a = 0)
        .def("regions", [](const File& f, const pyarray<size_t>& idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getRegions(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def("raw_etypes", [](const File& f) {
                auto res = f.getIndexEtypes(Range::all());
                return py::array(res.size(), res.data());
             })
        .def("raw_etypes", [](const File& f, int offset) {
                Range r(offset, 1);
                return f.getIndexEtypes(r)[0];
             })
        .def("raw_etypes", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getIndexEtypes(r);
                return py::array(res.size(), res.data());
             })
        .def("raw_mtypes", [](const File& f) {
                auto res = f.getIndexMtypes(Range::all());
                return py::array(res.size(), res.data());
             })
        .def("raw_mtypes", [](const File& f, int offset) {
                Range r(offset, 1);
                return f.getIndexMtypes(r)[0];
             })
        .def("raw_mtypes", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getIndexMtypes(r);
                return py::array(res.size(), res.data());
             })
        .def("raw_synapse_classes", [](const File& f) {
                auto res = f.getIndexSynapseClass(Range::all());
                return py::array(res.size(), res.data());
             })
        .def("raw_synapse_classes", [](const File& f, int offset) {
                Range r(offset, 1);
                return f.getIndexSynapseClass(r)[0];
             })
        .def("raw_synapse_classes", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getIndexSynapseClass(r);
                return py::array(res.size(), res.data());
             })
        .def("raw_regions", [](const File& f) {
                auto res = f.getIndexRegions(Range::all());
                return py::array(res.size(), res.data());
             })
        .def("raw_regions", [](const File& f, int offset) {
                Range r(offset, 1);
                return f.getIndexRegions(r)[0];
             })
        .def("raw_regions", [](const File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getIndexRegions(r);
                return py::array(res.size(), res.data());
             })
        .def("hasMiniFrequencies", &File::hasMiniFrequencies)
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
        .def("raw_morphologies", [](const MVD3File& f) {
                auto res = f.getIndexMorphologies(Range::all());
                return py::array(res.size(), res.data());
             })
        .def("raw_morphologies", [](const MVD3File& f, int offset) {
                Range r(offset, 1);
                return f.getIndexMorphologies(r)[0];
             })
        .def("raw_morphologies", [](const MVD3File& f, int offset, int count) {
                Range r(offset, count);
                auto res = f.getIndexMorphologies(r);
                return py::array(res.size(), res.data());
             })
        .def("me_combos", [](const MVD3File& f) {
                return f.getMECombos(Range::all());
             })
        .def("me_combos", [](const MVD3File& f, int offset) {
                Range r(offset, 1);
                return f.getMECombos(r)[0];
             })
        .def("me_combos", [](const MVD3File& f, int offset, int count) {
                Range r(offset, count);
                return f.getMECombos(r);
             })
        .def("me_combos", [](const MVD3File& f, const pyarray<size_t>& idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getMECombos(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def("layers", [](const MVD3File& f) {
                return f.getLayers(Range::all());
             })
        .def("layers", [](const MVD3File& f, int offset) {
                Range r(offset, 1);
                return f.getLayers(r)[0];
             })
        .def("layers", [](const MVD3File& f, int offset, int count) {
                Range r(offset, count);
                return f.getLayers(r);
             })
        .def("layers", [](const MVD3File& f, const pyarray<size_t>& idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getLayers(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def_property_readonly("all_morphologies", &MVD3File::listAllMorphologies)
        ;

    py::class_<SonataFile, std::shared_ptr<SonataFile>>(sonata, "File", file)
        .def(py::init<const std::string&>())
        .def("layers", [](const SonataFile& f) {
                return f.getLayers(Range::all());
             })
        .def("layers", [](const SonataFile& f, int offset) {
                Range r(offset, 1);
                return f.getLayers(r)[0];
             })
        .def("layers", [](const SonataFile& f, int offset, int count) {
                Range r(offset, count);
                return f.getLayers(r);
             })
        .def("layers", [](const SonataFile& f, const pyarray<size_t>& idx) {
                const auto& func = [&f](const MVD::Range& r){return f.getLayers(r);};
                return _atIndices<std::string>(func, f.size(), idx);
             })
        .def_property_readonly("all_layers", &SonataFile::listAllLayers)
        .def("hasAttribute", [](const SonataFile& f, const std::string& name){
                return f.hasAttribute(name) || f.hasDynamicsAttribute(name);
             })
        .def("getAttribute", [](const SonataFile& f, const std::string& name) {
                const std::string dtype = f.getAttributeDataType(name);
                if (dtype == "double" || dtype == "float") {
                    auto res = f.getAttribute<double>(name);
                    return py::array(res.size(), res.data());
                } else if (dtype == "string") {
                    auto res = f.getAttribute<std::string>(name);
                    return py::array(py::cast(res));
                } else {
                    auto res = f.getAttribute<size_t>(name);
                    return py::array(res.size(), res.data());
                }
             })
        .def("getAttribute", [](const SonataFile& f, const std::string& name, int offset, int count){
                const std::string dtype = f.getAttributeDataType(name);
                Range r(offset, count);
                if (dtype == "string") {
                    auto res = f.getAttribute<std::string>(name, r);
                    return py::array(py::cast(res));
                } else if (dtype == "double" || dtype == "float") {
                    auto res = f.getAttribute<double>(name, r);
                    return py::array(res.size(), res.data());
                } else {
                    auto res = f.getAttribute<int>(name, r);
                    return py::array(res.size(), res.data());
                }
             })
        .def("getAttribute", [](const SonataFile& f, const std::string& name, const pyarray<size_t>& idx) {
                const std::string dtype = f.getAttributeDataType(name);
                if (dtype == "string") {
                    const auto& func = [&f, &name](const MVD::Range& r) {return f.getAttribute<std::string>(name, r);};
                    auto res = _atIndices<std::string>(func, f.size(), idx);
                    return py::array(py::cast(res));
                } else if (dtype == "double" || dtype == "float") {
                    const auto& func = [&f, &name](const MVD::Range& r) {return f.getAttribute<double>(name, r);};
                    auto res = _atIndices<double>(func, f.size(), idx);
                    return py::array(res.size(), res.data());
                } else {
                    const auto& func = [&f, &name](const MVD::Range& r) {return f.getAttribute<size_t>(name, r);};
                    auto res = _atIndices<size_t>(func, f.size(), idx);
                    return py::array(res.size(), res.data());
                }
             })
        ;
    py::class_<TSVFile, std::shared_ptr<TSVFile>>(tsv, "File", file)
        .def(py::init<const std::string&>())
        ;
    py::class_<MEComboEntry>(tsv, "MEComboEntry")
        .def(py::init<>())
        .def_readonly("morphologyName", &MEComboEntry::morphologyName)
        .def_readonly("layer", &MEComboEntry::layer)
        .def_readonly("fullMType", &MEComboEntry::fullMType)
        .def_readonly("eType", &MEComboEntry::eType)
        .def_readonly("eModel", &MEComboEntry::eModel)
        .def_readonly("comboName", &MEComboEntry::comboName)
        .def_readonly("thresholdCurrent", &MEComboEntry::thresholdCurrent)
        .def_readonly("holdingCurrent", &MEComboEntry::holdingCurrent)
        ;
}
