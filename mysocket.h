#ifndef INCLUDE_MYSOCKET_H
#define INCLUDE_MYSOCKET_H
//#include"mysocket_data.h"
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include <netdb.h>
#define BOARDPORT 6000//�㲥�˿�//�������޸�Ϊȫ�ֱ���������Ϊ�ֱ���룬�������ظ�����Ĵ���
int sock;
struct sockaddr_in  board_sockaddr;//���ڷ��͹㲥�ĵ�ַ
int Udp_board_init();
void *boardserver();
int listen_board();
int timeout;
#endif
