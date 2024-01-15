FROM ubuntu:18.04

# ==============================
# Configuration static variables
# ==============================
ARG SGX_SDK=2.11.100.2
ARG RUNTIME_ROOT_DIR=/runtime


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
    ccache \
    make \
    wget \
    git \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Create symbolic links for clang and Python
RUN ln -s /usr/bin/clang-10 /usr/bin/clang
RUN ln -s /usr/bin/clang++-10 /usr/bin/clang++
RUN ln -s /usr/bin/pip3 /usr/bin/pip

# Install the latest version of Cmake and Ninja using pip3 (Kitware no longer provide Cmake for 18.04)
RUN pip3 install --upgrade pip
RUN pip3 install scikit-build
RUN pip3 install cmake ninja

# Set up local directories for pip and ccache
RUN mkdir /.cache && chmod 777 /.cache
RUN mkdir /.local && chmod 777 /.local
RUN mkdir /.ccache && chmod 777 /.ccache


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
# Compile the runtime
# =======================

WORKDIR ${RUNTIME_ROOT_DIR}
CMD ./build.sh