#include "display.h"
#include <netinet/ip.h>
#include <netinet/ether.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h> 

void display(struct tpacket3_hdr *ppd) {
    void *net = (uint8_t *) ppd + ppd->tp_net;
    struct iphdr *iph = (struct iphdr *)net;

    void *mac = (uint8_t *) ppd + ppd->tp_mac;
    struct ethhdr *ethh = (struct ethhdr *)mac;

    struct ether_addr ether_src;
    struct ether_addr ether_dest;
    memcpy(&ether_src, ethh->h_source, sizeof(ether_src));
    memcpy(&ether_dest, ethh->h_dest, sizeof(ether_dest));

    struct in_addr src = { iph->saddr };
    struct in_addr dest = { iph->daddr };

    printf("Packet length: %u, mac src: %s, mac dest: %s, packet src: %s, packet dest: %s\n",
           ppd->tp_snaplen,
           ether_ntoa(&ether_src),
           ether_ntoa(&ether_dest),
           inet_ntoa(src),
           inet_ntoa(dest));
}
