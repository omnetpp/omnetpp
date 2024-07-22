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
                * ) echo "Please answer yes or no.";;
            esac
        done
    fi
}

# echo the command and then run it.
echo_run() {
    local cmd="$*"
    echo -e "${YELLOW}\$ $cmd\n${RESET}"
    $cmd
}

# echo the command, ask for confirmation and then run it.
echo_ask_run() {
    local cmd="$*"
    echo -e "${GREEN}We are about to run the following command:\n${RESET}"
    echo -e "${YELLOW}\$ $cmd\n${RESET}"
    if ask_user "Do you want to run it (y), skip it (n) or quit, to run the command manually (^C)?" ; then
        $cmd
    fi
}

# set up Python virtual environment and dependencies
install_python_deps() {
    # Check if the virtual environment exists
    if [[ ! -f ".venv/bin/activate" ]]; then
        # Create a virtual environment if it doesn't exist
        echo
        echo -e "${GREEN}*** Creating a python virtual environment in '.venv' ***${RESET}"
        echo
        echo_run python3 -m venv .venv
    fi

    source .venv/bin/activate

    # Upgrade and install required Python packages within the virtual environment
    export PIP_DISABLE_PIP_VERSION_CHECK=1
    echo
    echo -e "${GREEN}*** Installing required python packages ***${RESET}"
    echo
    echo_run python3 -m pip install -r python/requirements.txt
}

# print the usage and supported options
print_usage() {
    echo "Install OMNeT++ dependencies then configure and build OMNeT++."
    echo
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h            Display this help message"
    echo "  -y            Assume 'yes' answer for all interactive prompts"
    echo "  --no-gui      Do not install GUI related dependencies for Qtenv and the IDE (implies --no-3d)"
    echo "  --no-3d       Do not install 3D-related dependencies for Qtenv OpenSceneGraph support"
}

# Parse command line options using getopt
OPTIONS=$(getopt -o yh --long no-3d --long no-gui -n "$0" -- "$@")
eval set -- "$OPTIONS"
while true; do
    case "$1" in
        -h)
            print_usage
            exit 0
            ;;
        -y)
            assume_yes=true
            shift
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
        --)
            shift
            break
            ;;
        *)
            echo "Invalid option: $1" >&2
            print_usage
            exit 1
            ;;
    esac
done

# print intro message
echo -e "\
${GREEN}This script will install the OMNeT++ development environment.
It will attempt to detect your operating system and package manager,
install the required dependencies, install a python virtual environment
and then configure and build OMNeT++.

Supported operating systems (and package managers): 
- Linux (apt, dnf, yum, zypper)
- macOS (homebrew)
- Windows (msys2/pacman)

${BLUE}NOTE: This is an experimental script. If it doesn't work, please open
an issue at https://github.com/omnetpp/omnetpp/issues and read
'InstallGuide.pdf' for manual installation instructions.
${RESET}"

if ! ask_user "Continue installing OMNeT++?" ; then
    echo -e "${GREEN}\nPlease read the installation manual (InstallGuide.pdf) for futher instructions.${RESET}"
    exit 0
fi

echo
echo -e "${GREEN}*** Installing dependencies ***${RESET}"
echo

if [[ "$(uname)" == "Linux" ]]; then
    # detect the operating system
    if [[ -f /etc/os-release ]]; then
        source /etc/os-release
    fi

    # detect the package manager    
    if [[ "$(command -v apt-get)" != "" && "$ID_LIKE" == *"debian"* ]]; then
        # apt-get is used on debian (i.e. Ubuntu,  etc.)
        packages="build-essential ccache clang lld gdb bison flex perl \
            python3 python3-pip libpython3-dev libxml2-dev zlib1g-dev \
            doxygen graphviz xdg-utils libdw-dev"

        if ! $no_gui; then
            packages="$packages qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools libqt5opengl5-dev libwebkit2gtk-4.1-0"
        fi

        if ! $no_3d; then
            packages="$packages libopenscenegraph-dev"
        fi

        echo_ask_run sudo apt-get install -y $packages

    elif [[ "$(command -v dnf)" != "" ]]; then
        # dnf is used on fedora
        packages="make ccache clang lld bison flex perl \
            python3-devel python3-pip libxml2-devel zlib-devel \
            doxygen graphviz xdg-utils libdwarf-devel"

        if ! $no_gui; then
            packages="$packages qt5-qtbase-devel webkit2gtk4.1"
        fi

        if ! $no_3d; then
            packages="$packages OpenSceneGraph-devel"
        fi

        echo_ask_run sudo dnf install -y $packages

    elif [[ "$(command -v yum)" != "" ]]; then
        # yum is used on redhat
        packages="make ccache clang lld bison flex perl \
            python3-devel python3-pip libxml2-devel zlib-devel \
            doxygen graphviz xdg-utils libdwarf-devel"

        if ! $no_gui; then
            packages="$packages qt5-qtbase-devel"
        fi

        echo_ask_run su -c 'yum install -y $packages'

    elif [[ "$(command -v zypper)" != "" ]]; then
        # zypper is used on OpenSUSE
        packages="make ccache clang lld bison flex perl \
            python3-devel python3-pip libxml2-devel zlib-devel\
            doxygen graphviz xdg-utils libdw-devel"
        
        if ! $no_gui; then
            packages="$packages qt5-qtbase-devel libwebkit2gtk-4_1-0"
        fi

        if ! $no_3d; then
            packages="$packages OpenSceneGraph-devel"
        fi

        echo_ask_run sudo zypper install -y $packages

    else
        echo -e "${RED}Package manager (apt-get, dnf, yum, zypper) not detected.\nSee 'doc/InstallGuide.pdf' and install the required packages manually.${RESET}"
        exit 1
    fi

    install_python_deps
fi

if [[ "$(uname)" == "Darwin" ]]; then
    if [[ "$(command -v brew)" == "" ]]; then
        echo -e "${RED}HOMEBREW (https://brew.sh) not detected. This script requires 'brew' to be installed and activated.${RESET}"
        exit 1
    fi

    packages="bison ccache flex perl python@3 make pkg-config doxygen graphviz"

    if ! $no_gui; then
        packages="$packages qt@5"
    fi

    if ! $no_3d; then
        packages="$packages openscenegraph"
    fi

    echo_run brew install $packages

    install_python_deps
fi

if [[ "$(uname -o)" == *"Msys"* ]]; then
    packages="autoconf2.72 bash bash-completion bison bzip2 ccache coreutils curl \
    dash diffutils dos2unix doxygen file filesystem findutils flex gawk gcc-libs \
    git grep gzip inetutils info less lndir make man-db mintty msys2-keyring \
    msys2-launcher msys2-runtime ncurses p7zip pacman pacman-mirrors pactoys-git \
    patch pax-git perl pkgfile rebase sed swig tar tftp-hpa time tzcode unzip \
    util-linux which zip\
    $MINGW_PACKAGE_PREFIX-clang \
    $MINGW_PACKAGE_PREFIX-pkg-config \
    $MINGW_PACKAGE_PREFIX-lld \
    $MINGW_PACKAGE_PREFIX-lldb \
    $MINGW_PACKAGE_PREFIX-gdb \
    $MINGW_PACKAGE_PREFIX-libxml2 \
    $MINGW_PACKAGE_PREFIX-python \
    $MINGW_PACKAGE_PREFIX-python-pip \
    $MINGW_PACKAGE_PREFIX-python-numpy \
    $MINGW_PACKAGE_PREFIX-python-matplotlib \
    $MINGW_PACKAGE_PREFIX-python-pandas \
    $MINGW_PACKAGE_PREFIX-python-seaborn \
    $MINGW_PACKAGE_PREFIX-python-scipy"

    # packages="$packages $MINGW_PACKAGE_PREFIX-graphviz" # temporarily disabled because of size

    if ! $no_gui; then
        packages="$packages $MINGW_PACKAGE_PREFIX-qt5-base $MINGW_PACKAGE_PREFIX-qt5-svg $MINGW_PACKAGE_PREFIX-qt5-imageformats"
    fi

    if ! $no_3d; then
        packages="$packages $MINGW_PACKAGE_PREFIX-OpenSceneGraph"
    fi

    pacman -S --needed --noconfirm $packages
fi

# common part on all OSes

# Activate the OMNeT++ environment (including the python virtualenv)
echo
echo_run source setenv
echo
echo -e "${GREEN}*** Configuring ***${RESET}"
echo
if $no_gui; then CONFIGOPTS="$CONFIGOPTS WITH_QTENV=no"; fi
if $no_3d; then CONFIGOPTS="$CONFIGOPTS WITH_OSG=no"; fi
echo_run ./configure $CONFIGOPTS
echo
echo -e "${GREEN}*** Compiling ***${RESET}"
echo
echo_run make -j16
echo
echo -e "${GREEN}*** Installation completed ***${RESET}"
echo
echo "Execute 'source setenv' to activate the OMNeT++ environment."
