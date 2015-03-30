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
#define BOARDPORT 6000//广播端口//若将其修改为全局变量，会因为分别编译，而存在重复定义的错误
int sock;
struct sockaddr_in  board_sockaddr;//用于发送广播的地址
int Udp_board_init();
void *boardserver();
void  listen_board();
int timeout;
#endif
