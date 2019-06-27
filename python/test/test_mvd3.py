"""Generic MVD3 read tests
"""
from os import path

import numpy
import pytest

import mvdtool as mt

_dir,_ = path.split(__file__)


@pytest.fixture()
def mvd():
    """Provide access to an MVD file
    """
    mvd_path = path.join(_dir, "circuit.mvd3").encode()
    return mt.MVD3.File(mvd_path)


def test_number_neurons(mvd):
    assert len(mvd) == 1000


def test_position_values(mvd):
    posics = mvd.positions()
    assert posics.shape[0] == 1000
    assert numpy.allclose(posics[0], [40.821401, 1986.506637, 10.788424])


def test_position_range(mvd):
    posics = mvd.positions()
    posics_range = mvd.positions(10, 10)

    assert len(posics_range) == 10
    assert numpy.allclose(posics_range, posics[10:20])


def test_morphology_values(mvd):
    morphos = mvd.morphologies()

    assert morphos[0] == "sm090227a1-2_idC"
    assert morphos[20] == "dend-C280998A-P3_axon-sm110131a1-3_INT_idA"
    assert morphos[21] == "dend-rr110114C1_idA_axon-sm110131a1-3_INT_idA"


def test_etype_values(mvd):
    etypes = mvd.etypes()

    etypes[0] == "cACint"
    etypes[10] == "cACint"
    etypes[20] == "cADpyr"

def test_mtype_values(mvd):
    mtypes = mvd.mtypes()

    mtypes[0] == "L1_SLAC"
    mtypes[10] == "L1_SLAC"
    mtypes[20] == "L23_PC"

def test_region_values(mvd):
    regions = mvd.regions()

    regions[1] == "L1"
    regions[9] == "L2"
    regions[10] == "L3"
    regions[15] == "L23"

def test_classes_values(mvd):
    classes = mvd.synapse_classes()

    classes[0] == "INH"
    classes[10] == "INH"
    classes[20] == "EXC"

def test_ranges(mvd):
    attrs = [
        "morphologies",
        "etypes",
        "mtypes",
        "regions",
        "synapse_classes"
    ]

    for attr in attrs:
        OFFSET = 10
        values = getattr(mvd, attr)()
        indices = getattr(mvd, f"all_{attr}")
        ranged = getattr(mvd, f"raw_{attr}")(OFFSET, 5)

        for n, i in enumerate(ranged):
            assert values[n + OFFSET] == indices[i]
