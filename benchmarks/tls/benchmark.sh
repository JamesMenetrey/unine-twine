#!/bin/bash
readonly SCRIPT_DIR=$(dirname $(realpath $0))
readonly NUMBER_OF_CONNECTIONS=512
readonly CLIENT_MACHINE_IP="172.28.254.53"
readonly CLIENT_MACHINE_SSH_KEY="/home/ubuntu/.ssh/id_ed25519_client"
readonly SERVER_MACHINE_IP="172.28.30.237"
readonly AESGCM="TLS13-AES128-GCM-SHA256"
readonly AESCCM="TLS13-AES128-CCM-SHA256"
readonly CHACHAPOLY="TLS13-CHACHA20-POLY1305-SHA256"

source $SCRIPT_DIR/../common.sh

mkdir -p ${SCRIPT_DIR}/results

launch_client () {
    SERVER_PORT=$1
    NUMBER_OF_CONCURRENT_CONNECTIONS=$2
    CIPHERS=$3

    ssh -i $CLIENT_MACHINE_SSH_KEY $CLIENT_MACHINE_IP "./client -v 4 -l $CIPHERS -i $SERVER_MACHINE_IP -p $SERVER_PORT -n $NUMBER_OF_CONNECTIONS -N $NUMBER_OF_CONCURRENT_CONNECTIONS 2>&1" | tee -a ${SCRIPT_DIR}/results/benchmark-client.csv
}

# Prototype: benchmark_native <profile_name>
function benchmark_native {
    PROFILE_NAME=$1

    cd ${SCRIPT_DIR}/native/build

    for ciphers in $AESGCM #$AESCCM $CHACHAPOLY
    do
        for concurrent_connections in {1..50..1}
        do
            for i in {1..10}
            do
                echo "./server-tls-poll-perf -v 4 -l $ciphers -p 11111 -n $NUMBER_OF_CONNECTIONS -N $concurrent_connections &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv.."
                ./server-tls-poll-perf -v 4 -l $ciphers -p 11111 -n $NUMBER_OF_CONNECTIONS -N $concurrent_connections &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv &
                sleep 2
                launch_client 11111 $concurrent_connections $ciphers
                sleep 1
            done
        done
    done
}

# Prototype: benchmark_native_sgx
function benchmark_native_sgx {
    cd ${SCRIPT_DIR}/native-sgx

    sudo ./sgx-lkl-setup.sh
    
    for ciphers in $AESGCM #$AESCCM $CHACHAPOLY
    do
        for concurrent_connections in {1..50..1}
        do
            for i in {1..10}
            do
                pkill sgx-lkl-run-oe
                echo "./run-benchmark.sh -v 4 -l $ciphers -p 11112 -n $NUMBER_OF_CONNECTIONS -N $concurrent_connections &>> ${SCRIPT_DIR}/results/benchmark-native-sgx.csv.."
                sudo ./run-benchmark.sh -v 4 -l $ciphers -p 11112 -n $NUMBER_OF_CONNECTIONS -N $concurrent_connections &>> ${SCRIPT_DIR}/results/benchmark-native-sgx.csv &
                sleep 5
                launch_client 11112 $concurrent_connections $ciphers
                sleep 1
            done
        done
    done
}

# Prototype: benchmark <profile_name> <iwasm_path>
function benchmark_wasm {
    PROFILE_NAME=$1
    IWASM_PATH=$2

    cd ${SCRIPT_DIR}/wasm/build
    
    for ciphers in $AESGCM #$AESCCM $CHACHAPOLY
    do
        for concurrent_connections in {1..50..1}
        do
            for i in {1..10}
            do
                echo "$IWASM_PATH --stack-size=$((3 * 1024 * 1024)) --heap-size=$((50 * 1024 * 1024)) --addr-pool=0.0.0.0/1 server-tls-poll-perf.aot -v 4 -l $ciphers -p 11111 -n $NUMBER_OF_CONNECTIONS -N $concurrent_connections &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv.."
                $IWASM_PATH --stack-size=$((3 * 1024 * 1024)) --heap-size=$((50 * 1024 * 1024)) --addr-pool=0.0.0.0/1 server-tls-poll-perf.aot -v 4 -l $ciphers -p 11111 -n $NUMBER_OF_CONNECTIONS -N $concurrent_connections &>> ${SCRIPT_DIR}/results/benchmark-$PROFILE_NAME.csv &
                sleep 5
                launch_client 11111 $concurrent_connections $ciphers
                sleep 1
            done 
        done
    done
}

echo "Script started at `date`."

disable_large_pages
benchmark_native native

enable_large_pages
benchmark_native native-large-pages
benchmark_wasm wasm $WAMR_PATH/product-mini/platforms/linux/build/iwasm
benchmark_native_sgx
benchmark_wasm wasm-sgx $WAMR_PATH/product-mini/platforms/linux-sgx/enclave-sample/iwasm

# Clean up
optin_large_pages

echo "Script ended at `date`."