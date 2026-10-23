FROM ghcr.io/omnetpp/distrobuild-base:ubuntu22.04-macos13.3-261023
LABEL maintainer="Rudolf Hornig <rudi@omnetpp.org>"

# during docker image builds, the 'tools' directory in the context dir should contain the Windows and macOS toolchains (i.e. ./tools/170717/...)
ENV TOOLS_DIR /root/tools
ENV TOOLS_VERSION 250612
ENV OMNETPP_DIR /root/omnetpp

# clone the directory
WORKDIR /root

# clone the repository either remotely or from a local repo in the context directory
#RUN git clone -n https://github.com/omnetpp/omnetpp.git $OMNETPP_DIR
# optionally you can use local bare repo, called 'omnetpp-repo' from the context dir
# use 'git clone -n ../../../.git omnetpp-repo' to create it
COPY omnetpp-repo/ $OMNETPP_DIR

# copy the windows/macOS tools into the build image
COPY tools/$TOOLS_VERSION $TOOLS_DIR/$TOOLS_VERSION

# update the eclipse build's MAVEN cache for faster builds
WORKDIR $OMNETPP_DIR

# allow using the repo even if its owner is the root
RUN git config --global --add safe.directory /root/omnetpp-repo/.git

# The dist build runs with --network none, so all Python packages are installed here from the build
# branch. configure requires the runtime modules (python/requirements.txt) because the repo has an 'ide'
# dir; they go into a venv on the system Python, the same way install.sh sets up a user installation.
RUN uv venv --python /usr/bin/python3 --no-python-downloads --seed /opt/pyvenv && \
    git show origin/topic/buildimage:python/requirements.txt > /tmp/requirements.txt && \
    uv pip install --python /opt/pyvenv --no-cache -r /tmp/requirements.txt && \
    rm -f /tmp/requirements.txt
ENV PATH="/opt/pyvenv/bin:$PATH" VIRTUAL_ENV=/opt/pyvenv

# The Sphinx toolchain comes from the locked doc/src project. It needs Python >=3.12 (the system Python
# is 3.10), so uv provides its own CPython. The venv is kept off PATH so that configure never builds
# against it; makedist uses it for the doc build only, and refuses to do so if the checked out uv.lock
# differs from the copy stored next to the venv.
RUN mkdir -p /tmp/docproj && \
    git show origin/topic/buildimage:doc/src/pyproject.toml > /tmp/docproj/pyproject.toml && \
    git show origin/topic/buildimage:doc/src/uv.lock > /tmp/docproj/uv.lock && \
    (cd /tmp/docproj && UV_PROJECT_ENVIRONMENT=/opt/docvenv UV_PYTHON=3.12 UV_PYTHON_INSTALL_DIR=/opt/uvpython \
        uv sync --frozen --no-dev --no-cache) && \
    cp /tmp/docproj/uv.lock /opt/docvenv/uv.lock && \
    find /opt/docvenv -type d -name '__pycache__' -prune -exec rm -rf {} + && \
    rm -rf /tmp/docproj

# getting only the dependnecies does not work correctly (not everything is cached unfortunately, so a full build is required)
#RUN git checkout master && (cd $OMNETPP_DIR/ui && (mvn dependency:go-offline -Dwhat=omnetpp -fn || true)) ; rm -rf $OMNETPP_DIR

# we dummy build the IDE and then delete the whole OMNeT++ source, so as a side effect the maven cache will be updated
RUN /bin/bash -c 'git checkout topic/buildimage && \
    cp configure.user.dist configure.user && \
    source ./setenv -f && \
    ./configure WITH_QTENV=no WITH_OSG=no WITH_OSGEARTH=no && \
    make MODE=release ui -j$(nproc) && \
    OFFLINE=no releng/ide/java/build-ide-java && \
    rm -rf $OMNETPP_DIR'

# sphinx_immaterial downloads the Google fonts of the theme when the build starts, which fails without
# network. A minimal build with the default theme fonts (conf.py does not override them) fills its cache.
ENV SPHINX_IMMATERIAL_EXTERNAL_RESOURCE_CACHE_DIR=/opt/sphinx-immaterial-cache
RUN mkdir -p /tmp/fontcache && cd /tmp/fontcache && \
    printf 'extensions = ["sphinx_immaterial"]\nhtml_theme = "sphinx_immaterial"\n' > conf.py && \
    printf 'Font cache\n==========\n' > index.rst && \
    /opt/docvenv/bin/sphinx-build -q -b html . out && \
    cd / && rm -rf /tmp/fontcache && \
    ls /opt/sphinx-immaterial-cache/google_fonts | grep -q response

# while running this image you should either map a local OMNeT++ repo from the host to the /root/omnetpp-repo dir
# or specify a remote OMNeT++ repo URL in the OMNETPP_REPO environment variable.
# default variables controlling the OMNeT++ build

ENV OMNETPP_REPO /root/omnetpp-repo
ENV GIT_VERSION origin/master
ENV WHAT omnetpp
ENV BUILD_CORE_DISTRO true
ENV BUILD_LINUX_X86_64_DISTRO true
ENV BUILD_LINUX_AARCH64_DISTRO true
ENV BUILD_WIN32_X86_64_DISTRO true
ENV BUILD_MACOS_X86_64_DISTRO true
ENV BUILD_MACOS_AARCH64_DISTRO true
CMD git clone -n $OMNETPP_REPO $OMNETPP_DIR && cd $OMNETPP_DIR && git checkout -f $GIT_VERSION && releng/makedist
