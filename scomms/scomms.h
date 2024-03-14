#ifndef __SCOMMS_H__
#define __SCOMMS_H__


#define MAX_TX_LENGTH   1024
#define CONNECT_TARGET  "/tmp/socket_server"

void server();
void client();

#endif