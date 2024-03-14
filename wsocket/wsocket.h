#ifndef __WSOCKET_H__
#define __WSOCKET_H__

int CreateTCPSocket();
int CreateUDPSocket();
void SetSocketOptions(int sockfd);
void BindSocket(int sockfd, char* ip, int port);
void ListenOnSocket(int sockfd, int backlog);

#endif
