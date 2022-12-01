{ 
  pname, version, src ? ./.,                 # direct parameters
  stdenv, lib, bintools, writeText,
  perl, flex, bison, lld, python3,           # dependencies
  MODE ? "release",                          # build parameters
}:

let
  omnetpp-outputs = stdenv.mkDerivation rec {
    inherit pname version src;

    enableParallelBuilding = true;
    strictDeps = true;

    # tools required for build only (needed in derivations)
    propagatedNativeBuildInputs = [
      stdenv perl bison flex bintools lld
      (python3.withPackages(ps: with ps; [ numpy pandas matplotlib scipy seaborn ]))
    ];

    patches = [ ./flake-support-on-6.0.1.patch ];

    # we have to patch all shebangs to use NIX versions of the interpreters
    postPatch = ''
      patchShebangs src
    '';

    preConfigure = ''
      if [ ! -f configure.user ]; then
        cp configure.user.dist configure.user
      fi
      source setenv
      rm -rf samples images/src python/ChangeLog
    '';

    configureFlags = [ "WITH_QTENV=no" "WITH_OSG=no" "WITH_OSGEARTH=no" ];

    buildFlags = [ "MODE=release" "OMNETPP_IMAGE_PATH=$(out)/images" ];

    installPhase = ''
      runHook preInstall
      
      mkdir -p $out

      # remove commands that are in different packages
      (cd bin; rm -f opp_ide omnetpp omnest opp_samples opp_docbrowser)

      mv doc/License doc/3rdparty.txt setenv Version Makefile.inc configure.user \
         bin images include lib python $out

      runHook postInstall
    '';

    setupHook = writeText "setup-hook" ''
      set +u # temporarily turn off unbouned variable check
      source @out@/setenv
      set -u
    '';

    meta = with lib; {
      homepage= "https://omnetpp.org";
      description = "OMNeT++ Discrete Event Simulator runtime";
      longDescription = "OMNeT++ is an extensible, modular, component-based C++ simulation library and framework, primarily for building network simulators.";
      changelog = "https://github.com/omnetpp/omnetpp/blob/omnetpp-${version}/WHATSNEW";
      license = licenses.free;
      maintainers = [ "rudi@omnetpp.org" ];
      platforms = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
    };
  };
in
   omnetpp-outputs
