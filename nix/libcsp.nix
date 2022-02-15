{ stdenv, lib, fetchurl, callPackage, python, pkg-config, unzip }:
let
  libsocketcan = callPackage ./libsocketcan.nix {};
in
stdenv.mkDerivation rec {
  name = "libcsp";
  src = fetchurl {
    url = "https://github.com/NTNU-SmallSat-Lab/libcsp/archive/refs/heads/hypso.zip";
    sha256 = "1i7akz053bd453636iafv6vs88f5383m2k85isqwfv1lk96sqsz3";
  };
  buildInputs = [ 
    unzip
    python
    pkg-config
  ];

  wafFlags = [
    "--out=$out"                        # set out directory
    "--prefix=$out"                     # set install directory
    "--include=${libsocketcan}/include" # include-path for libsocketcan
    "--install-csp"                     # install csp headers and lib
    "--enable-crc32"                    # enable cdc32 support
    "--enable-init-shutdown"            # use init system commands for shutdown/reboot
    "--enable-if-can"                   # enable can interface 
    "--enable-can-socketcan"            # enable linux socketcan driver
    "--enable-promisc"                  # enable promscuous support
    # "--enable-examples"                 # enable examples
    "--with-os=posix"                   # set operating system (deafult: posix)
    "--with-rtable=cidr"                # set routing table: 'static' or 'cidr'
    "--with-driver-usart=linux"         # build usart driver: windows, linux, None (default: None)
    "--with-driver-tcp=linux"           # build tcp driver
  ];

  configurePhase = ''
    python waf configure ${builtins.concatStringsSep " " wafFlags}  
  '';

  buildPhase = ''
    python waf build
  '';

  installPhase = ''
    python waf install
  '';
  
  dontFixup = true;
}
