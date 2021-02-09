"""Generic MVD3 read tests
"""
from os import path

import numpy
import pytest

import mvdtool as mt

_dir, _ = path.split(__file__)


@pytest.fixture(params=["circuit.mvd3", "sonata.h5", "sonata_alt.h5"])
def circuit(request):
    """Provide access to an circuit file
    """
    filename = path.join(_dir, request.param)
    return mt.open(filename)


def test_number_neurons(circuit):
    assert len(circuit) == 1000


def test_position_values(circuit):
    posics = circuit.positions()
    assert posics.shape[0] == 1000
    assert numpy.allclose(posics[0], [40.821_401, 1986.506_637, 10.788_424])

    posics = circuit.positions(0, 0)
    assert posics.shape[0] == 1000
    assert numpy.allclose(posics[0], [40.821_401, 1986.506_637, 10.788_424])

    posic = circuit.positions(995)
    assert posic.shape[0] == 1
    assert numpy.allclose(posic, posics[995])

    posics_995 = circuit.positions(995, 0)
    assert posics_995.shape[0] == 5
    assert numpy.allclose(posics_995[0], posics[995])

    posics_995 = circuit.positions(995, 1)
    assert posics_995.shape[0] == 1
    assert numpy.allclose(posics_995[0], posics[995])

    posics_995 = circuit.positions(995, 3)
    assert posics_995.shape[0] == 3
    assert numpy.allclose(posics_995[2], posics[997])


def test_position_indices(circuit):
    empty_posics = circuit.positions([])
    posics_multi = circuit.positions([2, 995, 996, 997])
    posics = circuit.positions(0, 0)
    assert posics_multi.shape == (4, 3)
    assert numpy.allclose(posics_multi[0], posics[2])
    assert numpy.allclose(posics_multi[1], posics[995])
    assert numpy.allclose(posics_multi[2], posics[996])
    assert numpy.allclose(posics_multi[3], posics[997])


def test_position_value(circuit):
    posic_0 = circuit.positions(0)
    assert posic_0.shape[0] == 1
    assert numpy.allclose(posic_0, [40.821_401, 1986.506_637, 10.788_424])


def test_rotation_values(circuit):
    assert circuit.rotated

    posics = circuit.rotations()
    assert posics.shape[0] == 1000
    assert numpy.allclose(posics[0], [0, -0.010_005_561, 0, 0.999_949_943])
    assert numpy.allclose(posics[20], [0, 0.923_706, 0, 0.383_102])


def test_rotation_value(circuit):
    assert circuit.rotated

    posic_0 = circuit.rotations(0)
    assert posic_0.shape[0] == 1
    assert numpy.allclose(posic_0, [0, -0.010_005_561, 0, 0.999_949_943])

def test_rotation_indices(circuit):
    assert circuit.rotated

    posic_multi_rot = circuit.rotations([0, 20])
    assert posic_multi_rot.shape[0] == 2
    assert numpy.allclose(posic_multi_rot[0], [0, -0.010_005_561, 0, 0.999_949_943])
    assert numpy.allclose(posic_multi_rot[1], [0, 0.923_706, 0, 0.383_102])


def test_position_range(circuit):
    posics = circuit.positions()
    posics_range = circuit.positions(10, 10)

    assert len(posics_range) == 10
    assert numpy.allclose(posics_range, posics[10:20])


def test_morphology_values(circuit):
    morphos = circuit.morphologies()

    assert morphos[0] == "sm090227a1-2_idC"
    assert morphos[20] == "dend-C280998A-P3_axon-sm110131a1-3_INT_idA"
    assert morphos[21] == "dend-rr110114C1_idA_axon-sm110131a1-3_INT_idA"


def test_morphology_value(circuit):
    morpho = circuit.morphologies(0)

    assert morpho == "sm090227a1-2_idC"


def test_morphology_values_indices(circuit):
    morphos = circuit.morphologies([0, 20, 21])
    assert len(morphos) == 3

    assert morphos[0] == "sm090227a1-2_idC"
    assert morphos[1] == "dend-C280998A-P3_axon-sm110131a1-3_INT_idA"
    assert morphos[2] == "dend-rr110114C1_idA_axon-sm110131a1-3_INT_idA"


def test_etype_values(circuit):
    etypes = circuit.etypes()

    assert etypes[0] == "cACint"
    assert etypes[10] == "cACint"
    assert etypes[20] == "cADpyr"


def test_etype_values_indices(circuit):
    etypes = circuit.etypes([0, 10, 20])

    assert etypes[0] == "cACint"
    assert etypes[1] == "cACint"
    assert etypes[2] == "cADpyr"


def test_etype_values_ranges(circuit):
    etypes = circuit.etypes()
    assert len(etypes) == 1000
    assert etypes[0] == "cACint"

    etypes = circuit.etypes(0, 0)
    assert len(etypes) == 1000
    assert etypes[0] == "cACint"

    etype = circuit.etypes(20)
    assert type(etype) is str
    assert etype == etypes[20]

    etypes_20 = circuit.etypes(20, 0)
    assert len(etypes_20) == 980
    assert etypes_20[0] == etypes[20]

    etypes_20 = circuit.etypes(20, 1)
    assert type(etypes_20) is not str
    assert len(etypes_20) == 1
    assert etypes_20[0] == etypes[20]

    etypes_20 = circuit.etypes(20, 10)
    assert len(etypes_20) == 10
    assert etypes_20[0] == etypes[20]


def test_mtype_values(circuit):
    mtypes = circuit.mtypes()

    assert mtypes[0] == "L1_SLAC"
    assert mtypes[10] == "L1_SLAC"
    assert mtypes[20] == "L23_PC"


def test_region_values(circuit):
    regions = circuit.regions()

    assert regions[1] == "L1"
    assert regions[9] == "L2"
    assert regions[10] == "L3"
    assert regions[15] == "L23"


def test_classes_values(circuit):
    classes = circuit.synapse_classes()

    assert classes[0] == "INH"
    assert classes[10] == "INH"
    assert classes[20] == "EXC"


def test_raw_etype(circuit):
    raw_etype = circuit.raw_etypes(22)

    assert raw_etype == 1

@pytest.fixture(params=["circuit_tsv.mvd3", "sonata.h5"])
def circuit_without_freq(request):
    """Provide access to a circuit file
    """
    filename = path.join(_dir, request.param)
    return mt.open(filename)

def test_has_mini_frequencies(circuit_without_freq):

    assert circuit_without_freq.hasMiniFrequencies() == False

@pytest.fixture(params=["circuit.mvd3", "nodes.h5"])
def circuit_with_freq(request):
    """Provide access to a circuit file
    """
    filename = path.join(_dir, request.param)
    return mt.open(filename)


def test_mini_frequencies(circuit_with_freq):

    assert circuit_with_freq.hasMiniFrequencies()
    exc_mini_frequencies = circuit_with_freq.exc_mini_frequencies()
    assert exc_mini_frequencies[0] == 0.63
    assert circuit_with_freq.exc_mini_frequencies(0) == 0.63
    assert exc_mini_frequencies[100] == 0.122
    assert circuit_with_freq.exc_mini_frequencies(95, 6)[5] == 0.122
    assert exc_mini_frequencies[200] == 0.04
    assert numpy.array_equal(circuit_with_freq.exc_mini_frequencies(
        [0, 100, 200]), [0.63, 0.122, 0.04])

    inh_mini_frequencies = circuit_with_freq.inh_mini_frequencies()
    assert inh_mini_frequencies[0] == 0.012
    assert circuit_with_freq.inh_mini_frequencies(0) == 0.012
    assert inh_mini_frequencies[100] == 0.013
    assert circuit_with_freq.inh_mini_frequencies(95, 6)[5] == 0.013
    assert inh_mini_frequencies[200] == 0.014
    assert numpy.array_equal(circuit_with_freq.inh_mini_frequencies(
        [0, 100, 200]), [0.012, 0.013, 0.014])


@pytest.mark.parametrize(
    "attr", ["morphologies", "etypes", "mtypes", "regions", "synapse_classes"]
)
def test_ranges(attr, circuit):
    if attr == "morphologies" and not isinstance(circuit, mt.MVD3.File):
        return

    OFFSET = 10
    values = getattr(circuit, attr)()
    indices = getattr(circuit, f"all_{attr}")
    ranged = getattr(circuit, f"raw_{attr}")(OFFSET, 5)

    for n, i in enumerate(ranged):
        assert values[n + OFFSET] == indices[i]


def test_population():
    filename = path.join(_dir, "sonata.h5")
    circuit = mt.open(filename, "truncated")

    assert len(circuit) == 10

    with pytest.raises(RuntimeError):
        circuit = mt.open(filename, "non-existent")


@pytest.fixture
def mvd3_file():
    return path.join(_dir, "circuit_tsv.mvd3")


@pytest.fixture
def tsv_file():
    return path.join(_dir, "mecombo_emodel.tsv")


@pytest.fixture
def tsv_file_tabs():
    return path.join(_dir, "mecombo_emodel_tabs.tsv")


def circuit_mvd3_tsv(mvd3_file, tsv_file):
    mvd3filename = path.join(_dir, mvd3_file)
    tsvfilename = path.join(_dir, tsv_file)
    mvd3file = mt.open(mvd3filename)
    mvd3file.open_combo_tsv(tsvfilename)
    return mvd3file


def test_circuit_mvd3_tsv(mvd3_file, tsv_file):
    circuit = circuit_mvd3_tsv(mvd3_file, tsv_file)
    _test_tsv_layer(circuit)
    _test_tsv_emodels(circuit)
    _test_tsv_emodels_ranges(circuit)
    _test_tsv_emodels_indices(circuit)
    _test_mvd3_mecombos_value(circuit)
    _test_mvd3_mecombos_indices(circuit)
    _test_tsv_threshold_current(circuit)
    _test_tsv_holding_current(circuit)
    _test_tsv_pybind_api(circuit)


def test_circuit_mvd3_tsv_tabs(mvd3_file, tsv_file_tabs):
    test_circuit_mvd3_tsv(mvd3_file, tsv_file_tabs)


def _test_tsv_layer(circuit_mvd3_tsv):
    layers = circuit_mvd3_tsv.layers()

    assert layers[0] == "1"
    assert layers[9] == "1"
    assert layers[33] == "6"


def _test_tsv_emodels(circuit_mvd3_tsv):
    emodels = circuit_mvd3_tsv.emodels()

    assert emodels[0] == "bAC_327962063"
    assert emodels[9] == "dSTUT_321707905"
    assert emodels[33] == "L6_cADpyr_471819401"


def _test_tsv_emodels_ranges(circuit_mvd3_tsv):
    emodels = circuit_mvd3_tsv.emodels()
    assert len(emodels) == 34
    assert emodels[0] == "bAC_327962063"

    emodels = circuit_mvd3_tsv.emodels(0, 0)
    assert len(emodels) == 34
    assert emodels[0] == "bAC_327962063"

    emodel = circuit_mvd3_tsv.emodels(9)
    assert type(emodel) is str
    assert emodel == emodels[9]

    emodels_9 = circuit_mvd3_tsv.emodels(9, 0)
    assert len(emodels_9) == 25
    assert emodels_9[0] == emodels[9]

    emodels_9 = circuit_mvd3_tsv.emodels(9, 1)
    assert type(emodels_9) is not str
    assert len(emodels_9) == 1
    assert emodels_9[0] == emodels[9]

    emodels_9 = circuit_mvd3_tsv.emodels(9, 10)
    assert len(emodels_9) == 10
    assert emodels_9[0] == emodels[9]


def _test_tsv_emodels_indices(circuit_mvd3_tsv):
    emodels = circuit_mvd3_tsv.emodels([0, 9, 33])

    assert emodels[0] == "bAC_327962063"
    assert emodels[1] == "dSTUT_321707905"
    assert emodels[2] == "L6_cADpyr_471819401"


def _test_mvd3_mecombos_value(circuit_mvd3_tsv):
    me_combo = circuit_mvd3_tsv.me_combos(9)

    assert me_combo == "dSTUT_1_87dd39e6b0255ec053001f16da85b0e0"


def _test_mvd3_mecombos_indices(circuit_mvd3_tsv):
    me_combos = circuit_mvd3_tsv.me_combos([0, 9, 33])

    assert me_combos[0] == "bAC_1_02583f52ff47b88961e4216e2972ee8c"
    assert me_combos[1] == "dSTUT_1_87dd39e6b0255ec053001f16da85b0e0"
    assert me_combos[2] == "cADpyr_6_97957c6ebc6ac6397bf0fa077d39580c"


def _test_tsv_threshold_current(circuit_mvd3_tsv):
    threshold_currents = circuit_mvd3_tsv.threshold_currents()

    assert threshold_currents[0] == 0
    assert threshold_currents[9] == 0
    assert threshold_currents[33] == 0.2


def _test_tsv_holding_current(circuit_mvd3_tsv):
    holding_currents = circuit_mvd3_tsv.holding_currents()

    assert holding_currents[0] == 0
    assert holding_currents[9] == 0.1
    assert holding_currents[33] == 0.15


def _test_tsv_pybind_api(circuit_mvd3_tsv):
    holding_current = circuit_mvd3_tsv.holding_currents(0)

    assert type(holding_current) is float
    assert holding_current == 0

    holding_currents = circuit_mvd3_tsv.holding_currents(0, 10)

    assert holding_currents[9] == 0.1
    assert len(holding_currents) == 10

    holding_currents = circuit_mvd3_tsv.holding_currents()

    assert holding_currents[33] == 0.15
    assert len(holding_currents) == 34


@pytest.fixture
def circuit_new_sonata():
    sonatafilename = path.join(_dir, "nodes.h5")
    sonata = mt.open(sonatafilename)
    return sonata


def test_emodels(circuit_new_sonata):
    emodels = circuit_new_sonata.emodels()

    assert emodels[0] == "CA1_int_cAC_990611HP2_2019032816214"
    assert emodels[42] == "CA1_pyr_cACpyr_oh140807_A0_idJ_2019032814272"
    assert emodels[2615] == "CA1_int_cNAC_990111HP2_2019032915570"


def test_layers(circuit_new_sonata):
    layers = circuit_new_sonata.layers()

    assert layers[0] == "SR"
    assert layers[42] == "SP"
    assert layers[2615] == "SO"


def test_layers_ranges(circuit_new_sonata):
    layers = circuit_new_sonata.layers()
    assert len(layers) == 2616
    assert layers[0] == "SR"

    layers = circuit_new_sonata.layers(0, 0)
    assert len(layers) == 2616
    assert layers[0] == "SR"

    layer = circuit_new_sonata.layers(42)
    assert type(layer) is str
    assert layer == layers[42]

    layers_42 = circuit_new_sonata.layers(42, 0)
    assert len(layers_42) == 2574
    assert layers_42[0] == layers[42]

    layers_42 = circuit_new_sonata.layers(42, 1)
    assert type(layers_42) is not str
    assert len(layers_42) == 1
    assert layers_42[0] == layers[42]

    layers_42 = circuit_new_sonata.layers(42, 10)
    assert len(layers_42) == 10
    assert layers_42[0] == layers[20]


def test_layer(circuit_new_sonata):
    layer = circuit_new_sonata.layers(42)

    assert layer == "SP"
    assert type(layer) is str


def test_list_allLayers(circuit_new_sonata):
    allLayers = circuit_new_sonata.all_layers

    assert allLayers[0] == "SR"
    assert allLayers[1] == "SP"
    assert allLayers[2] == "SO"


def test_query_arbitrary_parameters(circuit_new_sonata):
    assert circuit_new_sonata.hasAttribute("abcd") == False
    assert circuit_new_sonata.hasAttribute("layer") == True
    assert circuit_new_sonata.hasAttribute("holding_current") == True
    assert circuit_new_sonata.getAttribute("layer")[0] == "SR"
    assert numpy.array_equal(circuit_new_sonata.getAttribute("layer", 2, 5),
                             ['SR', 'SR', 'SR', 'SP', 'SP'])
    assert numpy.array_equal(circuit_new_sonata.getAttribute("layer", [0, 100, 200]),
                             ['SR', 'SP', 'SP'])
    assert circuit_new_sonata.getAttribute("exc_mini_frequency")[0] == 0.63
    assert numpy.array_equal(circuit_new_sonata.getAttribute("exc_mini_frequency", 2, 5),
                             [0.63, 0.63, 0.63, 0.63, 0.63])
    assert numpy.array_equal(circuit_new_sonata.getAttribute("exc_mini_frequency", [0, 100, 200]),
                             [0.63, 0.122, 0.04])
    assert circuit_new_sonata.getAttribute("mtype")[0] == 10
    assert numpy.array_equal(circuit_new_sonata.getAttribute("mtype", 2, 5),
                             [10, 10, 10,  8,  8])
    assert numpy.array_equal(circuit_new_sonata.getAttribute("mtype", [0, 100, 200]),
                             [10,  8,  8])
    assert circuit_new_sonata.getAttribute("holding_current")[0] == pytest.approx(0.00582906)
    assert circuit_new_sonata.getAttribute("threshold_current")[0] == pytest.approx(0.33203125)
