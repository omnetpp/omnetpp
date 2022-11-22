{ 
  pname, version, src ? ./.,                   # direct parameters
  stdenv, lib, xdg-utils
}:

let
  # TODO: use this to write a script file instead of echo
  #opp_docbrowser = writeShellScriptBin "opp_docbrowser" ''
  #  ${xdg-utils}/bin/xdg-open ${placeholder "out"}/index.html
  #'';
  omnetpp-outputs = stdenv.mkDerivation rec {
    inherit pname version src;

    outputs = [ "out" ];

    enableParallelBuilding = true;
    strictDeps = true;
    dontStrip = true;
    dontWrapQtApps = true;
    dontConfigure = true;
    dontBuild = true;

    buildInputs = [ xdg-utils ];

    installPhase = ''
      runHook preInstall

      mkdir -p ${placeholder "out"}
      cd doc
      mkdir -p bin
      echo -e >bin/opp_docbrowser "#!/bin/sh\n${xdg-utils}/bin/xdg-open ${placeholder "out"}/index.html"
      chmod +x bin/opp_docbrowser
      patchShebangs bin

      mv * ${placeholder "out"}

      runHook postInstall
    '';

    meta = with lib; {
      outputsToInstall = [ "out" ];
      homepage= "https://omnetpp.org";
      description = "OMNeT++ Discrete Event Simulator documentation";
      longDescription = "OMNeT++ is an extensible, modular, component-based C++ simulation library and framework, primarily for building network simulators.";
      changelog = "https://github.com/omnetpp/omnetpp/blob/omnetpp-${version}/WHATSNEW";
      license = licenses.free;
      maintainers = [ "rudi@omnetpp.org" ];
      platforms = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
    };
  };
in
   omnetpp-outputs
