# ezns - TCP Port Scanner 🚀

Welcome to **EZNS**! This is a simple and efficient TCP port scanner written in C. It's a great tool for network exploration and security auditing. 🧙‍♂️

<img src="https://external-content.duckduckgo.com/iu/?u=https%3A%2F%2Fe7.pngegg.com%2Fpngimages%2F724%2F306%2Fpng-clipart-c-logo-c-programming-language-icon-letter-c-blue-logo.png&f=1&nofb=1&ipt=3c24c0dcbad8975461142a1d0ee9dd65d3615363a66fd14b112cab38021987c7" height="35" alt="c logo"  />

## ✨ Features

-   **TCP Connect Scan:** Performs a standard TCP connect scan to identify open ports.
-   **Custom Port Ranges:** Specify a single port, a range of ports (e.g., 1-1024), or scan all ports (1-65535).
-   **Host Reachability Check:** Before scanning, the tool checks if the target host is reachable. If a single host is specified and it's unreachable, the program will notify the user and exit. If a range of IPs is provided, it will notify the user about the unreachable host and continue to the next one.
-   **Status Identification:**  Clearly identifies ports as "Open", "Closed", or "Filtered".
-   **User-Friendly Output:** Color-coded output for easy identification of port statuses.

## ⚙️ How it Works

The program operates by attempting to establish a full TCP connection with each port in the specified range on the target host.

1.  **Socket Creation:** For each port, a new TCP socket is created.
2.  **Connection Attempt:** The `connect()` system call is used to try to establish a connection to the target IP and port.
3.  **Status Analysis:**
    -   If `connect()` succeeds, the port is marked as **Open**.
    -   If `connect()` fails with `ECONNREFUSED`, the port is marked as **Closed**.
    -   If `connect()` fails with `ETIMEDOUT`, the port is marked as **Filtered** (meaning a firewall or other network device is likely blocking the connection).
4.  **Resource Management:** The socket is closed after each connection attempt to free up resources.

## 📦 Requirements

To compile and run EZNS, you'll need the following dependencies:

-   **GCC (GNU Compiler Collection):** The standard C compiler.
-   **Make:** The build automation tool.

### Installation Instructions

Here's how to install these dependencies on various Linux distributions:

#### Debian/Ubuntu
```bash
sudo apt update
sudo apt install build-essential
```

#### Fedora/CentOS/RHEL
```bash
sudo dnf groupinstall "Development Tools"
```
or
```bash
sudo yum groupinstall "Development Tools"
```

#### Arch Linux
```bash
sudo pacman -S base-devel
```

## 🛠️ How to Compile and Run

1.  **Compile the program:**
    ```bash
    make
    ```

2.  **Run the executable:**
    You don't necessarily need root privileges, but it's good practice for network scanning tools.
    ```bash
    ./bin/ezns <target_ipv4 / hostname> -p <port_range>
    ```

### Usage Examples

-   Scan a specific port on a specific IP:
    ```bash
    ./bin/ezns <your_ip / your_hostname> -p 80
    ```

-   Scan a range of port on localhost or on a specific IP:
    ```bash
    ./bin/ezns <your_ip / your_hostname> -p 1-1024
    ./bin/ezns localhost -p 1-1024
    ```

-   Scan all ports on a specific IP:
    ```bash
    ./bin/ezns <your_ip / your_hostname> -p all
    ```

-   Scan a range of IP addresses:
    ```bash
    ./bin/ezns 192.168.1.1-10 -p 80
    ```
    

Made with ❤️ by LDFR
