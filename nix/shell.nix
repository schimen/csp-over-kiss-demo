{ pkgs ? import <nixpkgs> {} }:

let
  nng = pkgs.callPackage ./nng.nix {};
  libcsp = pkgs.callPackage ./libcsp.nix {};
  libsocketcan = pkgs.callPackage ./libsocketcan.nix {};
in

pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    cmake
    gnumake
    check
    nng
    libcsp
    libsocketcan
    python
    socat
  ];
}
