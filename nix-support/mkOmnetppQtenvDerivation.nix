{ 
  pname, version, src ? ./.,                 # direct parameters
  stdenv, lib, bintools, autoPatchelfHook,
  perl, flex, bison, lld,                    # dependencies
  python3, qtbase, qtsvg,
  omnetpp,
  MODE ? "release",                          # build parameters
}:

let
  omnetpp-outputs = stdenv.mkDerivation rec {
    inherit pname version src;

    enableParallelBuilding = true;
    strictDeps = true;
    dontStrip = true;
    dontWrapQtApps = true;

    buildInputs = [ qtbase ];

    nativeBuildInputs = [ autoPatchelfHook perl python3 bison flex lld bintools qtbase];

    patches = [ ./flake-support-on-6.0.1.patch ];

    postPatch = ''
      # variables used as a replacement in the setup hook
      export qtPluginPath="${qtbase.bin}/${qtbase.qtPluginPrefix}:${qtsvg.bin}/${qtbase.qtPluginPrefix}"
      export ldLibraryPath="$out/lib"
    '';

    preConfigure = ''
      if [ ! -f configure.user ]; then
        cp configure.user.dist configure.user
      fi
      source setenv
    '';

    configureFlags = [ "LDFLAGS=-L${omnetpp.out}/lib" # to allow linking with core omnetpp libraries
                       "WITH_OSG=no" "WITH_OSGEARTH=no" "WITH_QTENV=yes" ];

    buildPhase = ''
      runHook preBuild
      (make utils && cd src/qtenv && make MODE=release -j$NIX_BUILD_CORES)
      runHook postBuild
    '';

    installPhase = ''
      runHook preInstall
      mkdir -p $out
      mv Version lib $out
      runHook postInstall
    '';

    preFixup = 
      lib.optionalString stdenv.isLinux ''
        addAutoPatchelfSearchPath "${omnetpp.out}/lib"
    '';

    setupHook = ./set-qtenv;

    meta = with lib; {
      homepage= "https://omnetpp.org";
      description = "OMNeT++ Discrete Event Simulator QTEnv";
      longDescription = "OMNeT++ is an extensible, modular, component-based C++ simulation library and framework, primarily for building network simulators.";
      changelog = "https://github.com/omnetpp/omnetpp/blob/omnetpp-${version}/WHATSNEW";
      license = licenses.free;
      maintainers = [ "rudi@omnetpp.org" ];
      platforms = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
    };
  };
in
   omnetpp-outputs
