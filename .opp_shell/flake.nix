{
  description = "OMNeT++ development environment (NixOS)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };

        # Dev tools required for building
        devTools = with pkgs; [
          # Common utilities for the shell
          bashInteractive
          which

          # Build Tools
          ccache
          gcc
          lld
          lldb
          gnumake
          autoconf
          bison
          flex
          perl
          pkg-config
          xdg-utils
          swig
          uv
        ];

        ideDependencies = with pkgs; [
          # for the eclipse IDE
          adw-gtk3
          fontconfig
          freetype
          glib
          glib-networking
          gtk3
          gsettings-desktop-schemas
          shared-mime-info
          webkitgtk_4_1
          libX11
          libXrender
          libXtst
          zlib

          # additional libraries for vscode
          alsa-lib
          atk
          cairo
          cups
          expat
          dbus
          libgbm
          libxkbcommon
          nspr
          nss
          pango
          udev
          libXcomposite
          libXdamage
          libXext
          libXfixes
          libXrandr
          libxcb

        ];

        # System libraries required by binary files (omnetpp or python wheels)
        runtimeLibs = with pkgs; [
          # libstdc++
          stdenv.cc.cc.lib

          # additional tools used in the opp project
          graphviz
          doxygen

          # Common core libraries
          zlib
          elfutils elfutils.dev
          # libffi
          # openssl
          # icu
          libxml2 libxml2.dev

          # python dependencies
          (python3.withPackages (ps: with ps; [
            packaging pip
            matplotlib numpy pandas scipy ipython
          ]))

          # UI & Graphics
          qt6.qtbase
          qt6.qtwayland
          qt6.qtsvg
          openscenegraph
          libGL libGL.dev

          # inet
          ffmpeg-headless ffmpeg-headless.dev
          z3 z3.dev
        ];

        nativeEnv = (pkgs.mkShell.override { stdenv = pkgs.clangStdenv; }) {
          name = "opp_shell"; # important because it is used in install.sh to skip dependency installation
          hardeningDisable = [ "fortify" ];

          nativeBuildInputs = devTools;
          buildInputs = runtimeLibs;

          # NIX_LD handles unpatched executables (like the IDE)
          # this requires programs.nix-ld.enable = true; in the global nixos config
          # alternatively ideDependencies could be added to the LD_LIBRARY_PATH in the shellHook
          NIX_LD = pkgs.lib.fileContents "${pkgs.stdenv.cc}/nix-support/dynamic-linker";
          NIX_LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath (ideDependencies);

          shellHook = ''
            export name="opp_shell" # important because it is used in install.sh to skip dependency installation

            # Ensure project-built binaries find their runtime dependencies
            export LD_LIBRARY_PATH="${pkgs.lib.makeLibraryPath (runtimeLibs)}"

            # Disable Nix hardening specifically for fortify to avoid the warning in debug builds (-O0)
            export NIX_HARDENING_ENABLE="stackprotector,format,relro,bindnow,pic"
            export GIO_EXTRA_MODULES="${pkgs.glib-networking}/lib/gio/modules"
            export XDG_DATA_DIRS="${pkgs.gtk3}/share/gsettings-schemas/${pkgs.gtk3.name}:${pkgs.gsettings-desktop-schemas}/share/gsettings-schemas/${pkgs.gsettings-desktop-schemas.name}:$XDG_DATA_DIRS"

            if [[ -z "$__omnetpp_root_dir" ]]; then
              _priv_dir=.
            else
              _priv_dir=$__omnetpp_root_dir
            fi

            if [[ -f $_priv_dir/setenv && -z "$skip_setenv" ]]; then
              source $_priv_dir/setenv
            fi
            unset _priv_dir
          '';
        };

      in
      {
        devShells.default = nativeEnv;
      });
}
