from libcpp.string cimport string
cimport std
cimport boost

# ======================================================================================================================
cdef extern from "mvd/mvd_generic.hpp" namespace "MVD::MVDType":
# ----------------------------------------------------------------------------------------------------------------------

    cdef enum MVDType:
        pass


# ======================================================================================================================
cdef extern from "mvd/mvd_generic.hpp" namespace "MVD":
# ----------------------------------------------------------------------------------------------------------------------
    ctypedef int DIM2 "2"
    ctypedef boost.multi_array[double, DIM2] Positions
    ctypedef boost.multi_array[double, DIM2] Rotations


    ###### Cybinding for class Range ######
    cdef cppclass Range:
        int offset
        int count
        Range()
        Range(std.size_t, std.size_t)


    cdef cppclass MVDFile:
        size_t getNbNeuron()
        Positions getPositions()
        Rotations getRotations()
        Positions getPositions(Range)
        Rotations getRotations(Range)


    MVDType is_mvd_file(string)
    MVDType open_mvd(string)
