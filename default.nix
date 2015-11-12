# Nix development environment
#
# build:
# nix-build -I "BBPpkgs=https://github.com/BlueBrain/bbp-nixpkgs/archive/master.tar.gz" default.nix
#
# build and test:
# nix-build -I "BBPpkgs=https://github.com/BlueBrain/bbp-nixpkgs/archive/master.tar.gz" --arg testExec true  default.nix  -j 4
#
# dev shell:
# nix-shell -I "BBPpkgs=https://github.com/BlueBrain/bbp-nixpkgs/archive/master.tar.gz"  default.nix
#
with import <BBPpkgs> { };

{ testExec ? false} : {


    func = stdenv.mkDerivation rec {
      name = "functionalizer-DEV_ENV";
      src = ./.;
      buildInputs = [stdenv pkgconfig boost cmake hdf5 doxygen zlib]
      ++ stdenv.lib.optional ( testExec == true) [ bbptestdata ];

      doCheck = testExec;

    };

}


