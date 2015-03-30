#include"mysocket_data.h"
#include"mysocket.h"
#include"Mysql.h"
#define PORT  6000
void *boardserver()
{

	PKT_EXIST pkt_server;
	struct sockaddr_in addrto;
	int sock=-1;
	const int opt=-1;
	int nlen=sizeof(addrto);
	int nb=0;
	int ret=-1;
	setvbuf(stdout, NULL, _IONBF, 0); 
	fflush(stdout); 
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{   
		fprintf(stderr,"socket error\n");	
	}   
	
	//设置该套接字为广播类型，
	nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
	if(nb == -1)
	{
		fprintf(stderr,"set socket error...\n");
	}
	bzero(&addrto, sizeof(struct sockaddr_in));
	addrto.sin_family=AF_INET;
	addrto.sin_addr.s_addr=htonl(INADDR_BROADCAST);
	addrto.sin_port=htons(PORT);
	while(1)
	{
		memset(&pkt_server,0,sizeof(pkt_server));
		//从广播地址发送消息
//		memset(pkt_server,0,sizeof(PKT_EXIST));
		pkt_server.cabinetid=LOCALCABINETID;
		pkt_server.status=1;
		ret=sendto(sock,&pkt_server, sizeof(pkt_server), 0, (struct sockaddr*)&addrto, nlen);
		if(ret<0)
			fprintf(stderr,"send error....\n");
		sleep(1);
	}
}
