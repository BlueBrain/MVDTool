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


def test_rotation_values(circuit):
    assert circuit.rotated

    posics = circuit.rotations()
    assert posics.shape[0] == 1000
    assert numpy.allclose(posics[0], [0, -0.010_005_561, 0, 0.999_949_943])
    assert numpy.allclose(posics[20], [0, 0.923_706, 0, 0.383_102])

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
def circuit_mvd3_tsv(mvd3_file, tsv_file):
    mvd3filename = path.join(_dir, mvd3_file)
    tsvfilename = path.join(_dir, tsv_file)
    mvd3file = mt.open(mvd3filename)
    mvd3file.read_tsv_info(tsvfilename)
    return mvd3file


def test_tsv_layer(circuit_mvd3_tsv):
    layers = circuit_mvd3_tsv.layers()

    assert layers[0] == 1
    assert layers[9] == 1
    assert layers[33] == 6


def test_tsv_emodels(circuit_mvd3_tsv):
    emodels = circuit_mvd3_tsv.emodels()

    assert emodels[0] == "bAC_327962063"
    assert emodels[9] == "dSTUT_321707905"
    assert emodels[33] == "L6_cADpyr_471819401"


def test_tsv_emodels_indices(circuit_mvd3_tsv):
    emodels = circuit_mvd3_tsv.emodels([0,9,33])

    assert emodels[0] == "bAC_327962063"
    assert emodels[1] == "dSTUT_321707905"
    assert emodels[2] == "L6_cADpyr_471819401"


def test_mvd3_mecombos_indices(circuit_mvd3_tsv):
    me_combos = circuit_mvd3_tsv.me_combos([0,9,33])

    assert me_combos[0] == "bAC_1_02583f52ff47b88961e4216e2972ee8c"
    assert me_combos[1] == "dSTUT_1_87dd39e6b0255ec053001f16da85b0e0"
    assert me_combos[2] == "cADpyr_6_97957c6ebc6ac6397bf0fa077d39580c"


def test_tsv_threshold_current(circuit_mvd3_tsv):
    threshold_currents = circuit_mvd3_tsv.threshold_currents()

    assert threshold_currents[0] == 0
    assert threshold_currents[9] == 0
    assert threshold_currents[33] == 0.2

def test_tsv_holding_current(circuit_mvd3_tsv):
    holding_currents = circuit_mvd3_tsv.holding_currents()

    assert holding_currents[0] == 0
    assert holding_currents[9] == 0.1
    assert holding_currents[33] == 0.15


@pytest.fixture
def new_sonata_file():
    return path.join(_dir, "nodes.h5")


@pytest.fixture
def circuit_new_sonata(new_sonata_file):
    sonatafilename = path.join(_dir, new_sonata_file)
    sonata = mt.open(new_sonata_file)
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


def test_list_allLayers(circuit_new_sonata):
    allLayers = circuit_new_sonata.all_layers

    assert allLayers[0] == "SR"
    assert allLayers[1] == "SP"
    assert allLayers[2] == "SO"
