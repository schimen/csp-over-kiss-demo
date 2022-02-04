{ stdenv, lib, fetchurl, pkgs, python, pkg-config }:

stdenv.mkDerivation rec {
  pname = "libcsp";
  version = "1.6.4";
  src = fetchurl {
    url = "https://github.com/GomSpace/libcsp/archive/refs/tags/${version}.tar.gz";
    sha256 = "1k6f9d1lzckaq6ybi8ss852xn77cpqmabzaa09lzbjqhrnw1s9pd";
  };
  buildInputs = [ 
    python
    pkg-config
  ];

  wafFlags = [
    "--out=$out"                # set out directory
    "--prefix=$out"             # set install directory
    "--install-csp"             # install csp headers and lib
    "--enable-shlib"            # build shared library
    "--enable-crc32"            # enable cdc32 support
    #"--enable-can-socketcan"    # enable linux socketcan driver
    "--enable-promisc"          # enable promscuous support
    "--enable-examples"         # enable examples
    "--with-os=posix"           # set operating system (deafult: posix)
    "--with-rtable='cidr'"      # set routing table: 'static' or 'cidr'
    "--with-driver-usart=linux" # build usart driver: windows, linux, None (default: None)
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
