#include "getinfo.h"

int get_ip(char *buffer, char *hostname) {

  struct addrinfo *result;

  if (getaddrinfo(hostname, NULL, NULL, &result) < 0) {
    perror("couldnt retrieve public IP");
    return -1;
  };
  struct sockaddr_in *ipv4 = (struct sockaddr_in *)result->ai_addr;

  if (inet_ntop(AF_INET, &(ipv4->sin_addr), buffer, ADDR_STR_LEN) != NULL) {
    freeaddrinfo(result);
    return 0;
  };

  return -1;
}
