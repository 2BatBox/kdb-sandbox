# KDB experimental sandbox application.

#What are the dependencies and how to satisfy them?
```
cmake >= 3.10
make >= 4.0
gcc >= 11.0.0 (Older versions are not tested.)
libssl-dev >= 1.1.1 (both 1.1.0 and 3.0.0 OpenSSL library versions are supported)
libwebsockets-dev == 4.2.2
```

Ubuntu
```
sudo apt-get install git cmake make gcc g++
```

Amazon Linux
```
sudo yum install git cmake make gcc g++
```

Installing KDB static library.
```
cd ~
git clone https://github.com/KxSystems/kdb.git

// The following lines must be appended to "~/.bashrc".
export QHOME=~/kdb
export PATH=$PATH:$QHOME/l64
export QLIC=$QHOME
```

#How to build the sandbox?
```
git clone https://github.com/2BatBox/kdb-sandbox
cd kdb-sandbox
mkdir build && cd build
cmake ../
make
./kdb_sandbox
```

#How to use the sandbox?
```
```

#How to update the raw history data?
```
cd scripts
./binance_spot_update_raw.sh
./binance_spot_update_replay.sh
```
