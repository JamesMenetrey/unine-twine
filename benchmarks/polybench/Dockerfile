FROM ubuntu:18.04

# ==============================
# Configuration static variables
# ==============================
ARG SGX_SDK=2.11.100.2
ARG BENCHMARKS_ROOT_DIR=/polybenchc


# ===============
# Essential tools
# ===============

# Baseline components
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    ca-certificates \
    software-properties-common \
    libclang-common-9-dev \
    make \
    wget \
    git \
    && rm -rf /var/lib/apt/lists/*

# Rely on `software-properties-common` to install the one of the latest version of cmake (at the time of writing at least)
RUN wget -qO - https://apt.kitware.com/keys/kitware-archive-latest.asc | apt-key add - && \
    apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main' && \
    apt update && \
    apt install -y cmake \
    && rm -rf /var/lib/apt/lists/*


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