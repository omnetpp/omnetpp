{ 
  pname, version, src ? ./.,                   # direct parameters
  stdenv, lib, perl, texlive, libxml2,         # dependencies
  inkscape, graphviz, doxygen, python3,
  xdg-utils
}:

let
  omnetpp-outputs = stdenv.mkDerivation rec {
    inherit pname version src;

    outputs = [ "out" ];

    enableParallelBuilding = true;
    strictDeps = true;
    dontStrip = true;
    dontWrapQtApps = true;
    dontConfigure = true;

    # tools required for build only (not needed in derivations)
    nativeBuildInputs = [
      stdenv perl texlive libxml2 inkscape graphviz doxygen
      (python3.withPackages(ps: with ps; [ sphinx sphinx-material ]))
    ];

    buildInputs = [ xdg-utils ];

    postPatch = ''
      patchShebangs src/utils
      substituteInPlace src/utils/opp_docbrowser --replace "xdg-open \$(dirname \$0)/../doc/index.html" "${xdg-utils.out}/bin/xdg-open ${placeholder "out"}/index.html"
      substituteInPlace doc/index.html --replace "Discrete Event Simulator</p>" "Discrete Event Simulator <small>- ${version}</small></p>"
      substituteInPlace doc/src/manual/Makefile --replace "VERSION=x.x" "VERSION=${version}"
      substituteInPlace doc/src/*.doxyfile --replace "PROJECT_NUMBER = x.x" "PROJECT_NUMBER = ${version}"
    '';

    buildPhase = '' 
      export VERSION=${version}
      (cd doc/src; make)
    '';

    installPhase = ''
      runHook preInstall

      mkdir -p ${placeholder "out"} ${placeholder "out"}/bin
      mv src/utils/opp_docbrowser ${placeholder "out"}/bin
      (cd doc && rm -rf src && mv * ${placeholder "out"})

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
