{
  description = "Flake for the project game of life 3D";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-23.11";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = inputs: inputs.utils.lib.eachDefaultSystem (system:
    let
      pkgs = import inputs.nixpkgs { inherit system; };
    in
    {
      devShell = pkgs.mkShell {
        buildInputs = with pkgs; [ git cmake gcc clang-tools_17 valgrind ];
     };
    });
}
