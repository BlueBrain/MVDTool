"""Generic MVD3 read tests
"""
from os import path

import numpy
import pytest

import mvdtool as mt

_dir, _ = path.split(__file__)


@pytest.fixture(params=["circuit.mvd3", "sonata.h5"])
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


def test_etype_values(circuit):
    etypes = circuit.etypes()

    assert etypes[0] == "cACint"
    assert etypes[10] == "cACint"
    assert etypes[20] == "cADpyr"


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
