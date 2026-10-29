#
# Copyright (C) OpenSim Kft.
#
# SPDX-License-Identifier: MIT
#

FROM quay.io/toolbx/ubuntu-toolbox:26.04
LABEL maintainer="Rudolf Hornig <rudi@omnetpp.org>"

SHELL ["/bin/bash", "-c"]
CMD ["/bin/bash"]
ARG VERSION

# setenv must be sourced after the system and user .bashrc is executed
# so we can initialize the OMNETPP environment variables properly.
# TRICK: we use the PROMPT_COMMAND environment variable to source entrypoint.sh
# right before the user prompt is shown and then unset it to prevent it from 
# being called again when the next prompt is shown.
ENV OPP_VER=$VERSION
ENV PROMPT_COMMAND="[[ '$__omnetpp_root_dir' != '/opt/omnetpp-$OPP_VER' ]] || source /opt/omnetpp-$OPP_VER/setenv; unset PROMPT_COMMAND"

ADD omnetpp-$VERSION-linux-x86_64.tgz /opt/

RUN export DEBIAN_FRONTEND=noninteractive && \
  apt-get update -y && \
  apt-get upgrade -y && \
  apt-get install -y --no-install-recommends git wget curl ca-certificates \
  bash apt-utils bash-completion bc bzip2 \
  curl dialog diffutils findutils gnupg gnupg2 gpgsm hostname iproute2 iputils-ping keyutils language-pack-en \
  less libcap2-bin libkrb5-3 libnss-mdns libnss-myhostname libvte-2.9*-common locales lsof \
  man-db manpages mtr ncurses-base openssh-client passwd pigz pinentry-curses procps \
  rsync sudo tcpdump time traceroute tree tzdata unzip util-linux wget xauth xz-utils zip && \
  apt-get clean && \
  cd /opt/omnetpp-$VERSION && \
  ./install.sh -y && \
  chmod -R og+w /opt/omnetpp-$VERSION && \
  rm -r out samples/*/out config.log config.status
