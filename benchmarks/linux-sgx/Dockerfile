FROM ubuntu:18.04


# ==================================
# Configuration environment variable
# ==================================
ENV BUILDING_SDK_FLAG= 


# ===============
# Essential tools
# ===============

# Baseline components
RUN apt-get update && apt-get install  -y \
    autoconf \
    automake \
    build-essential \
    cmake \ 
    curl \
    debhelper \
    git \
    libcurl4-openssl-dev \
    libprotobuf-dev \
    libssl-dev \
    libtool \
    lsb-release \
    ocaml \
    ocamlbuild \
    protobuf-compiler \
    python \
    wget \
    && rm -rf /var/lib/apt/lists/*


# =======================
# Compile the protected file system
# =======================

# Assume the linux-sgx source files are located in the path /linux-sgx
WORKDIR /linux-sgx

CMD ./build.sh ${BUILDING_SDK_FLAG}