#ifndef BLOCK_H
#define BLOCK_H
#define h1 hdr.bh1

#include <linux/if_packet.h>

struct block_desc {
	__u32 version;
	__u32 offset_to_priv;
	union tpacket_bd_header_u hdr;
};

void walk_block(struct block_desc *pbd, const int block_num);
void flush_block(struct block_desc *pbd);

#endif