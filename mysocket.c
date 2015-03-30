// 接收端 http://blog.csdn.net/robertkun
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include <errno.h>
#include"mysocket.h"
#include"mysocket_data.h"
#include "Mysql.h"

//为广播初始化套接字
int Udp_board_init()
{
	sock=-1;
//	int sock=-1;	
	int nb=0;
	const int opt=1;
//	struct sockaddr_in addrto;//用全局变量board_sockaddr

	setvbuf(stdout, NULL, _IONBF, 0); //设置标准输出为无缓冲区
	fflush(stdout); 

	// 绑定地址
	bzero(&board_sockaddr, sizeof(struct sockaddr_in));
	board_sockaddr.sin_family = AF_INET;
	board_sockaddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	board_sockaddr.sin_port = htons(BOARDPORT);
	
	// 广播地址
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{  
	       	printf("socket error\n");	
		return -1;
	}   
	nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
	if(nb == -1)
	{
		printf("set board error\n");
		return -1;
	}

	if(bind(sock,(struct sockaddr *)&(board_sockaddr), sizeof(struct sockaddr_in)) == -1) 
	{   
		printf("bind error...\n");
		return -1;
	}
	return 0;
}

//广播本机信息
void *boardserver()
{

	PKT_EXIST pkt_server;
	int nlen=sizeof(struct sockaddr_in);
	int ret=-1;

	while(1)
	{
		memset(&pkt_server,0,sizeof(pkt_server));
		//从广播地址发送消息
//		memset(pkt_server,0,sizeof(PKT_EXIST));
		pkt_server.cabinetid=LOCALCABINETID;
		pkt_server.status=1;
		ret=sendto(sock,&pkt_server, sizeof(pkt_server), 0, (struct sockaddr*)&board_sockaddr, nlen);
		if(ret<0)
			fprintf(stderr,"send error....\n");
		usleep(200000);//200000微妙即200毫秒广播一次
		//sleep(1);
	}
}

//监听广播信息并保存
void  listen_board()
{
	int num=0;
	int i;
	struct sockaddr_in  from;//用全局变量listen_board_sockaddr出错
	int len = sizeof(struct sockaddr_in);
	int ret=0;
	PKT_EXIST  pkt_server;
	int flag;
	memset(&pkt_server,0,sizeof(PKT_EXIST));
	bzero(&from, sizeof(struct sockaddr_in));
	from.sin_family = AF_INET;
	from.sin_addr.s_addr = htonl(INADDR_ANY);
	from.sin_port = htons(BOARDPORT);
/*	if(-1==fcntl(sock, F_SETFL,O_NONBLOCK))
		perror("fcntl socket set error");*/
	while(1)
	{
		if(timeout)
			break;
		//庸悴サ刂方邮芟?
//		printf("timeout = %d",timeout);
		ret=recvfrom(sock,&pkt_server,sizeof(pkt_server), 0, (struct sockaddr*)&from,(socklen_t*)&len);
		if(ret < 0)
		{
//?
			if(errno != EINTR)
				perror("recvfrom error\n");
			//error==EINTR其实是recvfrom超时，不需处理
		}
		else
		{
			if(ret > 0)
			{//将接收到的未收到过的信息记录在全局变量pkt_exist中
				strcpy(pkt_server.addr_ip,inet_ntoa(from.sin_addr));
				for(i=0;i<num;i++)
				{
					if(0==strcmp(pkt_server.addr_ip,pkt_exist[i].addr_ip))//该ip地址的信息已记录
					{
						flag=0;
						break;
					}
				}
				if(flag)
				{
					printf("Receiving the broadcast:%s %d %d\n",pkt_server.addr_ip,pkt_server.cabinetid,pkt_server.status);
					strcpy(pkt_exist[num].addr_ip,pkt_server.addr_ip);
					pkt_exist[num].cabinetid=pkt_server.cabinetid;
					pkt_exist[num].status=pkt_server.status;
					num++;
				}
			}
		}
		flag=1;
	}

}

