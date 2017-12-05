# MVD-tool

## Information

A simple C++ parser and tool for the MVD3 neuroscience file format

* provide a tool to convert MVD2 to MVD3
* provide a tool to export MVD to CSV
* provide a simple header base parser for MVD2
* provide a parser based on the HighFive library for MVD3

## Compilation

fetch HighFive as submodule ( if not already done )

```bash
git submodule update --recursive --init
```

configure, compile

```bash
mkdir build; pushd build
cmake ../
make
```

## Contributors

* Adrien Devresse  - <adrien.devresse@epfl.ch>
* Fernando Perreira - <fernando.pereira@epfl.ch>
* Arseny V. Povolotsky - <arseny.povolotsky@epfl.ch>

## License

LGPLv2.1+
