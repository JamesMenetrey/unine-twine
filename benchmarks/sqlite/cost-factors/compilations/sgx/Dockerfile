FROM ubuntu:18.04

# Install the prerequisites (from https://github.com/lsds/sgx-lkl/blob/oe_port/.azure-pipelines/scripts/install_prerequisites.sh)
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get install -y \
    build-essential \
    curl wget rsync pv \
    make gcc g++ bc python xutils-dev flex bison autogen libgcrypt20-dev libjson-c-dev \
    autopoint pkgconf autoconf libtool libcurl4-openssl-dev libprotobuf-dev libprotobuf-c-dev protobuf-compiler protobuf-c-compiler libssl-dev \
    ninja-build ansible "linux-headers-$(uname -r)" \
    python3 python3-setuptools python3-pip unzip dkms debhelper apt-utils pax-utils openjdk-8-jdk-headless \
    redis-tools \
    expect \
    gdb \
    shellcheck clang-format \
    git

RUN python3 -m pip install "jsonschema>=3" "black==19.10b0"

# Retrieve the sources
RUN git clone --branch oe_port --recursive https://github.com/lsds/sgx-lkl.git /sgx-lkl

WORKDIR /sgx-lkl
RUN git checkout 0c95637

# Set up Open Enclave
WORKDIR /sgx-lkl/openenclave

RUN scripts/ansible/install-ansible.sh
RUN ansible-playbook scripts/ansible/oe-contributors-acc-setup-no-driver.yml

# Prepare for building SGX-LKL
WORKDIR /sgx-lkl/
CMD make