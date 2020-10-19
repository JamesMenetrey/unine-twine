# Benchmarking tools for TWINE
This folder contains the experiments illustrated in the paper **TWINE: An Embedded Trusted Runtime for WebAssembly**.
This README helps to reproduce them, by providing the set up required to execute them.

## Prerequisites
### Operating system and software
- Ubuntu 18.04 (tested on 18.04.5)
- Intel SGX SDK 2.11+ and SGX driver 2.6+ (the SDK is expected to be installed in `/opt/intel/sgxsdk`) [[instructions](https://download.01.org/intel-sgx/latest/linux-latest/docs/)]
- Docker (tested on 19.03) [[instructions](https://docs.docker.com/engine/install/ubuntu/)]
- git
- Jupyter Notebook (to visualize the results; can be installed on a separate machine)

### Dependencies
#### TWINE
The source code of TWINE must be located in `/opt/wamr-sdk`.
The following commands clone its repository and symlink TWINE.

```
sudo mkdir /opt/twine
sudo sudo chown $(id -u ${USER}):$(id -g ${USER}) /opt/twine
git clone --depth 1 --recurse-submodules https://github.com/JamesMenetrey/unine-twine.git /opt/twine
sudo ln -s /opt/twine/runtime /opt/wamr-sdk
```

#### The official WAMR runtime
The source code of official WAMR must be located in `/opt/wamr-sdk-official`.
The experiments use a fork of it.

```
sudo mkdir /opt/wamr-sdk-official
sudo sudo chown $(id -u ${USER}):$(id -g ${USER}) /opt/wamr-sdk-official
git clone --depth 1 https://github.com/JamesMenetrey/wasm-micro-runtime.git /opt/wamr-sdk-official
```

## Running the experiments
### PolyBench/C (Figure 3)
```
cd /opt/twine/benchmarks
git checkout master
./polybench_build_docker.sh
./polybench_run_baremetal.sh
```

The output of the experiments are stored in `/opt/twine/benchmarks/polybench/results`.
The Jupyter notebook `polybench_analysis.ipynb` in the same folder can be used to analyse the raw data.
An instance of Jupyter notebook can be launched using the scipt: `/opt/twine/benchmarks/polybench/results/launch.sh`.

### Speedtest1 (Figure 4)
```
cd /opt/twine/benchmarks
git checkout master
./speedtest1_build_docker.sh
./speedtest1_run_baremetal.sh
```

The output of the experiments are stored in `/opt/twine/benchmarks/speedtest1/results`.
The Jupyter notebook `speedtest1_analysis.ipynb` in the same folder can be used to analyse the raw data.
An instance of Jupyter notebook can be launched using the scipt: `/opt/twine/benchmarks/speedtest1/results/launch.sh`.

### Breakdown of SQLite macro-benchmark (Figure 5)
```
cd /opt/twine/benchmarks
git checkout master
./benchmark_build_docker.sh
./benchmark_run_baremetal.sh
```

The output of the experiments are stored in `/opt/twine/benchmarks/sqlite/results`.
The Jupyter notebook `benchmark_analysis.ipynb` in the same folder can be used to analyse the raw data.
An instance of Jupyter notebook can be launched using the scipt: `/opt/twine/benchmarks/sqlite/results/launch.sh`.

### Breakdown of SQLite macro-benchmark optimised (Figure 6, linear charts)
To reproduce the two linear charts of Figure 6, the branch `optimized-pfs` must be used.

```
cd /opt/twine/benchmarks
git fetch --depth 1 origin optimized-pfs:optimized-pfs
git checkout optimized-pfs
./benchmark_build_docker.sh
./benchmark_run_baremetal.sh
```

The data is analysed in the same fashion as Figure 5.

### Breakdown of SQLite macro-benchmark optimised (Figure 6, bar chart)
The data in the bar chart results from the profiling of the TWINE, using the official Intel SGX SDK and the optimised one.

#### Execution time of official Intel SGX SDK
The data of the left bar can be obtained by running the following commands in `master` branch.

```
cd /opt/twine/benchmarks
git checkout master
./profiling_build_docker.sh
./profiling_run_baremetal.sh
```
The output of the experiments are stored in `/opt/twine/benchmarks/sqlite/results`.
The Jupyter notebook `profiling_analysis.ipynb` in the same folder can be used to analyse the raw data.
An instance of Jupyter notebook can be launched using the scipt: `/opt/twine/benchmarks/sqlite/results/launch.sh`.

#### Execution time of optimised Intel SGX SDK
The data of the right bar can be obtained by running the following commands in `optimized-pfs` branch.

```
cd /opt/twine/benchmarks
git fetch --depth 1 origin optimized-pfs:optimized-pfs
git checkout optimized-pfs
./profiling_build_docker.sh
./profiling_run_baremetal.sh
```
The output of the experiments are stored in `/opt/twine/benchmarks/sqlite/results`.
The Jupyter notebook `profiling_analysis.ipynb` in the same folder can be used to analyse the raw data.
An instance of Jupyter notebook can be launched using the scipt: `/opt/twine/benchmarks/sqlite/results/launch.sh`.

#### Aggregate the values as Figure 6
The execution time in Figure 6 are calculated as follows:

- Read (OCALLs): `trusted_fread_node_read_mht_node` + `trusted_fread_node_read_data_node`
- Read (memset): `trusted_memset_read_mht_node` + `trusted_memset_append_mht_node` + `trusted_memset_read_data_node` + `trusted_memset_append_data_node` + `trusted_memset_get_data_node`
- Read (other ops.): `wasi_fd_read` - (*Read (OCALLs):* + *Read (memset)*)
- SQLite inner work: *execution time* - *all wasi colummns*

By executing the Juypter notebook, these data are calculated in the files `profiling_regular-formatted.csv` (for the official Intel SGX SDK) and `profiling_ncopy-formatted.csv` (for the optimised Intel SGX SDK).

## License

The source code of TWINE and the source code of the benchmarks are released under the Apache license 2.0.
Check the file `LICENSE` for more information.

## Author

Jämes Ménétrey @ University of Neuchâtel, Switzerland