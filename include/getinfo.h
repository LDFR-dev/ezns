#ifndef _GETINFO_H
#define _GETINFO_H

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define ADDR_STR_LEN 46

int get_ip(char *buffer, char *hostname);

#endif
