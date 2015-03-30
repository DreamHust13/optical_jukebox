// 接收端 http://blog.csdn.net/robertkun
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include <errno.h>
#include"mysocket.h"
#include"mysocket_data.h"
int Udp_board_init()
{
	sock=-1;
//	int sock=-1;	
	int nb=0;
	const int opt=1;
	struct sockaddr_in addrto;

	setvbuf(stdout, NULL, _IONBF, 0); 
	fflush(stdout); 

	// 绑定地址
	bzero(&addrto, sizeof(struct sockaddr_in));
	addrto.sin_family = AF_INET;
	addrto.sin_addr.s_addr = htonl(INADDR_ANY);
	addrto.sin_port = htons(6000);
	
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

	if(bind(sock,(struct sockaddr *)&(addrto), sizeof(struct sockaddr_in)) == -1) 
	{   
		printf("bind error...\n");
		return -1;
	}
	return 0;
}
void  listen_board()
{
	int num=0;
	int i;
	struct sockaddr_in  from;
	int len = sizeof(struct sockaddr_in);
	int ret=0;
	PKT_EXIST  pkt_server;
	int flag;
	memset(&pkt_server,0,sizeof(PKT_EXIST));
	bzero(&from, sizeof(struct sockaddr_in));
	from.sin_family = AF_INET;
	from.sin_addr.s_addr = htonl(INADDR_ANY);
	from.sin_port = htons(6000);
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
				printf("recvfrom error\n");
		}
		else
		{
			strcpy(pkt_server.addr_ip,inet_ntoa(from.sin_addr));
			for(i=0;i<num;i++)
				if(0==strcmp(pkt_server.addr_ip,pkt_exist[i].addr_ip))
				{
					flag=0;
					break;
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
		flag=1;
	}

}

