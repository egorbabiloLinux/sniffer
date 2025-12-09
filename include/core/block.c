#include "block.h"
#include "../display/display.h"
#include <stdint.h>

unsigned long packets_total = 0; 
unsigned long bytes_total = 0;

void walk_block(struct block_desc *pbd, const int block_num) {
	int num_pkts = pbd->h1.num_pkts, i;
	unsigned long bytes = 0; 
	struct tpacket3_hdr *ppd;

	ppd = (struct tpacket3_hdr *) ((uint8_t *) pbd + pbd->h1.offset_to_first_pkt);
	
	for (i = 0; i < num_pkts; ++i) {
		bytes += ppd->tp_snaplen;
		display(ppd);

		ppd = (struct tpacket3_hdr *) ((uint8_t *) ppd + ppd->tp_next_offset);
	}

	packets_total += num_pkts;
	bytes_total += bytes;
} 

void flush_block(struct block_desc *pbd) {
	pbd->h1.block_status = TP_STATUS_KERNEL;
}