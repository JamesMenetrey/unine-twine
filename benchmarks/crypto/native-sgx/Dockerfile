FROM alpine:3.12 AS builder
ARG BUILD_ROOT=/benchmark
ARG BENCHMARK_SRC_HOST=crypto
ARG BENCHMARK_SRC_IMAGE=${BUILD_ROOT}/crypto
ARG WOLFSSL_HOST=wolfssl/dist
ARG WOLFSSL_IMAGE=${BUILD_ROOT}/wolfssl/dist

RUN apk add --no-cache clang=10.0.0-r2 lld=10.0.0-r0 compiler-rt=10.0.0-r0 compiler-rt-static=10.0.0-r0 musl-dev make

RUN mkdir -p ${BENCHMARK_SRC_IMAGE}

ADD ${BENCHMARK_SRC_HOST} ${BENCHMARK_SRC_IMAGE}
ADD ${WOLFSSL_HOST} ${WOLFSSL_IMAGE}

WORKDIR ${BENCHMARK_IMAGE}/build

RUN cd ${BENCHMARK_SRC_IMAGE}/native && make COMPILE_FLAGS="-fuse-ld=lld --rtlib=compiler-rt"


FROM alpine:3.6
ARG BUILD_ROOT=/benchmark
ARG BENCHMARK_EXE_IMAGE=${BUILD_ROOT}/crypto/native/build

COPY --from=builder ${BENCHMARK_EXE_IMAGE} /