#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <linux/if_tun.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>


#include "tap.h"


// Open an existing tap interface
void TAPOpenInterface(int* tapfd){
    // Open the TAP interface
    if((*tapfd = open(TAP_DEVICE, O_RDWR))<0){
        perror("Error opening TAP interface");
        exit(-1);
    }
}

// Create a tap interface
void TAPCreateInterface(int tapfd){
    struct ifreq ifr;

    // Initialize interface request structure
    memset(&ifr, 0, sizeof(ifr));
    // IFF_NO_PI: Do not provide packet information
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    // IFNAMSIZ vs IF_NAMESIZE
    strncpy(ifr.ifr_name, "tap0", IF_NAMESIZE);

    // Create the TAP interface
    if(ioctl(tapfd, TUNSETIFF, (void*)&ifr)<0){
        perror("Error setting up TAP interface");
        close(tapfd);
        exit(-2);
    }

    printf("Created TAP interface 'tap0'\n");
}

// Function to receive data off the TAP and send it for processing
void TAPReceiveAndProcess(int tapfd, void (*processData)(char*,ssize_t)){
    char buffer[MAX_TX_SIZE];
    while(1){
        // Read data off the tap0
        ssize_t bytes_read = read(tapfd, buffer, MAX_TX_SIZE);
        if(bytes_read < 0){
            perror("Error reading from TAP interface");
            close(tapfd);
            exit(-3);
        }

        // Process the data
        if(processData == NULL)
            continue;
        processData(buffer, bytes_read);
    }
}

void TAPConfigureInterface(const char *interface_name, const char* ip_address, const char *netmask) {
    char command[256];

    // Bring the interface down
    sprintf(command, "ip link set dev %s down", interface_name);
    system(command);

    // Assign IP address
    sprintf(command, "ip addr add %s/%s dev %s", ip_address, netmask, interface_name);
    system(command);

    // Bring the interface up
    sprintf(command, "ip link set dev %s up", interface_name);
    system(command);
}