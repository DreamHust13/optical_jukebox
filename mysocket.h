#ifndef INCLUDE_MYSOCKET_H
#define INCLUDE_MYSOCKET_H
//#include"mysocket_data.h"
#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
int sock;
int Udp_board_init();
void  listen_board();
int timeout;
#endif
