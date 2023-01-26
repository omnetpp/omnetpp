{
  pname, version, src ? ./.,                   # direct parameters
  stdenv, lib, python3,
  omnetpp
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

    # external OMNeT++ dependencies
    oppDependencies = [ ];

    propagatedNaviveBuildInputs = [ omnetpp ] ++ oppDependencies;

    installPhase = ''
      runHook preInstall

      echo -e >setenv "export OMNETPP_ROOT=\$__omnetpp_root_dir\n"
      substituteInPlace src/utils/opp_ide \
          --replace "\$IDEDIR/error.log" "/dev/null" \
          --replace "\$IDEDIR/../samples" "\$(pwd)"
      mkdir -p ${placeholder "out"} ${placeholder "out"}/bin
      mv setenv ide ${placeholder "out"}
      mv src/utils/opp_ide src/utils/omnetpp src/utils/omnest ${placeholder "out"}/bin
      patchShebangs ${placeholder "out"}/bin

      runHook postInstall
    '';

    meta = with lib; {
      outputsToInstall = [ "out" ];
      homepage= "https://omnetpp.org";
      description = "OMNeT++ Discrete Event Simulator IDE";
      longDescription = "OMNeT++ is an extensible, modular, component-based C++ simulation library and framework, primarily for building network simulators.";
      changelog = "https://github.com/omnetpp/omnetpp/blob/omnetpp-${version}/WHATSNEW";
      license = licenses.free;
      maintainers = [ "rudi@omnetpp.org" ];
      platforms = [ "x86_64-linux" ];
    };
  };
in
   omnetpp-outputs
