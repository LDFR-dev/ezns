#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <getopt.h>
#include <errno.h>
#include <sys/time.h>

#define COLOR_GREEN   "\x1b[32m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_RESET   "\x1b[0m"

void print_usage(const char *prog_name) {
    printf("Usage: %s <target_ip> -p <port_range>\n", prog_name);
    printf("  -p <port_range>: Specify port range (e.g., 1-1024, 80, all)\n");
}

void parse_ports(char *port_str, int *start_port, int *end_port) {
    if (strcmp(port_str, "all") == 0) {
        *start_port = 1;
        *end_port = 65535;
        return;
    }
    char *dash = strchr(port_str, '-');
    if (dash) {
        *dash = '\0';
        *start_port = atoi(port_str);
        *end_port = atoi(dash + 1);
    } else {
        *start_port = *end_port = atoi(port_str);
    }
}

void tcp_connect_scan(const char *target_ip, int start_port, int end_port) {
    printf("Starting TCP Connect Scan on %s for ports %d-%d\n", target_ip, start_port, end_port);

    for (int port = start_port; port <= end_port; port++) {
        int sockfd;
        struct sockaddr_in addr;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("socket");
            continue;
        }

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (inet_pton(AF_INET, target_ip, &addr.sin_addr) <= 0) {
            perror("inet_pton");
            close(sockfd);
            return;
        }

        if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            if (errno == ECONNREFUSED) {
                printf("Port %d: " COLOR_RED "Closed (Connection Refused)" COLOR_RESET "\n", port);
            } else if (errno == ETIMEDOUT) {
                printf("Port %d: " COLOR_YELLOW "Filtered (Connection Timed Out)" COLOR_RESET "\n", port);
            }
        } else {
            printf("Port %d: " COLOR_GREEN "Open" COLOR_RESET "\n", port);
        }
        close(sockfd);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }

    char *target_ip = NULL;
    char *port_range_str = NULL;
    int start_port = 1, end_port = 1024; // Default range
    int opt;

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                port_range_str = optarg;
                break;
            case '?':
                print_usage(argv[0]);
                return 1;
        }
    }

    if (optind < argc) {
        target_ip = argv[optind];
    } else {
        fprintf(stderr, "Target IP address is missing.\n");
        print_usage(argv[0]);
        return 1;
    }

    if (port_range_str) {
        parse_ports(port_range_str, &start_port, &end_port);
    }

    tcp_connect_scan(target_ip, start_port, end_port);

    return 0;
}