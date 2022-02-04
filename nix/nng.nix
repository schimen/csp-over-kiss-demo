{ stdenv, lib, fetchFromGitHub, cmake, ninja }:
 
stdenv.mkDerivation rec {
  pname = "nng";
  version = "1.5.2";
  meta = with lib; {
    description = "A lightweight, broker-less library, offering a simple API to solve common recurring messaging problems.";
    homepage    = "https://nng.nanomsg.org/";
    license     = licenses.mit;
    platforms   = platforms.linux;
    maintainers = [ ];
  };
  src = fetchFromGitHub {
    owner = "nanomsg";
    repo = "nng";
    rev = "v${version}";
    hash = "sha256:1sap0iny3z9lhmaiassv8jc399md1307y32xxx3mrr74jcpcrf59";
  };
  buildInputs = [ cmake ninja ];
}