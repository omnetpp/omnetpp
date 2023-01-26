{ 
  pname, version, src ? ./.,                 # direct parameters
  stdenv, lib, bintools, autoPatchelfHook, writeText,
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
    dontWrapQtApps = true;

    # external OMNeT++ dependencies
    oppDependencies = [ qtbase ];

    buildInputs = [ omnetpp ] ++ oppDependencies;

    propagatedNativeBuildInputs = [ autoPatchelfHook omnetpp ] ++ omnetpp.oppDependencies ++ oppDependencies;

    patches = [ ./flake-support-on-6.0.1.patch ];

    postPatch = ''
      # variables used as a replacement in the setup-hook
      export ldLibraryPathVar=${if stdenv.isDarwin then "DYLD_FALLBACK_LIBRARY_PATH" else "LD_LIBRARY_PATH"}
      export ldLibraryPath="$out/lib"
      export qtPluginPath="${qtbase.bin}/${qtbase.qtPluginPrefix}:${qtsvg.bin}/${qtbase.qtPluginPrefix}"
    '';

    shellHook = ''
      echo "shellhook-qtenv run 2"
      export XXX_SHELLHOOK_2=yes
    '';

    setupHook =  writeText "../setenv" ''
      export @ldLibraryPathVar@='@ldLibraryPath@':''${@ldLibraryPathVar@:-}
      export QT_PLUGIN_PATH='@qtPluginPath@':''${QT_PLUGIN_PATH:-}

      # disable GL support as NIX does not play nicely with OpenGL (except on nixOS)
      export QT_XCB_GL_INTEGRATION=''${QT_XCB_GL_INTEGRATION:-none}
      
      export DISPLAY=''${DISPLAY:-:0}
      export XDG_RUNTIME_DIR=''${XDG_RUNTIME_DIR:-$TMPDIR/xdg_runtime}
      export XDG_CACHE_HOME=''${XDG_CACHE_HOME:-$TMPDIR/xdg_cache}
      export HOME=''${HOME:-$TMPDIR/xdg_home}
    '';

    preConfigure = ''
      if [ ! -f configure.user ]; then
        cp configure.user.dist configure.user
      fi
      source setenv
    '';

    configureFlags = [ "LDFLAGS=-L${omnetpp.out}/lib" # to allow linking with core omnetpp-runtime libraries
                       "WITH_OSG=no" "WITH_OSGEARTH=no" "WITH_QTENV=yes" ];

    buildPhase = ''
      runHook preBuild
      # to build only the qtenv library we go directly into its source folder
      (make utils && cd src/qtenv && make MODE=${MODE} -j$NIX_BUILD_CORES)
      runHook postBuild
    '';

    preFixup = 
      lib.optionalString stdenv.isLinux ''
        addAutoPatchelfSearchPath "${omnetpp.out}/lib"
    '';

    installPhase = ''
      runHook preInstall
      mkdir -p $out
      mv Version lib $out
      runHook postInstall
    '';

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
