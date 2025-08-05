# EZNS - Raw TCP Packet Sender TCP 🚀

Welcome to **EZNS**! This is a simple yet powerful C program designed to craft and send raw TCP packets using raw sockets. It's a great tool for anyone looking to dive deep into network protocols and understand the magic happening under the hood. 🧙‍♂️

## ✨ Features

-   **Custom IP Headers:** Forge your own IP headers from scratch.
-   **Custom TCP Headers:** Build TCP headers, specifically tailored for SYN packets.
-   **Checksum Calculation:** Automatically calculates the TCP checksum for you.
-   **Raw Socket Power:** Sends packets directly through raw sockets for maximum control.

## ⚙️ How it Works

The program is built around a few key components:

1.  **IP & TCP Structures:**
    We define `ip_packet_t` and `tcp_packet_t` structs to represent the IP and TCP headers. This allows us to manipulate each field of the packet headers with precision.

2.  **Header Construction:**
    -   `build_ip_header()`: This function assembles the IP header, setting fields like version, TTL, and source/destination addresses.
    -   `build_tcp_packet()`: This function constructs the TCP header, setting the source/destination ports and the SYN flag to initiate a connection.

3.  **TCP Checksum:**
    -   `compute_tcp_checksum()`: This is a crucial step! The function calculates the TCP checksum based on the TCP header, data, and a "pseudo-header" containing IP-level information.

4.  **Packet Transmission:**
    -   In `main()`, we create a raw socket (`AF_INET`, `SOCK_RAW`, `IPPROTO_RAW`).
    -   We set the `IP_HDRINCL` socket option, which tells the kernel that we're providing our own IP header.
    -   The IP and TCP headers are copied into a single buffer.
    -   `sendto()` is used to send the packet on its way!

## 🛠️ How to Compile and Run

1.  **Compile the program:**
    ```bash
    make
    ```

2.  **Run the executable:**
    Since we're using raw sockets, you'll need root privileges.
    ```bash
    sudo ./ezns
    ```


Made with ❤️ by LDFR