#include <netinet/in.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

#define SOURCE_ADDRESS "192.168.1.16"
#define SOURCE_PORT 2006

#define DESTINATION_ADDRESS "192.168.122.25"
#define DESTINATION_PORT 2007

#define WINDOW_SIZE 65535

typedef struct {
	
	uint8_t version_and_IHL;
	uint8_t type_of_service;
	uint16_t total_length;
	uint16_t identification;
	uint16_t flags_and_fragmentation_offset;
	uint8_t time_to_live;
	uint8_t protocol;
	uint16_t header_checksum;
	uint32_t source_address;
	uint32_t destination_address;
	
	// not using options
} ip_packet_t;

void build_ip_header(ip_packet_t* packet, uint32_t source_address, uint32_t destination_address)
{
	packet->version_and_IHL = (4 << 4) | 5;	// IPv4 (4) and IHL = 5 (20 bytes = no options)
	packet->type_of_service = 0;
	packet->total_length = htons(20 + 20); 	// ip head + tcp head
	packet->identification = htons(0x1234); 	// random
	packet->flags_and_fragmentation_offset = htons(0x4000);
	packet->time_to_live = 64;
	packet->protocol = 6;	// TCP
	packet->header_checksum = 0;
	packet->source_address = source_address;
	packet->destination_address = destination_address;
}


typedef struct {

	uint16_t source_port;
	uint16_t destination_port;
	uint32_t sequence_number;
	uint32_t acknowledgement_number;	// 0 for SYN
	uint16_t offset_and_flags;
	uint16_t window_size;
	uint16_t checksum;
	uint16_t urgent_pointer;

} tcp_packet_t;


uint16_t compute_tcp_checksum(
    tcp_packet_t* tcp_header,
    int tcp_length,
    uint32_t source_ip,
    uint32_t dest_ip
) {
    struct pseudo_header {
        uint32_t source_address;
        uint32_t destination_address;
        uint8_t placeholder;
        uint8_t protocol;
        uint16_t tcp_length;
    };

    int total_length = sizeof(struct pseudo_header) + tcp_length;
    uint8_t buffer[4096];  // big enough for headers + pseudo header
    memset(buffer, 0, sizeof(buffer));

    // Fill pseudo header
    struct pseudo_header psh;
    psh.source_address = source_ip;
    psh.destination_address = dest_ip;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(tcp_length);

    // Copy pseudo header
    memcpy(buffer, &psh, sizeof(psh));

    // Copy TCP header (and data if you have any)
    memcpy(buffer + sizeof(psh), tcp_header, tcp_length);

    // Compute checksum
    uint32_t sum = 0;
    for (int i = 0; i < total_length; i += 2) {
        uint16_t word = (buffer[i] << 8) + (i + 1 < total_length ? buffer[i + 1] : 0);
        sum += word;
    }

    // Fold 32-bit sum to 16 bits
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return (uint16_t)(~sum);
}

void build_tcp_packet(tcp_packet_t* packet, uint16_t source_port, uint16_t destination_port)
{
	packet->source_port = htons(source_port);
	packet->destination_port = htons(destination_port);
	packet->sequence_number = 0;
	packet->acknowledgement_number = 0;

	uint16_t data_offset = 5; 	// 5 * 4 = 20 (min tcp head)
	uint16_t flags = 0x02;
	packet->offset_and_flags = htons((data_offset << 12) | flags);

	packet->window_size = htons(WINDOW_SIZE);
	packet->checksum = 0;
	packet->urgent_pointer = 0;

}

int main (void)
{
	tcp_packet_t tcp_p;
	build_tcp_packet(&tcp_p, SOURCE_PORT, DESTINATION_PORT);


	ip_packet_t ip_p;
	uint32_t source_address = inet_addr(SOURCE_ADDRESS);
	uint32_t destination_address = inet_addr(DESTINATION_ADDRESS);
	build_ip_header(&ip_p, source_address, destination_address);

	tcp_p.checksum = compute_tcp_checksum(&tcp_p, sizeof(tcp_packet_t), ip_p.source_address, ip_p.destination_address);

	uint8_t packet[40]; // 20 bytes IP + 20 bytes TCP
	memset(packet, 0, sizeof(packet));

	memcpy(packet, &ip_p, sizeof(ip_packet_t));
	memcpy(packet + sizeof(ip_packet_t), &tcp_p, sizeof(tcp_packet_t));

	int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (sock < 0) return 1;

	int one = 1;
	setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one));
	
	struct sockaddr_in dest;
	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(DESTINATION_PORT);  // optional; not used by IP
	dest.sin_addr.s_addr = inet_addr(DESTINATION_ADDRESS);

	ssize_t sent = sendto(sock, packet, sizeof(packet), 0,
                      (struct sockaddr*)&dest, sizeof(dest));

	if (sent < 0) {
    		perror("sendto");
	} else {
    		printf("Packet sent (%zd bytes)\n", sent);
	}

	close(sock);
	return 0;
}
