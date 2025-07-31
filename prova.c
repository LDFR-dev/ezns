#include <arpa/inet.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <unistd.h>

#define DEST_IP "192.168.122.25"
#define FTP_PORT 21
#define SSH_PORT 22
#define SMTP_PORT 25
#define HTTP_PORT 80
#define POP3_PORT 110
#define IMAP_PORT 143
#define HTTPS_PORT 443

void check_port (int port_number)
{
	int sockfd;
	struct sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	inet_pton(AF_INET, DEST_IP, &addr.sin_addr);
	addr.sin_port = htons(port_number);

	if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		printf("connection refused\n");
	} else {
		printf("connection accepted\n");
	};
	close(sockfd);
}

int main (void)
{
	for(uint16_t p = 0; p < 65535; p++)
	{
		check_port(p);
	}
	return 0;
}
