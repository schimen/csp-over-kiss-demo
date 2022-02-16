{ stdenv, lib, fetchurl, autoreconfHook }:

stdenv.mkDerivation rec {
  pname = "libsocketcan";
  version = "0.0.12";
  src = fetchurl {
    url = "https://git.pengutronix.de/cgit/tools/libsocketcan/snapshot/libsocketcan-${version}.tar.gz";
    sha256 = "0k549qmbclkrrf92fdqh8np7blbw4dfwds0sa8s4vwx6mwr2shz4";
  };
  buildInputs = [ autoreconfHook ];
}
