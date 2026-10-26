#!/usr/bin/env bash

# Stop on the first error
set -e

# colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[0;37m'
RESET='\033[0m'

# Global flags
assume_yes=false
no_3d=false
no_gui=false
no_build=false
quiet_flag=false
PYTHON3=${PYTHON3:-python3}
PYTHON3_DOTLESS=${PYTHON3//./}

# print the usage and supported options
print_usage() {
    echo "Install OMNeT++ dependencies then configure and build OMNeT++."
    echo
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h            Display this help message"
    echo "  -q            Quiet mode - do not write anything to output"
    echo "  -y            Assume 'yes' answer for all interactive prompts"
    echo "  --no-gui      Do not install GUI related dependencies for Qtenv and the IDE (implies --no-3d)"
    echo "  --no-3d       Do not install 3D-related dependencies for Qtenv OpenSceneGraph support"
    echo "  --no-build    Do not configure and build OMNeT++ after installing dependencies"
}

# prompt user for yes/no response
ask_user() {
    if $assume_yes; then
        return 0
    else
        local yn
        while true; do
            read -p "$1 (y/n): " yn
            case "$yn" in
                [Yy]* ) return 0;;
                [Nn]* ) return 1;;
                * ) echo -e "Please answer yes or no.";;
            esac
        done
    fi
}

# quiet echo function - only output if not in quiet mode
echo_q() {
    if ! $quiet_flag; then
        echo "$@"
    fi
}

# echo the command and then run it.
echo_run() {
    local cmd="$*"
    if ! $quiet_flag; then
        echo -e "${YELLOW}\$ $cmd${RESET}"
        echo
    fi
    $cmd
}

# echo the command, ask for confirmation and then run it as root calling either sudo or su.
echo_root_run() {
    local cmd="$*"
    local cmd_root=""
    if [[ "$(whoami)" != "root" ]]; then
        if [[ "$(command -v sudo)" != "" ]]; then
            cmd_root="sudo sh -c '$cmd'"
        elif [[ "$(command -v su)" != "" ]]; then
            cmd_root="su -c \"$cmd\""
        else
            cmd_root="$cmd"
        fi
    else
        cmd_root="$cmd"
    fi
        echo_q -e "${GREEN}We are about to run the following command:${RESET}"
        echo_q
        echo_q -e "${YELLOW}\$ $cmd_root${RESET}"
        echo_q
    if ask_user "Do you want to run it (y), skip it (n) or quit, to run the command manually (^C)?" ; then
        echo_q
        eval $cmd_root
    fi
}

# locate uv or install it for the current user (pipx is preferred because 'pip --user' is refused on PEP 668 distros)
find_or_install_uv() {
    local user_bin
    user_bin="$($PYTHON3 -m site --user-base || true)/bin" # exits with 1 when the user site is disabled (e.g. NixOS, inside a venv)
    for UV in "$(command -v uv)" "$HOME/.local/bin/uv" "$user_bin/uv"; do
        [[ -n "$UV" && -x "$UV" ]] && return
    done

    echo_q
    echo_q -e "${GREEN}*** Installing uv for the current user ***${RESET}"
    echo_q
    if [[ "$(command -v pipx)" != "" ]]; then
        echo_run pipx install uv
    else
        echo_run $PYTHON3 -m pip install --user uv
    fi
    for UV in "$HOME/.local/bin/uv" "$user_bin/uv"; do
        if [[ -x "$UV" ]]; then
            echo_q -e "${YELLOW}uv was installed as '$UV'. Add '$(dirname $UV)' to your PATH to use it from the command line.${RESET}"
            return
        fi
    done
    echo_q -e "${RED}Could not install uv. Install it manually (see https://docs.astral.sh/uv/) and re-run this script.${RESET}"
    exit 1
}

# set up Python virtual environment and dependencies
install_python_deps() {
    find_or_install_uv
    if [[ ! -f .venv/bin/activate ]]; then
        # Create a virtual environment
        echo_q
        echo_q -e "${GREEN}*** Creating a python virtual environment in '.venv' ***${RESET}"
        echo_q
        # the venv must use the same interpreter that OMNeT++ is configured with, never one downloaded by uv
        echo_run env UV_PYTHON_DOWNLOADS=never $UV venv --python "$PYTHON3" --seed --prompt "$(basename $PWD)/.venv" .venv
    fi
    echo_q -e "${GREEN}*** Activating python virtual environment ***${RESET}"
    source .venv/bin/activate

    echo_q
    echo_q -e "${GREEN}*** Installing required python packages ***${RESET}"
    echo_q
    echo_run $UV pip install --python .venv/bin/python -r python/requirements.txt
}

# install dependencies
install_deps() {
    if [[ "$(uname)" == "Linux" ]]; then
        # detect the operating system
        if [[ -f /etc/os-release ]]; then
            source /etc/os-release
        fi

        # detect the package manager
        if [[ "$(command -v apt)" != "" ]]; then # e.g. ID=ubuntu (tested versions: 22.04, 24.04, 25.04)
            # apt-get is used on debian (i.e. Ubuntu,  etc.)
            packages="make diffutils pkg-config ccache clang lld gdb lldb bison flex perl sed gawk $PYTHON3 $PYTHON3-pip $PYTHON3-venv $PYTHON3-dev pipx libxml2-dev zlib1g-dev doxygen graphviz xdg-utils libdw-dev"

            if ! $no_gui; then
                packages="$packages qt6-base-dev qt6-base-dev-tools qmake6 libqt6svg6 qt6-wayland libwebkit2gtk-4.1-0"
            fi

            if ! $no_3d; then
                packages="$packages libopenscenegraph-dev"
            fi

            echo_root_run "apt update; DEBIAN_FRONTEND=noninteractive apt install -y $packages ; apt clean"

        elif [[ "$(command -v dnf)" != "" && "$ID" == "fedora" ]]; then # e.g. ID=fedora (tested versions: 42)
            # dnf is used on fedora
            packages="make ccache clang awk lld lldb gdb bison flex perl $PYTHON3-devel $PYTHON3-pip uv libxml2-devel zlib-devel doxygen graphviz xdg-utils libdwarf-devel"

            if ! $no_gui; then
                packages="$packages qt6-qttools-devel qt6-qtbase-devel qt6-qtsvg qt6-qtwayland webkit2gtk4.1"
            fi

            if ! $no_3d; then
                packages="$packages OpenSceneGraph-devel"
            fi

            echo_root_run "dnf install -y $packages ; dnf clean packages"

        elif [[ "$(command -v dnf)" != "" &&  ( "$ID" == "almalinux" || "$ID" == "rhel" || "$ID" == "rocky" || "$ID" == "centos" ) ]]; then # (tested versions 9, 10)
            packages="make ccache clang lld lldb gdb bison flex perl $PYTHON3-devel $PYTHON3-pip libxml2-devel zlib-devel graphviz xdg-utils elfutils-devel"

            if ! $no_gui; then
                packages="$packages qt6-qttools-devel qt6-qtbase-devel qt6-qtsvg qt6-qtwayland" # 10: webkit2gtk4.1 9:webkit2gtk3
            fi

            if ! $no_3d; then
                echo_q -e "${RED}This Linux distibution does not support OpenSceneGraph. Please disable the 3D support with --no-3d.${RESET}"
                exit 1
            fi

            echo_root_run "dnf install -y epel-release && dnf install -y $packages ; dnf clean packages"

        elif [[ "$(command -v zypper)" != "" && ( "$ID" == "opensuse-leap" || "$ID" == "opensuse-tumbleweed") ]]; then
            # zypper is used on OpenSUSE. 

            # KLUDGE: Force using python 3.12 on leap 15.x because the default python version is 3.6 which is too old.
            if [[ "$PYTHON3" == "python3" && "$ID" == "opensuse-leap" && "$VERSION" == 15.* ]]; then
                PYTHON3="python3.12"
                PYTHON3_DOTLESS="python312"
            fi

            packages="make ccache clang lld lldb gdb bison gawk flex perl $PYTHON3_DOTLESS-devel $PYTHON3_DOTLESS-pip doxygen graphviz xdg-utils libdw-devel"
            if [[ "$ID" == "opensuse-tumbleweed" ]]; then
                packages="$packages uv"
            fi

            if ! $no_gui; then
                packages="$packages qt6-base-devel qt6-wayland libQt6Svg6 libwebkit2gtk-4_1-0"
            fi

            if ! $no_3d; then
                packages="$packages libOpenSceneGraph-devel OpenSceneGraph-plugins"
            fi

            echo_root_run "zypper install -y $packages ; zypper clean"

        elif [[ "$(command -v pacman)" != "" ]]; then
            # pacman is used on Arch Linux
            packages="make diffutils ccache clang pkgconf lld lldb gdb bison gawk flex perl python python-pip uv libxml2 zlib doxygen graphviz xdg-utils libdwarf"

            if ! $no_gui; then
                packages="$packages qt6-base qt6-svg qt6-wayland webkit2gtk-4.1"
            fi

            if ! $no_3d; then
                packages="$packages openscenegraph"
            fi

            echo_root_run "pacman -Syu --needed --noconfirm $packages ; pacman -Scc --noconfirm"

        elif [[ "$ID" == "nixos" ]]; then
            echo_q -e "${YELLOW}NixOS detected, dependencies should already be installed.${RESET}"
            echo_q
            # do not install python dependencies
        else
            echo_q -e "${RED}Package manager (apt, dnf, zypper, pacman) not detected.\nSee 'doc/InstallGuide.pdf' and install the required packages manually.${RESET}"
            exit 1
        fi

        install_python_deps
    fi

    if [[ "$(uname)" == "Darwin" ]]; then
        if [[ "$(command -v brew)" == "" ]]; then
            echo_q -e "${RED}HOMEBREW (https://brew.sh) not detected. This script requires 'brew' to be installed and activated.${RESET}"
            exit 1
        fi

        packages="bison ccache llvm flex perl python@3 uv make pkg-config doxygen graphviz"

        if ! $no_gui; then
            packages="$packages qt@6 qtkeychain"
        fi

        if ! $no_3d; then
            packages="$packages openscenegraph"
        fi

        echo_run "brew install --formula $packages"

        # Make sure that the packages just installed are available (and not some other versions that are also installed on the system)
        # This is especially important because python3 might be already installed from a differenet source
        # We must ensure the virtual environment is created by the python package just installed from homebrew
        if [ $(uname -m) = "arm64" ]; then
            eval "$(/opt/homebrew/bin/brew shellenv)"
        else
            eval "$(/usr/local/bin/brew shellenv)"
        fi

        install_python_deps
    fi

    if [[ "$(uname -o)" == *"Msys"* ]]; then
        packages="autoconf2.72 bash bash-completion bison bzip2 ccache coreutils curl \
        dash diffutils dos2unix doxygen file filesystem findutils flex gawk gcc-libs \
        git grep gzip inetutils info less lndir make man-db mintty msys2-keyring \
        msys2-launcher msys2-runtime ncurses p7zip pacman pacman-mirrors pactoys-git \
        patch pax-git perl pkgfile rebase sed tar tftp-hpa time tzcode unzip \
        util-linux which zip\
        $MINGW_PACKAGE_PREFIX-clang \
        $MINGW_PACKAGE_PREFIX-pkgconf \
        $MINGW_PACKAGE_PREFIX-lld \
        $MINGW_PACKAGE_PREFIX-lldb \
        $MINGW_PACKAGE_PREFIX-gdb \
        $MINGW_PACKAGE_PREFIX-gcc-compat \
        $MINGW_PACKAGE_PREFIX-libxml2 \
        $MINGW_PACKAGE_PREFIX-python \
        $MINGW_PACKAGE_PREFIX-python-pip \
        $MINGW_PACKAGE_PREFIX-python-numpy \
        $MINGW_PACKAGE_PREFIX-python-matplotlib \
        $MINGW_PACKAGE_PREFIX-python-pandas \
        $MINGW_PACKAGE_PREFIX-python-seaborn \
        $MINGW_PACKAGE_PREFIX-python-scipy \
        $MINGW_PACKAGE_PREFIX-python-ipython"

        # packages="$packages $MINGW_PACKAGE_PREFIX-graphviz" # temporarily disabled because of size

        if ! $no_gui; then
            packages="$packages $MINGW_PACKAGE_PREFIX-qt6-base $MINGW_PACKAGE_PREFIX-qt6-svg $MINGW_PACKAGE_PREFIX-qt6-imageformats"
        fi

        if ! $no_3d; then
            packages="$packages $MINGW_PACKAGE_PREFIX-OpenSceneGraph"
        fi

        echo_run "pacman -S --needed --noconfirm $packages"
        echo_run "pacman -Scc --noconfirm"

        # we are using pacman to install all needed python modules
        # so installing a virtual environment is not needed
    fi
}

##########################
# installer main program #
##########################

# Parse command line options
while [[ $# -gt 0 ]]; do
    case "$1" in
        -q)
            quiet_flag=true
            shift
            ;;
        -y)
            assume_yes=true
            shift
            ;;
        -h)
            print_usage
            exit 0
            ;;
        --no-gui)
            no_gui=true
            no_3d=true
            shift
            ;;
        --no-3d)
            no_3d=true
            shift
            ;;
        --no-build)
            no_build=true
            shift
            ;;
        *)
            echo_q "Invalid option: $1" >&2
            print_usage
            exit 1
            ;;
    esac
done

# print intro message
echo_q -e "\
${GREEN}This script will install the OMNeT++ development environment.
It will attempt to detect your operating system and package manager,
install the required dependencies, install a python virtual environment
and then configure and build OMNeT++.

Supported operating systems [and package managers]:
- Linux (Ubuntu/Debian [apt], Fedora/AlmaLinux/RHEL/RockyLinux [dnf],
        OpenSuse-tumbleweed/leap [zypper], ArchLinux [pacman], NixOS [nix])
- macOS 16 [homebrew]
- Windows 11/Msys2 [pacman]

${BLUE}NOTE: This is an experimental script. If it doesn't work, please open
an issue at https://github.com/omnetpp/omnetpp/issues and read
'InstallGuide.pdf' for manual installation instructions.
${RESET}"

if ! ask_user "Continue installing OMNeT++?" ; then
    echo_q -e "${GREEN}\nPlease read the installation manual (InstallGuide.pdf) for futher instructions.${RESET}"
    exit 0
fi

# activate the OMNeT++ environment (including the python virtualenv)
echo_q
echo_run source setenv -q

echo_q -e "${GREEN}*** Installing dependencies ***${RESET}"
echo_q
install_deps

# configure the simulator
echo_q
echo_q -e "${GREEN}*** Configuring ***${RESET}"
echo_q
# disable some config options based on the provided command-line flags
if $no_gui; then CONFIGOPTS="$CONFIGOPTS WITH_QTENV=no"; fi
if $no_3d; then CONFIGOPTS="$CONFIGOPTS WITH_OSG=no"; fi

if ! $no_build; then
    echo_run source setenv -q
    echo_run ./configure $CONFIGOPTS

    # build
    echo_q
    echo_q -e "${GREEN}*** Compiling ***${RESET}"
    echo_q
    echo_run make -j16
else
    echo_q -e "${YELLOW}Skipping configuration and build as requested.${RESET}"
fi

echo_q
echo_q -e "${GREEN}*** Installation completed ***${RESET}"
echo_q
echo_q -e "Execute '${YELLOW}source setenv${RESET}' to activate the OMNeT++ environment."
echo_q
