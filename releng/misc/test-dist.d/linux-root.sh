#!/usr/bin/env bash
#
# Runs as root in a fresh distro container: creates the non-root sudo user "opp" that
# install.sh expects, then installs the archive as that user.
# Mounts: /dist (archives, ro), /harness (this directory, ro), /out (results, rw)
#
# Usage: linux-root.sh <archive file name> [install.sh args...]
#
ARCHIVE=$1; shift

. /etc/os-release
case "$ID" in
    ubuntu|debian) apt-get update -qq && DEBIAN_FRONTEND=noninteractive apt-get install -y -qq sudo tar gzip >/dev/null ;;
    fedora|almalinux|rhel|rocky|centos) dnf install -y -q sudo tar gzip findutils which procps-ng >/dev/null ;;
    opensuse*) zypper -n -q install sudo tar gzip findutils which >/dev/null ;;
    arch) pacman -Syu --noconfirm --needed sudo tar gzip which >/dev/null ;;
esac
useradd -m -s /bin/bash opp
echo 'opp ALL=(ALL) NOPASSWD: ALL' > /etc/sudoers.d/opp
chmod 440 /etc/sudoers.d/opp
chmod 777 /out

# su is missing from some base images (fedora:46); sudo -i also gives a clean login environment
exec sudo -iu opp env STEPS=install bash /harness/dist-test.sh "/dist/$ARCHIVE" /home/opp /out "$@"
