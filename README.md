# csp-over-kiss-demo
This repository contains a demonstration of Cubesat Protocol over Serial with KISS.
It is implemented using [libcsp](https://github.com/NTNU-SmallSat-Lab/libcsp) and communicating over virtual serial-ports.

The demonstration contains two programs, a server and a client. The client sends messages over KISS to the server, and the server displays the received message.

### Dependencies:
To build this demonstration, you need:
- [libcsp](https://github.com/NTNU-SmallSat-Lab/libcsp) repository, which is included as a submodule in this repository. If you want to use libcsp from another location, edit the `LIBCSP` variables in the Makefile.
  - You need to build libcsp, and for this you need python (2.7).
- git
- gcc
- make
- socat (for creating virtual serial ports)

### Install libcsp
Before you try to build this example, make sure you install libcsp correctly.
The easiest and best way is to use the included submodule. 
To install libcsp, navigate to the root folder of the repository and run these commands:
```bash
git submodule update --init
cd libcsp
python waf configure --out=./build             \
                     --prefix=./build          \
                     --install-csp             \
                     --enable-crc32            \
                     --enable-init-shutdown    \
                     --enable-if-kiss          \
                     --enable-promisc          \
                     --with-os=posix           \
                     --with-rtable=cidr        \
                     --with-driver-usart=linux \
                     --with-driver-tcp=linux
python waf build install
cd ..
```

### Build:
To build this demonstration, navigate to the root directory of this repository and run
```bash
make
```
This should build both the server and the client. to build only the server or the client, run

```bash
make server
``` 

or 

```bash
make client
```

### Run demonstration:
To run the demonstration, make sure both the server and client is built correctly.
Running the example is done by these three steps:
- Open pseudo-serial port with socat
	- `socat -d -d pty,rawer pty,rawer`
- Start server, listening for messages
	- `./build/server <csp-id> <kiss-device>`
- Start client, sending messages to server
	- `./build/client <csp-id> <kiss-device> <server-csp-id>`

Here is an example where the opened ports are `/dev/pts/1` and `/dev/pts/2`:
```bash
socat -d -d pty,rawer pty,rawer &
sleep 1
./build/server 8 /dev/pts/1 &
./build/client 9 /dev/pts/2 8
```
