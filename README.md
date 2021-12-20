# MVD-tool

## Information

A simple C++ parser and tool for the MVD3 and Sonata neuroscience file formats

* provide a tool to convert MVD2 to MVD3
* provide a tool to export MVD to CSV
* provide a simple header base parser for MVD2
* provide a parser based on the HighFive library for MVD3 and Sonata

It also provides a high level Python API for parsing MVD3 and Sonata files.

## Compilation

#### Prerequisites
 - CMake >= 3.0
 - GCC >= 4.9
 - BOOST >= 1.41
 - HighFive
 - libSONATA

#### Compile and Install the C++ parser

```bash
mkdir build; pushd build
cmake ../ -DCMAKE_INSTALL_PREFIX=<install_dir>
make
make install
```
#### Compile and Install the Python API
```bash
python setup.py install
```

## Examples with the Python API

#### Reading Sonata files
```python
import mvdtool
node = mvdtool.open("tests/nodes.h5")
# retrieve data for the full range
node.morphologies()
# retrieve data for a certain range
node.morphologies([0, 1, 3])
```

#### Reading MVD3 files
```python
import mvdtool
mvd = mvdtool.open("tests/circuit.mvd3")
mvd.morphologies()
```
Certain attributes in MVD files require the information from the mecombo file. In this case, it's strictly necessary that the mecombo file contains all combos from the MVD file.
```python
mvd_tsv = mvdtool.open("tests/circuit_tsv.mvd3")
mvd_tsv.open_combo_tsv("tests/mecombo_emodel.tsv")
mvd_tsv.emodels()
```

## Funding & Acknowledgment
 
The development of this software was supported by funding to the Blue Brain Project, a research center of the École polytechnique fédérale de Lausanne (EPFL), from the Swiss government's ETH Board of the Swiss Federal Institutes of Technology.
 
Copyright © 2015-2021 Blue Brain Project/EPFL


## License

LGPLv2.1+
