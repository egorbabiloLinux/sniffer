#ifndef DISPLAY_H
#define DISPLAY_H
#include <linux/if_packet.h>

void display(struct tpacket3_hdr *ppd);

#endif