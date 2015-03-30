#include"mysocket.h"
#include"mysocket_data.h"
#include"tcp_server.h"
#include"Mysql.h"

//初始化主服务器端的TCP连接(原名InitServer)
int  InitMainServerTCP(const int port)
{
	int sockfd;
	SOCKADDR_IN addrSrv;
	int len=sizeof(SOCKADDR_IN);
	int reuse=1;
	bzero(&addrSrv,sizeof(addrSrv));
	addrSrv.sin_addr.s_addr=INADDR_ANY;
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(port);
	//设置TCP复用
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		perror("tcp main socket error");
		return -1;
	}
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int))<0)
	{
		fprintf(stderr,"set sock option error\n");
		return -1;
	}
	if(bind(sockfd,(SOCKADDR *)&addrSrv,len)<0)
	{
		perror("Main server tcp bind error\n");
		return -1;
	}
	if(listen(sockfd,MLEN)<0)
	{
		perror("Main server tcp listen error\n");
		return -1;
	}
	return sockfd;
}

//将TCP接收到的整张从服务器数据库表插入总表
void *ServeForSlaveServer(void *arg)
{

	PKT_SEND  pkt_recv;
	int ret;
	int num=0;
	int sockfd=*((int *)arg);
	//memset(&pkt_recv,0,sizeof(PKT_SEND));
	ret=recv(sockfd,(char *)&pkt_recv,sizeof(pkt_recv),MSG_WAITALL);
	if(ret<0)
	{
		perror("client quit");
		close(sockfd);
		return ;
	}
	else
	{
//改：不用输出
		printf("The table count received from cabinet %d is %d\n\n",pkt_recv.data[0].cabinetid,pkt_recv.count);
		for(num=0;num<pkt_recv.count;num++)
		{
		insertable(pkt_recv.data[num].caddyid,pkt_recv.data[num].row,pkt_recv.data[num].coolumn,pkt_recv.data[num].cabinetid,MAINTABLENAME);
		}
		printf("已向总表插入%d\n\n",num);
	}
	close(sockfd);
}

//与InitServer()函数合并，改为InitMainServerTCP()
/* int InitUpdateMainServer(const int port)
{
	int sockfd;
	SOCKADDR_IN  addr;
	int reuse=1;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
		perror("tcp main socket error");
	bzero(&addr,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);
	addr.sin_addr.s_addr=INADDR_ANY;
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int))<0)
	{
		fprintf(stderr,"set sock option error\n");
		return -1;
	}
	if(bind(sockfd,(SOCKADDR *)&addr,sizeof(SOCKADDR_IN))<0)
		perror("tcp main bind error");
	if(listen(sockfd,MLEN)<0)
		perror("tcp listen error");
	return sockfd;
} */

int connect_retry(int sockfd,const struct sockaddr *addr,socklen_t alen)
//支持重试的连接
{
	int nsec;
	for(nsec=1;nsec<= MAXSLEEP;nsec<<=1)
	{
		if(connect(sockfd,addr,alen) == 0)
			return 0;
		if(nsec <= MAXSLEEP/2)
			usleep(nsec*100000);//微妙单位
	}
	return -1;
	
}
//初始化从服务器端的TCP连接(原名InitUpdateServer)
int InitSlaveServerTCP(const int port)
{
	int socktd;
	SOCKADDR_IN  desaddr;
	socktd=socket(AF_INET,SOCK_STREAM,0);
	if(socktd<0)
	{
		perror("socket error\n");
		return -1;
	}
	bzero(&desaddr,sizeof(desaddr));
	desaddr.sin_family=AF_INET;
	desaddr.sin_addr.s_addr=inet_addr(MAINSERVERIP);
	desaddr.sin_port=htons(port);

	if(connect_retry(socktd,(SOCKADDR *)&desaddr,sizeof(SOCKADDR_IN))<0)
	{
		perror("connect error\n");
		return -1;
	}
	return socktd;
}

//发送更新数据的实际操作
//删掉了，不需要
/* void  SendUpdateData(int sock)
{
	if(send(sock,&pkt_update,sizeof(pkt_update),0)<0)
	{
		perror("send msg failed");
		close(sock);
	}
} */


