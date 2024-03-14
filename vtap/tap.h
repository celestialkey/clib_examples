#ifndef __TAP_H__
#define __TAP_H__

#include <stdlib.h>

#define MAX_TX_SIZE 1024
#define TAP_DEVICE  "/dev/net/tun"

// Open a new tap file
void TAPOpenInterface(int* tapfd);

// Link a TAP device to an open fd
void TAPCreateInterface(int tapfd);

// Configure an existing TAP interface
void TAPConfigureInterface(const char *interface_name, const char* ip_address, const char *netmask) ;

// Function to receive data off the TAP and send it for processing
void TAPReceiveAndProcess(int tapfd, void (*processData)(char*,ssize_t));

#endif