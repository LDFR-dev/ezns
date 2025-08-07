#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "getinfo.h"

#define COLOR_GREEN "\x1b[32m"
#define COLOR_RED "\x1b[31m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_RESET "\x1b[0m"

void print_logo() {
    printf(COLOR_CYAN "\n"
           "                        \n"
           "      ___ _____ __  ___ \n"
           "     / _ \\_  / '_ \\/ __|\n"
           "    |  __// /| | | \\__ \\\n"
           "     \\___/___|_| |_|___/\n"
           "                        \n\n" COLOR_RESET);
}

void print_usage(const char *prog_name) {
  printf("Usage: %s <target_ip> -p <port_range>\n", prog_name);
  printf("  -p <port_range>: Specify port range (e.g., 1-1024, 80, all)\n");
}

void print_scan_header(const char *target_ip, int start_port, int end_port) {
  printf("\nStarting TCP Connect Scan on %s\n", target_ip);
  printf("For ports %d to %d\n\n", start_port, end_port);
  
  printf(COLOR_CYAN "┌────────────────────────┬──────────────────────────────────┐\n" COLOR_RESET);
  printf(COLOR_CYAN "│" COLOR_RESET "%-24s" COLOR_CYAN "│" COLOR_RESET "%-34s" COLOR_CYAN "│\n" COLOR_RESET, " Port", " Status");
  printf(COLOR_CYAN "├────────────────────────┼──────────────────────────────────┤\n" COLOR_RESET);
}

void print_scan_result(int port, const char *status, const char *color) {
  printf(COLOR_CYAN "│" COLOR_RESET " %-22d " COLOR_CYAN "│" COLOR_RESET " %s%s" COLOR_RESET,
         port, color, status);
  
  // Calcola gli spazi necessari per allineare il bordo destro (32 spazi totali per il contenuto)
  int status_len = strlen(status);
  int spaces_needed = 32 - status_len;
  
  for (int i = 0; i < spaces_needed; i++) {
    printf(" ");
  }
  
  printf(" " COLOR_CYAN "│\n" COLOR_RESET);
}

void print_scan_footer() {
  printf(COLOR_CYAN "└────────────────────────┴──────────────────────────────────┘\n\n" COLOR_RESET);
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
  print_scan_header(target_ip, start_port, end_port);

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
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
               sizeof(timeout));
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
               sizeof(timeout));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, target_ip, &addr.sin_addr) <= 0) {
      perror("inet_pton");
      close(sockfd);
      return;
    }

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      if (errno == ECONNREFUSED) {
        print_scan_result(port, "Closed (Connection Refused)", COLOR_RED);
      } else if (errno == ETIMEDOUT) {
        print_scan_result(port, "Filtered (Connection Timed Out)",
                        COLOR_YELLOW);
      }
    } else {
      print_scan_result(port, "Open", COLOR_GREEN);
    }
    close(sockfd);
  }
  print_scan_footer();
}

int is_host_reachable(const char *target_ip) {
    int sockfd;
    struct sockaddr_in addr;
    int reachable = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return 0;
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(80); // Check on port 80
    if (inet_pton(AF_INET, target_ip, &addr.sin_addr) <= 0) {
        close(sockfd);
        return 0;
    }

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) >= 0) {
        reachable = 1;
    }

    close(sockfd);
    return reachable;
}

int is_valid_ipv4(const char *ip) {
  struct sockaddr_in sa;
  if (strchr(ip, '-')) {
    char ip_copy[ADDR_STR_LEN];
    strncpy(ip_copy, ip, ADDR_STR_LEN);
    char *dash = strchr(ip_copy, '-');
    *dash = '\0';
    return inet_pton(AF_INET, ip_copy, &(sa.sin_addr)) == 1;
  }
  return inet_pton(AF_INET, ip, &(sa.sin_addr)) == 1;
}

void parse_ip_range(char *ip_range_str, char *start_ip, char *end_ip) {
  char *dash = strchr(ip_range_str, '-');
  if (dash) {
    // Copy the start IP part
    strncpy(start_ip, ip_range_str, dash - ip_range_str);
    start_ip[dash - ip_range_str] = '\0';

    // Construct the end IP
    char *last_dot = strrchr(start_ip, '.');
    if (last_dot) {
        strncpy(end_ip, start_ip, last_dot - start_ip + 1);
        end_ip[last_dot - start_ip + 1] = '\0';
        strcat(end_ip, dash + 1);
    } else {
        strcpy(end_ip, dash + 1);
    }
  } else {
    strcpy(start_ip, ip_range_str);
    strcpy(end_ip, ip_range_str);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    print_logo();
    print_usage(argv[0]);
    return 1;
  }

  char *target = NULL;
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
    target = argv[optind];
  } else {
    fprintf(stderr, "Target IP address or range is missing.\n");
    print_usage(argv[0]);
    return 1;
  }

  if (port_range_str) {
    parse_ports(port_range_str, &start_port, &end_port);
  }

  print_logo();

  char start_ip_str[ADDR_STR_LEN];
  char end_ip_str[ADDR_STR_LEN];
  int is_range = (strchr(target, '-') != NULL);

  if (is_valid_ipv4(target)) {
    parse_ip_range(target, start_ip_str, end_ip_str);

    struct in_addr start_ip_addr, end_ip_addr;
    inet_pton(AF_INET, start_ip_str, &start_ip_addr);
    inet_pton(AF_INET, end_ip_str, &end_ip_addr);

    uint32_t start_ip = ntohl(start_ip_addr.s_addr);
    uint32_t end_ip = ntohl(end_ip_addr.s_addr);

    for (uint32_t current_ip = start_ip; current_ip <= end_ip; current_ip++) {
      struct in_addr current_ip_addr;
      current_ip_addr.s_addr = htonl(current_ip);
      char current_ip_str[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &current_ip_addr, current_ip_str, INET_ADDRSTRLEN);

      if (!is_host_reachable(current_ip_str)) {
          fprintf(stderr, "Host %s is unreachable.\n", current_ip_str);
          if(is_range){
              continue;
          } else {
              return 1;
          }
      }
      tcp_connect_scan(current_ip_str, start_port, end_port);
    }
  } else {
    char target_ip[ADDR_STR_LEN];
    if (get_ip(target_ip, target) < 0) {
      perror("failed getting ipv4 address");
      return 1;
    }
    if (!is_host_reachable(target_ip)) {
        fprintf(stderr, "Host %s is unreachable.\n", target_ip);
        return 1;
    }
    tcp_connect_scan(target_ip, start_port, end_port);
  }

  return 0;
}

