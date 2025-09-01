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

Test connection without authentication.
```
./kdb_sandbox -m test -h 127.0.0.1 -p 1234 -u user
```

Test connection with a username.
```
./kdb_sandbox -m test -h 127.0.0.1 -p 1234 -u user
```

Test connection with a username and password.
```
./kdb_sandbox -m test -h 127.0.0.1 -p 1234 -u user -U pass
```

Send an arbitrary command and read the result.
```
./kdb_sandbox -m cmd -h 127.0.0.1 -p 1234 -c "3+4.5"
```

Publish a trade event information.
```
./kdb_sandbox -m trade -h 127.0.0.1 -p 1234 -c "3+4.5"
```
