#
# Copyright (C) OpenSim Kft.
#
# SPDX-License-Identifier: MIT
#

FROM quay.io/toolbx/arch-toolbox:latest
LABEL maintainer="Rudolf Hornig <rudi@omnetpp.org>"

CMD ["/bin/bash"]

RUN printf '\n[multilib]\nInclude = /etc/pacman.d/mirrorlist\n' >> /etc/pacman.conf && \
  pacman -Syu --noconfirm && \
  pacman -S --noconfirm --needed --assume-installed NTSYNC-MODULE \
    wine-staging winetricks \
    vulkan-icd-loader lib32-vulkan-icd-loader \
    mesa lib32-mesa vulkan-radeon lib32-vulkan-radeon vulkan-intel lib32-vulkan-intel vulkan-mesa-layers \
    pipewire lib32-pipewire lib32-libpulse lib32-gnutls ttf-liberation \
    wget p7zip sudo git bash-completion && \
  rm -rf /var/cache/pacman/pkg/*

COPY opp-win-shell /usr/local/bin/

# Unlike omnetpp-gui (PROMPT_COMMAND), the shell is entered from a login-only profile hook,
# so that 'distrobox enter windows-box -- bash' remains a plain Linux shell for debugging.
COPY profile-hook.sh /etc/profile.d/zz-opp-win-shell.sh
