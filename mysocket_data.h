#ifndef INCLUDE_MYSOCKET_DATA_H
#define INCLUDE_MYSCCKET_DATA_H
#include<stdio.h>
#define MAX_CADDY_NUM  240
#define MAX_CABINET_NUM  10
typedef struct 
{
	char caddyid[10];
	int row,coolumn,cabinetid;
}PKT_UPDATE;
typedef struct 
{
	int      count;
	PKT_UPDATE      data[MAX_CADDY_NUM];
}PKT_SEND;
typedef struct
{
	int cabinetid;
	char addr_ip[16];
	int status;
}PKT_EXIST;
PKT_SEND   pkt_send;
PKT_EXIST  pkt_exist[MAX_CABINET_NUM];
PKT_UPDATE pkt_update;
#endif
