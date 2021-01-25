FROM alpine:3.12 AS builder
ARG BUILD_ROOT=/benchmark
ARG SQLITE_ROOT_HOST=sqlite/dist/sqlite
ARG SQLITE_ROOT_IMAGE=${BUILD_ROOT}/sqlite/dist/sqlite
ARG SPEEDTEST_LIB_SRC_HOST=speedtest1/benchmark
ARG SPEEDTEST_LIB_SRC_IMAGE=${BUILD_ROOT}/speedtest1/benchmark
ARG SPEEDTEST_EXE_SRC_HOST=speedtest1/benchmark-sgx
ARG SPEEDTEST_EXE_SRC_IMAGE=${BUILD_ROOT}/speedtest1/benchmark-sgx

RUN apk add --no-cache gcc g++ musl-dev cmake make

RUN mkdir -p ${SQLITE_ROOT_IMAGE}
RUN mkdir -p ${SPEEDTEST_LIB_SRC_IMAGE}
RUN mkdir -p ${SPEEDTEST_EXE_SRC_IMAGE}/build

ADD ${SQLITE_ROOT_HOST} ${SQLITE_ROOT_IMAGE}
ADD ${SPEEDTEST_LIB_SRC_HOST}/include ${SPEEDTEST_LIB_SRC_IMAGE}/include
ADD ${SPEEDTEST_LIB_SRC_HOST}/src ${SPEEDTEST_LIB_SRC_IMAGE}/src
ADD ${SPEEDTEST_LIB_SRC_HOST}/CMakeLists.txt ${SPEEDTEST_LIB_SRC_IMAGE}/CMakeLists.txt
ADD ${SPEEDTEST_EXE_SRC_HOST}/src ${SPEEDTEST_EXE_SRC_IMAGE}/src
ADD ${SPEEDTEST_EXE_SRC_HOST}/CMakeLists.txt ${SPEEDTEST_EXE_SRC_IMAGE}/CMakeLists.txt

WORKDIR ${SPEEDTEST_EXE_SRC_IMAGE}/build

RUN cmake .. && make


FROM alpine:3.6
ARG BUILD_ROOT=/benchmark
ARG SPEEDTEST_EXE_SRC_IMAGE=${BUILD_ROOT}/speedtest1/benchmark-sgx

COPY --from=builder ${SPEEDTEST_EXE_SRC_IMAGE}/build/benchmark-sgx /