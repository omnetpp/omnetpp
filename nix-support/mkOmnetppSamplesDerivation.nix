{
  pname, version, src ? ./.,        # direct parameters
  stdenv, lib, lld,                 # build environment
  autoPatchelfHook,
  perl, omnetpp,                    # dependencies
  python3
}:
let
in
stdenv.mkDerivation rec {
  inherit pname version src;

  enableParallelBuilding = true;
  strictDeps = true;
  dontStrip = true;
  dontWrapQtApps = true;

  buildInputs = [ omnetpp (python3.withPackages(ps: with ps; [ pyqt5 ])) ];

  # tools required for build only (not needed in derivations)
  nativeBuildInputs = [ autoPatchelfHook ];

  # tools required for build only (needed in derivations)
  propagatedNativeBuildInputs = buildInputs ++ [ ];

  prePatch = ''
    cd samples
    patchShebangs . wiredphy parsim resultfiles queuenet queueinglibext cqn/parsim
    substituteInPlace .opp_samples_wrapper --replace "@omnetpp.bin@" "${omnetpp.bin}"
    substituteInPlace .opp_samples_wrapper --replace "@omnetpp.dev@" "${omnetpp.dev}"    
  '';

  buildPhase = ''
    make MODE=release -j$NIX_BUILD_CORES
  '';

  installPhase = ''
    runHook preInstall

    rm -rf ./*/out queueinglibext resultfiles database wizards
    mkdir -p ${placeholder "out"} ${placeholder "out"}/bin
    mv .opp_samples_wrapper opp_samples samples.html ${placeholder "out"}/bin/
    mv * ${placeholder "out"}/

    runHook postInstall
  '';

  meta = with lib; {
    homepage= "https://omnetpp.org";
    description = "OMNeT++ Sample Simulations";
    maintainers = [ "rudi@omnetpp.org" ];
    platforms = [  "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
  };
}