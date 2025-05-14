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
PYTHON3=python3

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
    echo -e "${YELLOW}\$ $cmd${RESET}"
    echo
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
    echo -e "${GREEN}We are about to run the following command:${RESET}"
    echo
    echo -e "${YELLOW}\$ $cmd_root${RESET}"
    echo
    if ask_user "Do you want to run it (y), skip it (n) or quit, to run the command manually (^C)?" ; then
        echo
        eval $cmd_root
    fi
}

# set up Python virtual environment and dependencies
install_python_deps() {
    # Create a virtual environment
    echo
    echo -e "${GREEN}*** Creating a python virtual environment in '.venv' ***${RESET}"
    echo
    echo_run $PYTHON3 -m venv .venv --upgrade-deps --clear --prompt "$(basename $PWD)/.venv"

    echo -e "${GREEN}*** Activating python virtual environment ***${RESET}"
    source .venv/bin/activate

    # Upgrade and install required Python packages within the virtual environment
    export PIP_DISABLE_PIP_VERSION_CHECK=1
    echo
    echo -e "${GREEN}*** Installing required python packages ***${RESET}"
    echo
    echo_run python3 -m pip install -r python/requirements.txt
}

# install dependencies
install_deps() {
    if [[ "$(uname)" == "Linux" ]]; then
        # detect the operating system
        if [[ -f /etc/os-release ]]; then
            source /etc/os-release
        fi

        # detect the package manager
        if [[ "$(command -v apt)" != "" ]]; then # e.g. ID=ubuntu
            # apt-get is used on debian (i.e. Ubuntu,  etc.)
            packages="build-essential pkg-config ccache clang lld gdb bison flex perl python3 python3-pip python3-venv python3-dev libxml2-dev zlib1g-dev doxygen graphviz xdg-utils libdw-dev"

            if ! $no_gui; then
                packages="$packages qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools libqt5opengl5-dev libwebkit2gtk-4.1-0"
            fi

            if ! $no_3d; then
                packages="$packages libopenscenegraph-dev"
            fi

            echo_root_run "DEBIAN_FRONTEND=noninteractive apt install -y $packages ; apt clean"

        elif [[ "$(command -v dnf)" != "" && "$ID" == "fedora" ]]; then # e.g. ID=fedora
            # dnf is used on fedora
            packages="make ccache clang awk lld gdb libstdc++-static bison flex perl python3-devel python3-pip libxml2-devel zlib-devel doxygen graphviz xdg-utils libdwarf-devel"

            if ! $no_gui; then
                packages="$packages qt5-qtbase-devel qt5-qtsvg webkit2gtk4.1"
            fi

            if ! $no_3d; then
                packages="$packages OpenSceneGraph-devel"
            fi

            echo_root_run "dnf install -y $packages ; dnf clean packages"

        elif [[ "$(command -v dnf)" != "" &&  ( "$ID" == "almalinux" || "$ID" == "rhel" ) ]]; then
            packages="make clang lld gdb bison flex perl python3-devel python3-pip libxml2-devel zlib-devel graphviz xdg-utils elfutils-devel"

            if ! $no_gui; then
                packages="$packages qt5-qtbase-devel qt5-qtsvg qt5-qtwayland webkit2gtk3"
            fi

            if ! $no_3d; then
                echo -e "${RED}This Linux distibution does not support OpenSceneGraph. Please disable the 3D support with --no-3d.${RESET}"
                exit 1
            fi

            echo_root_run "dnf install -y $packages ; dnf clean packages"

        elif [[ "$(command -v zypper)" != "" && "$ID" == "opensuse-tumbleweed" ]]; then
            # zypper is used on OpenSUSE. leap is not supported because of old (3.6) python
            packages="make ccache clang lld gdb bison gawk flex perl python3-devel python3-pip libxml2-devel zlib-devel doxygen graphviz xdg-utils libdw-devel"

            if ! $no_gui; then
                packages="$packages libqt5-qtbase-devel libqt5-qtwayland libQt5Svg5 libwebkit2gtk-4_1-0"
            fi

            if ! $no_3d; then
                packages="$packages libOpenSceneGraph-devel"
            fi

            echo_root_run "zypper install -y $packages ; zypper clean"

        elif [[ "$(command -v pacman)" != "" ]]; then
            # pacman is used on Arch Linux
            packages="make diffutils ccache clang lldb pkgconf lld gdb bison gawk flex perl python python-pip libxml2 zlib doxygen graphviz xdg-utils libdwarf"

            if ! $no_gui; then
                packages="$packages qt5-base qt5-svg qt5-wayland webkit2gtk"
            fi

            if ! $no_3d; then
                packages="$packages openscenegraph"
            fi

            echo_root_run "pacman -Sy --needed --noconfirm $packages ; pacman -Scc --noconfirm"

        else
            echo -e "${RED}Package manager (apt, dnf, zypper, pacman) not detected.\nSee 'doc/InstallGuide.pdf' and install the required packages manually.${RESET}"
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

        echo_run "brew install $packages"

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

Supported operating systems [and package managers]: 
- Linux: Ubuntu/Debian [apt], Fedora/AlmaLinux/RHEL9 [dnf], 
         OpenSuse-tumbleweed [zypper], Arch Linux [pacman]
- macOS 14 [homebrew]
- Windows 11/Msys2 [pacman]

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
install_deps

# activate the OMNeT++ environment (including the python virtualenv)
echo
echo_run source setenv

# configure the simulator
echo
echo -e "${GREEN}*** Configuring ***${RESET}"
echo
# disable some config options based on the provided command-line flags
if $no_gui; then CONFIGOPTS="$CONFIGOPTS WITH_QTENV=no"; fi
if $no_3d; then CONFIGOPTS="$CONFIGOPTS WITH_OSG=no"; fi
echo_run ./configure $CONFIGOPTS

# build
echo
echo -e "${GREEN}*** Compiling ***${RESET}"
echo
echo_run make -j16
echo
echo -e "${GREEN}*** Installation completed ***${RESET}"
echo
echo -e "Execute '${YELLOW}source setenv${RESET}' to activate the OMNeT++ environment."
echo
