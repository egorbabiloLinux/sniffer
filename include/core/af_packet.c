#include <netinet/ip.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <poll.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <linux/if_packet.h>
#include "block.h"
#include "../display/display.h"

int main(int argc, char **argp) {
	int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (fd < 0) {
		int saved_errno = errno;
		fprintf(stderr, "socket creation failed: %s\n", strerror(saved_errno = errno));
		exit(EXIT_FAILURE);
	}
	
	int err;
	int v = TPACKET_V3; 
	err = setsockopt(fd, SOL_PACKET, PACKET_VERSION, &v, sizeof(v));
	if (err < 0) {
		int saved_errno = errno;
		fprintf(stderr, "setsockopt failed: %s\n", strerror(saved_errno = errno));
		exit(EXIT_FAILURE);
	}

	struct tpacket_req3 req;
	req.tp_block_size = 4096;
	req.tp_frame_size = 2048;
	req.tp_block_nr = 128;
	req.tp_frame_nr = (req.tp_block_nr * req.tp_block_size) / req.tp_frame_size;
	err = setsockopt(fd, SOL_PACKET, PACKET_RX_RING, &req, sizeof(req));
	if (err < 0) {
		int saved_errno = errno;
		fprintf(stderr, "rx ring creationl failed: %s\n", strerror(saved_errno = errno));
		exit(EXIT_FAILURE);
	}	

	size_t total_size = (size_t)req.tp_block_size * req.tp_block_nr;

	void *ring = mmap(NULL, total_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (ring == MAP_FAILED) {
		perror("mmap error");
		close(fd);
		exit(EXIT_FAILURE);
	}
	
	struct iovec *rd;
    rd = calloc(req.tp_block_nr, sizeof(*rd));
	for (int i = 0; i < req.tp_block_nr; i++) {
		rd[i].iov_base = ring + (i * req.tp_block_size);
		rd[i].iov_len = req.tp_block_size;
	}


	struct pollfd pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.fd = fd;
	pfd.events = POLLIN | POLLERR;
	pfd.revents = 0;

	int block_num = 0;
	int blocks = req.tp_block_nr;

	while(1) {
		struct block_desc *pbd = (struct block_desc *) rd[block_num].iov_base;

		if ((pbd->h1.block_status & TP_STATUS_USER) == 0) {
			poll(&pfd, 1, -1); //СДЕЛАТЬ ПРОВЕРКУ НА ПРЕРВЫНИЕ СИГНАЛОМ
			continue;
		}

		walk_block(pbd, block_num);
		flush_block(pbd);
		block_num = (block_num + 1) % blocks;
	}

	free(rd);
	munmap(ring, total_size);
	close(fd);
	return 0;
}
