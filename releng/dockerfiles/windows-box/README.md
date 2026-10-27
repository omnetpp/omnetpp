# windows-box

Arch Linux based distrobox for interactively testing the OMNeT++ Windows distribution
(`*-windows-x86_64.7z`) under Wine. Entering the box drops into the distribution's own
MSYS2 clang64 shell, started through its `mingwenv.cmd`.

## Setup

    ./build
    distrobox assemble create --file distrobox.ini

The box uses a separate home (`~/distrobox/windows-box`) holding the Wine prefix; the real
home is still mounted at its usual path. To publish the image:

    docker push ghcr.io/omnetpp/windows-box:latest

## Usage

Unpack the distribution on the host (no spaces in the path), then:

    cd ~/omnetpp-win/omnetpp-6.5.0
    distrobox enter windows-box

The tree is found by walking up from the current directory to a directory containing
`mingwenv.cmd`, falling back to `$OMNETPP_WIN_ROOT`. The first start creates the Wine prefix,
and the first shell of a fresh tree unpacks the clang64 toolchain (takes a while).

- `opp-win-shell` - open the MSYS2 shell (also run automatically on enter)
- `opp-win-shell --direct` - start `bash.exe` directly, bypassing `cmd.exe`/`mingwenv.cmd`
- `opp_ide` (inside the MSYS2 shell) - start the IDE; keep that shell open while the IDE runs, Wine
  terminates processes attached to the console when it closes
- `distrobox enter windows-box -- bash` or `OPP_NO_MSYS=1` - plain Linux shell for debugging

Wine's `cmd.exe` cannot run the argument re-splitting in `msys2_shell.cmd`, so extra shell
arguments are dropped unless `--direct` is used, e.g. `opp-win-shell --direct -c 'make -j8'`.
The command line still passes through `cmd.exe`: avoid `| & < >` inside double quotes.

msys2-runtime 3.6.1 (toolchain 250612) crashes on every fork under Wine; 3.5.7 and 3.6.11 work.

Wine runs with `WINEDEBUG=-all`; override it (e.g. `WINEDEBUG=fixme+all`) when diagnosing.
