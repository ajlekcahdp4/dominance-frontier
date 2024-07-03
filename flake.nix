{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
    treefmt-nix = {
      url = "github:numtide/treefmt-nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };

  };

  outputs =
    { flake-parts, treefmt-nix, ... }@inputs:
    flake-parts.lib.mkFlake { inherit inputs; } {

      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];

      imports = [ treefmt-nix.flakeModule ];

      perSystem =
        { pkgs, ... }:
        let
          dominance-frontier = pkgs.callPackage ./. { enableAsserts = true; };

          debugPackages = with pkgs; [
            valgrind
            gdb
          ];

          envPackages = with pkgs; [
            debugPackages
            xdot
          ];

          nativeBuildInputs = dominance-frontier.nativeBuildInputs ++ envPackages;
          buildInputs = dominance-frontier.buildInputs;
        in
        {
          imports = [ ./nix/treefmt.nix ];
          devShells.default = pkgs.mkShell { inherit nativeBuildInputs buildInputs; };
          packages.default = dominance-frontier;
        };
    };
}
