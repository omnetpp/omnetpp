{
  description = "OMNeT++ Discrete Event Simulator";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-22.11";
    #nixpkgs.url = "nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem(system:
    let
      pname = "omnetpp";
      githash = self.shortRev or "dirty";
      timestamp = nixpkgs.lib.substring 0 8 self.lastModifiedDate;
      gversion = "${githash}.${timestamp}";
      sversion = "6.0.1"; # schemantic version number
      pkgs = import nixpkgs { inherit system; };
      tarball_base_url = "https://github.com/omnetpp/omnetpp/releases/download/${pname}-${sversion}/${pname}-${sversion}";
      src_core = pkgs.fetchzip { url = "${tarball_base_url}-core.tgz"; sha256 = "sha256-ftmYmDSE3zGm9pLyi3QR7wDWinYwAN8rQog0WietSCk="; };
      src_linux-x86_64 = pkgs.fetchzip { url = "${tarball_base_url}-linux-x86_64.tgz"; sha256 = "sha256-968zGE+C7G5nYNw7jlcVbs5QwXuPy8dTMQQzn5mLJyU="; };

    in rec {
      # set different defaults for creating packages.
      oppPkgs =  pkgs // {
          inherit (pkgs.llvmPackages_14) stdenv bintools;  # use clang14 instead of the standard g++ compiler
          inherit (pkgs.qt515) qtbase qtsvg;
          python3 = pkgs.python310;
          texlive = pkgs.texlive.combined.scheme-full;
          callPackage = pkgs.newScope oppPkgs;
      };

      packages = rec {
        default = packages."${pname}-runtime";

        "${pname}-runtime" = oppPkgs.callPackage ./nix-support/mkOmnetppDerivation.nix {
          pname = "${pname}-runtime";
          version = sversion;
          src = src_core;
        };

        "${pname}-qtenv" = oppPkgs.callPackage ./nix-support/mkOmnetppQtenvDerivation.nix {
          pname = "${pname}-qtenv";
          version = sversion;
          src = src_core;
          omnetpp = self.packages.${system}."${pname}-runtime"; # depends on the base omnetpp runtime component
        };

        "${pname}-doc" = oppPkgs.callPackage ./nix-support/mkOmnetppDocDerivation.nix {
          pname = "${pname}-doc";
          version = sversion;
          src = src_core;
        };

        "${pname}-ide" = oppPkgs.callPackage ./nix-support/mkOmnetppIDEDerivation.nix {
          pname = "${pname}-ide";
          version = sversion;
          src = src_linux-x86_64;
          omnetpp = self.packages.${system}."${pname}-runtime";
        };

        "${pname}-runtime-git" = oppPkgs.callPackage ./nix-support/mkOmnetppDerivation.nix {
          pname = "${pname}-runtime-git";
          version = gversion;
          src = self;
        };

        "${pname}-samples-git" = oppPkgs.callPackage ./nix-support/mkOmnetppSamplesDerivation.nix {
          pname = "${pname}-samples-git";
          version = gversion;
          src = self;
          omnetpp = self.packages.${system}."${pname}-runtime";
          omnetpp-qtenv = self.packages.${system}."${pname}-qtenv";
        };

        "${pname}-doc-git" = oppPkgs.callPackage ./nix-support/mkOmnetppDocGitDerivation.nix {
          pname = "${pname}-doc-git";
          version = gversion;
          src = self;
        };
      };

      devShells = rec {
        default = oppPkgs.stdenv.mkDerivation {
          name = "${pname}-${sversion} dependencies";
          buildInputs =
              self.packages.${system}."${pname}-runtime".buildInputs ++
              self.packages.${system}."${pname}-qtenv".buildInputs;
          propagatedNativeBuildInputs =
              self.packages.${system}."${pname}-runtime".propagatedNativeBuildInputs ++
              self.packages.${system}."${pname}-qtenv".propagatedNativeBuildInputs;

          shellHook = ''
            source setenv
          '';
        };

        installed = oppPkgs.stdenv.mkDerivation {
          name = "${pname}-${sversion} installed";
          packages = [
            self.packages.${system}."${pname}-runtime"
            self.packages.${system}."${pname}-qtenv"
            self.packages.${system}."${pname}-samples-git"
            self.packages.${system}."${pname}-doc"
            self.packages.${system}."${pname}-ide"
          ];

          propagatedNativeBuildInputs = self.packages.${system}."${pname}-runtime".propagatedNativeBuildInputs;

          shellHook = ''
            source ${self.packages.${system}."${pname}-runtime"}/setenv 1>/dev/null
            source ${self.packages.${system}."${pname}-qtenv"}/nix-support/setup-hook
          '';
        };
      };

      apps = rec {
        "${pname}-samples" = {
          type = "app";
          program = "${self.packages.${system}."${pname}-samples-git"}/bin/opp_samples";
        };

        default = apps."${pname}-samples";
      };
    }) // {
      templates = rec {
        samples = {
          path = ./samples;
          description = "OMNeT++ Samples";
          welcomeText =
            ''This template contains OMNeT++ sample projects. Change to the project
            you want to build and create an OMNeT++ development shell.

            Use:

              cd <dirname>
              nix develop -i github:omnetpp/omnetpp/flake/omnetpp-6.0.x
              opp_makemake -f
              make
              ./executable_name
            '';
        };

        default = samples;
      };
    };
}