## Version 2.3.0
 - TSV reader for unified API (MDV3+TSV / Sonata)
   * emodels and current API now functional in both configs.
 - Mvd3 reader refactoring

## Version 2.2.1
 - API to get emodels and currents (Sonata)
 - Support for automatically select a single population (default)
 - Python: Support to extract properties for a list of gids
 - Better dectection of mvd and sonata files.

## Version 2.2.0
 - Support optional Rotations or provided as angles
 - Use Version directly from git (git describe)
 - Added stride and offset support in conVersion utility

## Version 2.1.0
 - Initial support for Sonata nodes (`SonataFile`) using a new `MVD::File` interface.
   * Test utility to convert mvd3 to sonata `mvd2sonata.py`
 - New `open()` generic function
 - Tests to use Boost shared libs

## Version 2.0.0
 - Switched Bindings to Pybind11
 - Dropped MVD2 bindings

## Version 1.5.2
 - Fixed dependencies in Cmake installed target

## Version 1.5.1
 - Dont read whole library dataset if not required
 - Improved tests and CI with docker & spack
 - Python 3 and bindings compilation

## Version 1.5
 - Added regions API

## Version 1.4
 - Added `getLayer()` and `HyperColumns` API
 - Use external Highfive
 - Python bindings: Dont regen if not requested, respect paths

## Version 1.3
 - New python bindings
 - Support for additional properties

## Version 1.2
 - update HighFive dependency
 - minor cmake fixes

## Version 1.1
- support for summary circuit description
- reinforce MVD2 parser
