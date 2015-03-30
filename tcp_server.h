#ifndef  INCLUDE_TCP_SERVER_H
#define INCLUDE_TCP_SERVER_H
#define TCP_PORT1   8000
#define TCP_PORT2   9000
#define MLEN    20
#define MAXSLEEP 128
#include"mysocket.h"
typedef  struct sockaddr_in   SOCKADDR_IN;
typedef struct sockaddr  SOCKADDR;
int  InitMainServerTCP(const int port);
void *ServeForSlaveServer(void *arg);
//int InitUpdateMainSerer(const int port);//与InitServer()合并为InitMainServerTCP
int connect_retry(int sockfd,const struct sockaddr *addr,socklen_t alen);
int InitSlaveServerTCP(const int port);
void SendUpdateData(int sock);
#endif
