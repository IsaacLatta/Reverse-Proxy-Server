# Reverse-Proxy

## Description

- This is a Reverse Proxy Server written with standalone ASIO.
- The Proxy will accept TCP connections a forward incoming connections to it's specified backend server.
- This Proxy Server was written with the intention of gaining both knowledge and experience in lower level I/O programming, the ASIO library, and the TCP/IP protocol.
- This project is still in development, I will likely be making modifications to it in the future, as I intend on using it in future projects.

## Prerequisites

- To compile the proxy, ensure you have a C++ compiler installed, and standalone ASIO.
- ASIO can be downloaded [here](https://think-async.com/Asio/Download.html).
- g++ can be found [here](https://gcc.gnu.org/).
- Or to install via your package manager of choice, for example:
```bash
sudo apt-get install g++ libasio-dev
```
- Or ...

```bash
sudo dnf install g++ libasio-dev
```

## Installation

1. Clone the repository:
```bash
git clone https://IsaacLatta/Reverse-Proxy
```

2. Navigate to the directory:
```bash
cd /path/to/Reverse-Proxy
```

3. Compile the project
```bash
g++ -I/path/to/asio -o proxy main.cpp Session.cpp RevProxy.cpp logger.cpp
```

## Running the program

- The proxy takes 3 command line arguments to run, the port to host the proxy on, the backend server's IP address, and the backend server's port.

- For example to run the proxy on port 1234 and forward to a server on port 12345 on localhost run:
```bash
./proxy 1234 127.0.0.1 12345
```

## Usage

- The proxy accepts incoming network request up to a size of 64KB. To change, modify the BUFFER_SIZE macro in the Session.h file.
- The proxy will read outgoing data from its backend server until the connection is closed, or it receives the eof error on the socket fd.
- A new conncetion is created for each network request, and closed on any errors or when the response forwarding is complete.
- To see a basic usage message, run the proxy with no arguments:
```bash
./proxy
```

## Troubleshooting
- If testing over a network, your firewall will likely need to be configured to allow connections on the host port, this is especially likely if the supplied port is below 1000.
- Errors will logged in the proxy-date.log file.

## Contributions
- If you have any suggestions or improvements, I am absolutley open to them! Feel free to open an issue or submit a pull request.
