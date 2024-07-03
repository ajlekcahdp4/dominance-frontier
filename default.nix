{
  stdenv,
  lib,
  cmake,
  flex,
  bison,
  popl,
  ...
}:
let
  fs = lib.fileset;
in
stdenv.mkDerivation {
  pname = "dominance-frontier";
  version = "0.0.0";
  src = fs.toSource {
    root = ./.;
    fileset = fs.unions [
      ./CMakeLists.txt
      ./src
      ./include
      ./cmake
    ];
  };

  nativeBuildInputs = [
    cmake
    flex
    bison
    popl
  ];
  buildInputs = [ ];
}
