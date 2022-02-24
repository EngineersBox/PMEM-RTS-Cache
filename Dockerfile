FROM ubuntu:20.04

# Update apt repo entries
RUN apt-get -y update

# Install and configure debconf
RUN apt-get install -y debconf-utils

# Install dependencies
RUN \
    DEBIAN_FRONTEND=noninteractive apt-get install -y pkg-config && \
    apt-get install -y autoconf \
      libndctl-dev \
      ndctl \
      libdaxctl-dev \
      daxctl \
      cxl-cli \
      pandoc \
      git \
      make \
      gpg \
      wget \
      build-essential \
      software-properties-common \
      curl \
      htop \
      man \
      unzip \
      vim

# Get GPG signing key
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null

# Add kitware repo to sources and update
RUN \
    echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ focal main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null && \
    apt-get update

# Add the kitware-archive-keyring package for updates
RUN \
    rm /usr/share/keyrings/kitware-archive-keyring.gpg && \
    apt-get install -y kitware-archive-keyring

# Install latest cmake
RUN apt-get install -y cmake

# Install PMDK
RUN \
    git clone https://github.com/pmem/pmdk && \
    cd pmdk && \
    git checkout tags/1.11.1 && \
    make && \
    make install

CMD ["/bin/bash"]