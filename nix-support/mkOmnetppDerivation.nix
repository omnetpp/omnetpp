{ 
  pname, version, src ? ./.,                 # direct parameters
  stdenv, lib, bintools, writeText,
  perl, flex, bison, lld,                    # dependencies
  python3, qtbase ? null, qtsvg ? null,
  MODE ? "release",                          # build parameters
  WITH_QTENV ? false,
}:
assert WITH_QTENV -> ! builtins.any isNull [ qtbase qtsvg ] ;

let
  omnetpp-outputs = stdenv.mkDerivation rec {
    inherit pname version src;

    enableParallelBuilding = true;
    strictDeps = true;
    dontStrip = true;
    dontWrapQtApps = true;
    # hardeningDisable = all;

    buildInputs = [ ] ++ lib.optionals WITH_QTENV [ qtbase ];

    # tools required for build only (not needed in derivations)
    nativeBuildInputs = [ ] ++ lib.optionals WITH_QTENV [ qtbase ];

    # tools required for build only (needed in derivations)
    propagatedNativeBuildInputs = [
      stdenv perl bison flex bintools lld
      (python3.withPackages(ps: with ps; [ numpy pandas matplotlib scipy seaborn ]))
    ];

    patches = [ ./flake-support-on-6.0.1.patch ];

    configureFlags = [ "WITH_OSG=no" "WITH_OSGEARTH=no" ]
                     ++ lib.optionals (!WITH_QTENV) [ "WITH_QTENV=no" ];

    # we have to patch all shebangs to use NIX versions of the interpreters
    prePatch = ''
      patchShebangs setenv
      patchShebangs src/nedxml
      patchShebangs src/utils
    '';

    preConfigure = ''
      if [ ! -f configure.user ]; then
        cp configure.user.dist configure.user
      fi
      source setenv
      rm -rf samples images/src
    '';

    buildPhase = (lib.optionalString WITH_QTENV ''
      runHook preBuild

      # Do not link with qtenv by default. It should be loaded dynamically.
      substituteInPlace Makefile.inc --replace "ALL_ENV_LIBS += \$(QTENV_LIBS)" ""
      substituteInPlace Makefile.inc --replace ${qtbase.dev}/lib ${qtbase.out}/lib
    '') + ''
      # fix the image path temporarily to the final pacakge location
      substituteInPlace Makefile.inc --replace "\$(OMNETPP_ROOT)/images" "${placeholder "out"}/images"

      make MODE=release -j$NIX_BUILD_CORES

      runHook postBuild
    '';

    installPhase = ''
      runHook preInstall
      mkdir -p ${placeholder "out"}

    '' + (lib.optionalString WITH_QTENV ''
      substituteInPlace nix-support/set-qtenv --replace "@qtPluginPath@" "${qtbase.bin}/${qtbase.qtPluginPrefix}:${qtsvg.bin}/${qtbase.qtPluginPrefix}"
      substituteInPlace nix-support/set-qtenv --replace "@ldLibraryPath@" "${placeholder "out"}/lib"
      mv nix-support/set-qtenv ${placeholder "out"}
    '') + ''

      # fix the image path back to the original value
      substituteInPlace Makefile.inc --replace "${placeholder "out"}/images" "\$(OMNETPP_ROOT)/images"

      (cd bin; rm -f opp_ide omnetpp omnest opp_samples opp_docbrowser)
      mv doc/License doc/3rdparty.txt ${placeholder "out"}
      mv setenv Version Makefile.inc configure.user bin images include lib python ${placeholder "out"}

      runHook postInstall
    '';

    postFixup = lib.optionalString (WITH_QTENV && stdenv.isLinux) ''      
        patchelf --set-rpath '${placeholder "out"}/lib:${qtbase.out}/lib' ${placeholder "out"}/lib/liboppqtenv.so
    '';

    setupHook = writeText "setup-hook" ''
      set +u # turn off unbouned variable check
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
