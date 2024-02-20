{
  description = "Flake for the Kjell shell";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/23.11";

    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, ... }@inputs:
    inputs.utils.lib.eachSystem [
      "x86_64-linux"
      "i686-linux"
      "aarch64-linux"
      "x86_64-darwin"
    ] (system:
      let
        pkgs = import nixpkgs {
          inherit system;

          overlays = [ ];
        };
      in {
        devShells.default = pkgs.mkShell rec {
          name = "Kjell shell";

          packages = with pkgs; [
            ccls
            clang-tools
            bison
            haskellPackages.BNFC
          ];

          # development.
          shellHook = let icon = "f121";
          in ''
            export PS1="$(echo -e '\u${icon}') {\[$(tput sgr0)\]\[\033[38;5;228m\]\w\[$(tput sgr0)\]\[\033[38;5;15m\]} (${name}) \\$ \[$(tput sgr0)\]"
          '';
        };

        packages.default = pkgs.callPackage ./default.nix { };
      });
}

