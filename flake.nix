{
  description = "OMNeT++ Discrete Event Simulator";

  inputs = {
    # nixpkgs.url = "nixpkgs/nixos-22.05";
    nixpkgs.url = "nixpkgs/nixos-unstable";
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
      sha256_core = "sha256-ftmYmDSE3zGm9pLyi3QR7wDWinYwAN8rQog0WietSCk="; # sha256 of the core tarball
    in rec {
      # set different defaults for creating packages.
      oppPkgs =  pkgs // {
          stdenv = pkgs.llvmPackages_14.stdenv;  # use clang14 instead of the standard g++ compiler
          lld = pkgs.lld_14;
          python3 = pkgs.python310;
          inherit (pkgs.qt515) qtbase qtsvg;
          texlive = pkgs.texlive.combined.scheme-full;
          callPackage = pkgs.newScope oppPkgs;
      };

      packages = rec {
        default = packages."${pname}-runtime";

        "${pname}-runtime" = oppPkgs.callPackage ./nix-support/mkOmnetppDerivation.nix {
          pname = "${pname}-runtime";
          version = sversion;
          src = pkgs.fetchzip {
            url = "${tarball_base_url}-core.tgz";
            sha256 = sha256_core;
          };
          WITH_QTENV = false;
        };

        "${pname}-qtenv" = oppPkgs.callPackage ./nix-support/mkOmnetppQtenvDerivation.nix {
          pname = "${pname}-qtenv";
          version = sversion;
          src = pkgs.fetchzip {
            url = "${tarball_base_url}-core.tgz";
            sha256 = sha256_core;
          };
          omnetpp = self.packages.${system}."${pname}-runtime"; # depends on the base omnetpp runtime component
        };

        "${pname}-doc" = oppPkgs.callPackage ./nix-support/mkOmnetppDocDerivation.nix {
          pname = "${pname}-doc";
          version = sversion;
          src = pkgs.fetchzip {
            url = "${tarball_base_url}-core.tgz";
            sha256 = sha256_core;
          };
        };

        "${pname}-ide" = oppPkgs.callPackage ./nix-support/mkOmnetppIDEDerivation.nix {
          pname = "${pname}-ide";
          version = sversion;
          src = pkgs.fetchzip {
            url = "${tarball_base_url}-linux-x86_64.tgz";
            sha256 = "sha256-968zGE+C7G5nYNw7jlcVbs5QwXuPy8dTMQQzn5mLJyU=";
          };
          omnetpp = self.packages.${system}."${pname}-runtime";
        };

        "${pname}-runtime-git" = oppPkgs.callPackage ./nix-support/mkOmnetppDerivation.nix {
          pname = "${pname}-runtime-git";
          version = gversion;
          src = self;
          WITH_QTENV = true;
        };

        "${pname}-samples-git" = oppPkgs.callPackage ./nix-support/mkOmnetppSamplesDerivation.nix {
          pname = "${pname}-samples-git";
          version = gversion;
          src = self;
          omnetpp = self.packages.${system}."${pname}-git";
        };

        "${pname}-doc-git" = oppPkgs.callPackage ./nix-support/mkOmnetppDocGitDerivation.nix {
          pname = "${pname}-doc-git";
          version = gversion;
          src = self;
        };
      };

      devShells = rec {
        ${pname} = oppPkgs.stdenv.mkDerivation {
          name = "${pname}";
          # This shell is intended to be used by users writing models.
          # It includes also the omnetpp package as a dependency.
          buildInputs = [ 
            self.packages.${system}."${pname}-runtime" 
            # self.packages.${system}."${pname}-samples-git" 
            # self.packages.${system}."${pname}-doc"
            # self.packages.${system}."${pname}-ide"
          ];
          shellHook = ''
            source ${self.packages.${system}.default}/setenv
            source ${self.packages.${system}."${pname}-qtenv"}/setenv
          '';
        };

        "${pname}-dev" = oppPkgs.stdenv.mkDerivation {
          name = "${pname}-dev";
          # use the same toolchain as omnetpp, but nothing else is included in this shell
          buildInputs = self.packages.${system}.default.buildInputs;
          propagatedNativeBuildInputs = self.packages.${system}.default.propagatedNativeBuildInputs;
        };

        default = devShells.${pname};
      };

      apps = rec {
        "${pname}-samples" = {
          type = "app";
          program = "${self.packages.${system}."${pname}-samples-git"}/bin/.opp_samples_wrapper";
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