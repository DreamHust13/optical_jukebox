#ifndef  INCLUDE_TCP_SERVER_H
#define INCLUDE_TCP_SERVER_H
#define TCP_PORT1   8000
#define TCP_PORT2   9000
#define MLEN    20
typedef  struct sockaddr_in   SOCKADDR_IN;
typedef struct sockaddr  SOCKADDR;
int  InitServer(const int port);
void *ServeForSlaveServer(void *arg);
int InitUpdateMainSerer(const int port);
int InitUpdateServer(const int port);
void SendUpdateData(int sock);
#endif
