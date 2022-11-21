{ 
  pname, version, src ? ./.,                 # direct parameters
  stdenv, lib, fetchurl, symlinkJoin, lndir, # build environment
  bintools, perl, flex, bison, lld,          # dependencies
  python3,
  qtbase ? null, qtsvg ? null,
  MODE ? "release",                          # build parameters
  WITH_QTENV ? false,                        # build parameters
}:
assert WITH_QTENV -> ! builtins.any isNull [ qtbase qtsvg ] ;

let
  omnetpp-outputs = stdenv.mkDerivation rec {
    inherit pname version src;

    outputs = [ "bin" "dev" "out" ]; # doc, samples, gui, gui3d, ide

    enableParallelBuilding = true;
    strictDeps = true;
    dontStrip = true;
    dontWrapQtApps = true;
    # hardeningDisable = all;

    buildInputs = [ ] ++ lib.optionals WITH_QTENV [ qtbase ];

    # tools required for build only (not needed in derivations)
    nativeBuildInputs = [ ];

    # tools required for build only (needed in derivations)
    propagatedNativeBuildInputs = [
      stdenv perl bison flex bintools lld lndir
      (python3.withPackages(ps: with ps; [ numpy pandas matplotlib scipy seaborn ]))
    ] ++ lib.optionals WITH_QTENV [ qtbase ];

    configureFlags = [ "WITH_OSG=no" "WITH_OSGEARTH=no" ]
                     ++ lib.optionals (!WITH_QTENV) [ "WITH_QTENV=no" ];

    # we have to patch all shebangs to use NIX versions of the interpreters
    prePatch = ''
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

    buildPhase = ''
      substituteInPlace Makefile.inc --replace ${qtbase.dev}/lib ${qtbase.out}/lib
      substituteInPlace Makefile.inc --replace "OMNETPP_IMAGE_PATH := \$(OMNETPP_ROOT)/images" "OMNETPP_IMAGE_PATH := ${placeholder "bin"}/images"
      make MODE=release -j$NIX_BUILD_CORES
    '';

    installPhase = ''
      runHook preInstall

      substituteInPlace nix-support/set-qtenv --replace "@qtPluginPath@" "${qtbase.bin}/${qtbase.qtPluginPrefix}:${qtsvg.bin}/${qtbase.qtPluginPrefix}"
      substituteInPlace nix-support/set-qtenv --replace "@ldLibraryPath@" "${placeholder "dev"}/lib"
      substituteInPlace Makefile.inc --replace "OMNETPP_LIB_DIR = \$(OMNETPP_ROOT)/lib\$(OUTPUT_PREFIX)" "OMNETPP_LIB_DIR = ${placeholder "bin"}/lib"

      mkdir -p ${placeholder "dev"}/bin ${placeholder "dev"}/lib

      rm nix-support/*.nix
      mv Makefile.inc configure.user include src nix-support ${placeholder "dev"}
      (cd bin && mv opp_run_dbg* opp_configfilepath opp_featuretool opp_makemake opp_shlib_postprocess ${placeholder "dev"}/bin || true)
      (cd lib && mv *.a *_dbg.* liboppqtenv.* ${placeholder "dev"}/lib || true)

      mkdir -p ${placeholder "bin"}/bin ${placeholder "bin"}/lib
      mv images setenv Version ${placeholder "bin"}
      (cd bin && mv  opp_run* opp_msgtool opp_msgc opp_nedtool opp_charttool opp_eventlogtool opp_fingerprinttest opp_scavetool opp_test ${placeholder "bin"}/bin || true)
      (cd lib && mv * ${placeholder "bin"}/lib || true)
      mv python ${placeholder "bin"}

      mkdir -p ${placeholder "out"}

      runHook postInstall
      '';


    preFixup = lib.optionalString stdenv.isLinux ''
      (
        # patch rpath on BIN executables
        for file in $(find ${placeholder "bin"} -type f -executable); do
          if patchelf --print-rpath $file; then
            patchelf --set-rpath '${placeholder "bin"}/lib' $file
          fi
        done
        for file in $(find ${placeholder "dev"} -type f -executable); do
          if patchelf --print-rpath $file; then
            patchelf --set-rpath '${placeholder "bin"}/lib:${qtbase.out}/lib' $file
          fi
        done
      )'';

    meta = with lib; {
      outputsToInstall = [ "bin" "dev" ];
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
/*
  symlinkJoin {
    name = "${pname}-${version}";
    paths = with omnetpp-outputs; [ bin dev ]; 
    postBuild = "";  # TODO optimize the symlink forest (include, src, images, samples, python could be linked as a single directory)
  }
*/