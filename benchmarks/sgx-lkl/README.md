# Installing SGX-LKL

1. Install SGX-LKL using the `.deb` file, since the official Debian repository is no longer available: `sudo dpkg -i sgx-lkl-nonrelease_0.2.0-dev.20210412.1-b6e838e_amd64.deb`
2. Update any missing dependencies: `sudo apt-get install -f`
3. Reference the directory of the binaries in your path variable: `/opt/sgx-lkl-nonrelease/bin`
