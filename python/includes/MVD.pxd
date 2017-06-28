from libcpp.string cimport string
cimport MVD_MVDType

# ======================================================================================================================
cdef extern from "mvd/mvd_generic.hpp" namespace "MVD":
# ----------------------------------------------------------------------------------------------------------------------
    # Function to check format
    inline MVD_MVDType.MVDType is_mvd_file(const string & filename)