# -*- coding: utf-8 -*-
# =====================================================================================================================
# These bindings were automatically generated by cyWrap. Please do dot modify.
# Additional functionality shall be implemented in sub-classes.
#
__copyright__ = "Copyright 2016 EPFL BBP-project"
# =====================================================================================================================
from cython.operator cimport dereference as deref
from libcpp cimport bool
cimport std
from MVD cimport MVDFile, Range

#Workaround for cython templates, which dont accept integers yet

# ======================================================================================================================
cdef extern from "mvd/mvd3.hpp" namespace "MVD3":
# ----------------------------------------------------------------------------------------------------------------------

    ###### Cybinding for class MVD3File ######
    cdef cppclass MVD3File(MVDFile):
        MVD3File(std.string) except+
        # Inherits getNbNeuron(), getPositions(MVD.Range=None), getRotations(Range=None)
        std.vector[std.string] getMorphologies(Range) except+
        std.vector[std.string] getMorphologies() except+
        std.vector[std.string] getEtypes(Range) except+
        std.vector[std.string] getEtypes() except+
        std.vector[std.string] getMtypes(Range) except+
        std.vector[std.string] getMtypes() except+
        std.vector[std.string] getSynapseClass(Range) except+
        std.vector[std.string] getSynapseClass() except+
        std.vector[int] getLayers(Range) except+
        std.vector[int] getLayers() except+
        std.vector[int] getHyperColumns(Range) except+
        std.vector[int] getHyperColumns() except+
        std.vector[int] getMiniColumns(Range) except+
        std.vector[int] getMiniColumns() except+
        std.vector[std.size_t] getIndexMorphologies(Range) except+
        std.vector[std.size_t] getIndexEtypes(Range) except+
        std.vector[std.size_t] getIndexMtypes(Range) except+
        std.vector[std.size_t] getIndexSynapseClass(Range) except+
        std.vector[std.string] listAllMorphologies() except+
        std.vector[std.string] listAllEtypes() except+
        std.vector[std.string] listAllMtypes() except+
        std.vector[std.string] listAllSynapseClass() except+
        std.vector[double] getCircuitSeeds() except+
