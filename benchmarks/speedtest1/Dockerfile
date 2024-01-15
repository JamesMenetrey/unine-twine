FROM ubuntu:18.04

# ==============================
# Configuration static variables
# ==============================
ARG WASI_SDK_VERSION=14
ARG SGX_SDK=2.11.100.2
ARG BENCHMARKS_ROOT_DIR=/speedtest1


# ===============
# Essential tools
# ===============

# Baseline components
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    ca-certificates \
    software-properties-common \
    clang-10 \
    gcc \
    make \
    wget \
    git \
    && rm -rf /var/lib/apt/lists/*

# Create symbolic links for clang
RUN ln -s /usr/bin/clang-10 /usr/bin/clang
RUN ln -s /usr/bin/clang++-10 /usr/bin/clang++

# Rely on `software-properties-common` to install the one of the latest version of cmake (at the time of writing at least)
RUN wget -qO - https://apt.kitware.com/keys/kitware-archive-latest.asc | apt-key add - && \
    apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main' && \
    apt update && \
    apt install -y cmake \
    && rm -rf /var/lib/apt/lists/*


# ======================
# WASM Compilation tools
# ======================

# WASI SDK
WORKDIR /opt
RUN wget --quiet https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-${WASI_SDK_VERSION}/wasi-sdk-${WASI_SDK_VERSION}.0-linux.tar.gz  && \
    tar xf wasi-sdk-${WASI_SDK_VERSION}.0-linux.tar.gz && \
    rm wasi-sdk-${WASI_SDK_VERSION}.0-linux.tar.gz && \
    mv wasi-sdk-${WASI_SDK_VERSION}.0 wasi-sdk

ENV PATH="$PATH:/opt/wasi-sdk"


# ======================
# SGX Compilation tools
# ======================

# SGX SDK
WORKDIR /opt/intel
RUN wget --quiet https://download.01.org/intel-sgx/sgx-linux/2.11/distro/ubuntu18.04-server/sgx_linux_x64_sdk_${SGX_SDK}.bin && \
    chmod u+x sgx_linux_x64_sdk_${SGX_SDK}.bin && \
    echo "yes" | ./sgx_linux_x64_sdk_${SGX_SDK}.bin \
    source /opt/intel/sgxsdk/environment


# =======================
# Compile the benchmark binaries
# =======================

WORKDIR ${BENCHMARKS_ROOT_DIR}
CMD ./build.sh