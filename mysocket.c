// ���ն� http://blog.csdn.net/robertkun
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include <errno.h>
#include"mysocket.h"
#include"mysocket_data.h"
#include "Mysql.h"

//Ϊ�㲥��ʼ���׽���
int Udp_board_init()
{
	sock=-1;
//	int sock=-1;	
	int nb=0;
	const int opt=1;
//	struct sockaddr_in addrto;//��ȫ�ֱ���board_sockaddr

	setvbuf(stdout, NULL, _IONBF, 0); //���ñ�׼���Ϊ�޻�����
	fflush(stdout); 

	// �󶨵�ַ
	bzero(&board_sockaddr, sizeof(struct sockaddr_in));
	board_sockaddr.sin_family = AF_INET;
	board_sockaddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	board_sockaddr.sin_port = htons(BOARDPORT);
	
	// �㲥��ַ
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

//�㲥������Ϣ
void *boardserver()
{

	PKT_EXIST pkt_server;
	int nlen=sizeof(struct sockaddr_in);
	int ret=-1;

	while(1)
	{
		memset(&pkt_server,0,sizeof(pkt_server));
		//�ӹ㲥��ַ������Ϣ
//		memset(pkt_server,0,sizeof(PKT_EXIST));
		pkt_server.cabinetid=LOCALCABINETID;
		pkt_server.status=1;
		ret=sendto(sock,&pkt_server, sizeof(pkt_server), 0, (struct sockaddr*)&board_sockaddr, nlen);
		if(ret<0)
			fprintf(stderr,"send error....\n");
		usleep(200000);//200000΢�200����㲥һ��
		//sleep(1);
	}
}

//�����㲥��Ϣ������
int  listen_board()
{
	int num=0;
	int i;
	struct sockaddr_in  from;//��ȫ�ֱ���listen_board_sockaddr����
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
			return num;

		//		printf("timeout = %d",timeout);
		ret=recvfrom(sock,&pkt_server,sizeof(pkt_server), 0, (struct sockaddr*)&from,(socklen_t*)&len);
		if(ret < 0)
		{
//?
			if(errno != EINTR)
				perror("recvfrom error\n");
			//error==EINTR��ʵ��recvfrom��ʱ�����账��
		}
		else
		{
			if(ret > 0)
			{//�����յ���δ�յ�������Ϣ��¼��ȫ�ֱ���pkt_exist��
				strcpy(pkt_server.addr_ip,inet_ntoa(from.sin_addr));
				for(i=0;i<num;i++)
				{
					if(0==strcmp(pkt_server.addr_ip,pkt_exist[i].addr_ip))//��ip��ַ����Ϣ�Ѽ�¼
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

