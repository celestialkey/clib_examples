#include <stdio.h>
#include <unistd.h>

#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>

#include "tap.h"
int cnt = 0;
void print_tcp_flags(uint16_t flags) {
    printf("Flags: ");
    if (flags & TH_FIN) printf("FIN ");
    if (flags & TH_SYN) printf("SYN ");
    if (flags & TH_RST) printf("RST ");
    if (flags & TH_PUSH) printf("PSH ");
    if (flags & TH_ACK) printf("ACK ");
    if (flags & TH_URG) printf("URG ");
    printf("\n");
}

void ProcessPacket(char* buffer, ssize_t length){
    cnt++;
    // Ethernet header
    struct ethhdr *eth_header = (struct ethhdr *)buffer;
    printf("Ethernet Header [%d]:\n", cnt);
    printf("\tSource MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
           eth_header->h_source[0], eth_header->h_source[1],
           eth_header->h_source[2], eth_header->h_source[3],
           eth_header->h_source[4], eth_header->h_source[5]);
    printf("\tDestination MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
           eth_header->h_dest[0], eth_header->h_dest[1],
           eth_header->h_dest[2], eth_header->h_dest[3],
           eth_header->h_dest[4], eth_header->h_dest[5]);
    printf("\tEthernet Type: 0x%04X\n", ntohs(eth_header->h_proto));

    // IP header
    struct iphdr *ip_header = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    printf("IP Header:\n");
    printf("\tIP S->D: %s -> %s\n", inet_ntoa(*(struct in_addr *)&ip_header->saddr), inet_ntoa(*(struct in_addr *)&ip_header->daddr));
    printf("\tProtocol: %d\n", ip_header->protocol);

    // TCP or UDP header
    if (ip_header->protocol == IPPROTO_TCP) {
        struct tcphdr *tcp_header = (struct tcphdr *)(buffer + sizeof(struct ethhdr) + ip_header->ihl * 4);
        printf("TCP Header:\n");
        printf("\tPort S->D: %d -> %d\n", ntohs(tcp_header->source), ntohs(tcp_header->dest));
        printf("\tSequence number: %u\n", ntohl(tcp_header->seq));
        printf("\tAcknowledgement number: %u\n", ntohl(tcp_header->ack_seq));
        printf("\tData offset: %u bytes\n", tcp_header->doff * 4);
        printf("\tWindow size: %u\n", ntohs(tcp_header->window));
        print_tcp_flags(tcp_header->th_flags);
    } else if (ip_header->protocol == IPPROTO_UDP) {
        struct udphdr *udp_header = (struct udphdr *)(buffer + sizeof(struct ethhdr) + ip_header->ihl * 4);
        printf("UDP Header:\n");
        printf("\tPort S->D: %d -> %d\n", ntohs(udp_header->source));
    }

    printf("\n");
}

int main(){
    int tapFd = 0;

    TAPOpenInterface(&tapFd);
    TAPCreateInterface(tapFd);
    TAPConfigureInterface("tap0", "10.10.40.13", "24");
    TAPReceiveAndProcess(tapFd, ProcessPacket);

    close(tapFd);
    return 0;
}