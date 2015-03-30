#include<signal.h>
#include"Mysql.h"
#include"mysocket.h"
#include"mysocket_data.h"
#include"tcp_server.h"
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

extern void *boardserver();
int mainserflag=0;//主服务器存在标志
void ServeAsMainServer();
int CompeteForMainServer();
void ServeAsSlaveServer();
void setmainip(const char *IP);
void setlocalip();
void *SendMysqlUpdate();

//判断主服务器是否存在
void ismainexist()
{
	int num=0;
	while(1)
	{
		if(pkt_exist[num].status==0)
			break;
		else if(0==strcmp(MAINSERVERIP,pkt_exist[num].addr_ip))
		{
			mainserflag=1;
			break;
		}
		num++;
	}

}

static void DealPipe()
{
//程序运行过程中，会遇到信号SIGPIPE。当向已关闭的SOCK_STREAM套接字时，会发送该信号
//阻塞该信号，不将其递送给进程的信号集：在线程中需要使用pthread_sigmask函数
	sigset_t signal_mask;
	sigemptyset(&signal_mask);
	sigaddset(&signal_mask,SIGPIPE);
	int rc=pthread_sigmask(SIG_BLOCK,&signal_mask,NULL);
	if(rc!=0)
		printf("block sigpipe error\n");
}

//定时器时间到时的信号处理函数
static void sig_alrm(int signo)
{
	timeout=1;
//	printf("alarm\n");
}
	
int main()
{
//改
	int flag=0;//主机是否为主服务器的标志
//初始化
	//全局变量存储收听到的广播信息
	memset(pkt_exist,0,MAX_CABINET_NUM*sizeof(PKT_EXIST));
	timeout=0;
	mysqlinit();
//	pthread_mutex_init(&mutex,NULL);//最后没有使用
	setmainip(LOCALHOST);//调试程序使用，将模拟终止的主机ip地址设为原地址，最终不应该有此语句。
	if(Udp_board_init()==-1)
	{
		fprintf(stderr,"socket init error\n");
		return ;
	}
	
	struct sigaction alarmact;
	bzero(&alarmact,sizeof(alarmact));
	alarmact.sa_handler = sig_alrm;
	alarmact.sa_flags = SA_NOMASK;
	if(sigaction(SIGALRM,&alarmact,NULL) < 0 )
	{
		fprintf(stderr,"sigaction error\n");
		return -1;
	}
	struct itimerval timeset;//设置定时器的时间
	timeset.it_interval.tv_sec = 0;
	timeset.it_interval.tv_usec = 0;
	timeset.it_value.tv_sec = 0;
	timeset.it_value.tv_usec = 400000;//设置定时器为400ms
	if(-1 == setitimer(ITIMER_REAL,&timeset,NULL))
		perror("Setitimer error");
	//alarm(5);
	if(signal(SIGINT,setlocalip))
	{
		fprintf(stderr,"signal for Ctrl+C error\n");
		return -1;
	}
	listen_board();
	ismainexist();
	
	while(1)
	{
		if(mainserflag)
		{
			printf("main server exist\n");
			ServeAsSlaveServer();
		}
		else
		{
			flag=CompeteForMainServer();
			if(flag)
				ServeAsMainServer();
			else
				ServeAsSlaveServer();
		}
	}
	return 0;
}

//判断本机IP是否为最小IP
int IsIPSminimum()
{
	int i=0;
	char minip[16];
	strcpy(minip,LOCALHOST);
	while(1)
	{
		if(pkt_exist[i].status==0)
			break;
		else
		{
			if((strcmp(pkt_exist[i].addr_ip,minip))<0)
				strcpy(minip,pkt_exist[i].addr_ip);
		}
		i++;
	}
	if(0==strcmp(LOCALHOST,minip))
		return 1;//是最小IP，竞选成功主服务器
	else
		return 0;//不是最小IP，竞选失败
}

//修改为返回是否竞选成功：1表示成功，0表示失败
int CompeteForMainServer()
{
	int flag=0;
	pthread_t tid;
	pthread_create(&tid,NULL,boardserver,NULL);
	memset(pkt_exist,0,MAX_CABINET_NUM*sizeof(PKT_EXIST));
	timeout=0;
	if(SIG_ERR==(signal(SIGALRM,sig_alrm)))
	{
		fprintf(stderr,"sig_arm error\n");
	}
//	alarm(4);
	struct itimerval timeset;//设置定时器的时间
	timeset.it_interval.tv_sec = 0;
	timeset.it_interval.tv_usec = 0;
	timeset.it_value.tv_sec = 0;
	timeset.it_value.tv_usec = 400000;//设置定时器为400000us
	if(-1 == setitimer(ITIMER_REAL,&timeset,NULL))
		perror("Setitimer error");
//	Udp_board_init();
	listen_board();
	pthread_cancel(tid);
	flag=IsIPSminimum();
//以下为修改：
//	if(flag)
//		ServeAsMainServer();
//	else
//		ServeAsSlaveServer();
	return flag;
}

//将本机地址设置为主服务器地址
void setmainip(const char *IP)
{
	char cmd[100];
	sprintf(cmd,"sudo ifconfig eth0 %s netmask 255.255.255.0",IP);
	system(cmd);
	system("sudo route add default gw 192.168.1.1");
//	system("sudo ifdown eth0;sudo ifup eth0");
}

//将本机地址设置为修改前地址，用于调试使用，正式版本不需要
void setlocalip()
{
	char cmd[100];
	sprintf(cmd,"sudo ifconfig eth0 %s netmask 255.255.255.0",LOCALHOST);
	system(cmd);
	system("sudo route add default gw 192.168.1.1");
	exit(1);
//	system("sudo ifdown eth0;sudo ifup eth0");
}

//接收从服务器发来的整个数据库表
void ListenForAllTable()
{
	//初始化接收TCP连接
	int sockfd=InitMainServerTCP(TCP_PORT1);
	pthread_t tid[MLEN];
	int ret;
	int clfd[MLEN];//每个连接对应的套接字数组
	int count=0;
	puts("now the main server process 1 is working");
	while(1)
	{
		{
			//clfd[count]为此次连接对应的套接字
			clfd[count]=accept(sockfd,NULL,NULL);
			if(clfd[count]==-1)
			{
				close(clfd[count]);
				close(sockfd);
				perror("accept error\n");
			}
			//为每一个连接建立一个线程，处理其发来的数据库表
			ret=pthread_create(&tid[count],NULL,ServeForSlaveServer,(void *)&clfd[count]);
			if(ret!=0)
			{
				close(clfd[count]);
				close(sockfd);
				perror("thread create error\n");
			}	
			if(count++!=MLEN)
				continue;
			fprintf(stderr,"too many client,please try again\n");
		}
	}
}

//模拟光盘匣位置变动
void rand_num()
{
	int  caddyid;
	srand((int)time(NULL));
	memset(&pkt_update,0,sizeof(pkt_update));
	caddyid=rand()%25-1;
	sprintf(pkt_update.caddyid,"%d",caddyid);
	pkt_update.row=rand()%MAXROW;
	pkt_update.coolumn=rand()%MAXCOLUMN;
	pkt_update.cabinetid=LOCALCABINETID;
//改：不输出随机修改数据
//printf("%d %d %d \n",caddyid,pkt_update.row,pkt_update.coolumn);
}

//接收并处理数据库表的更新
void *DealTableUpdate(void *arg)
{
	int clfd=*((int *)arg);
//改：不输出
//	printf("%d\n",clfd);
	DealPipe();
	PKT_UPDATE  pkt_recv;
	int ret;
	while(1)
	{
		ret=recv(clfd,&pkt_recv,sizeof(pkt_recv),0);
//改：不需输出
//		printf("%d bytes receved\n",ret);
		if(ret<=0)
		{
			close(clfd);
			break;
		}
		else
			update(pkt_recv.caddyid,pkt_recv.row,pkt_recv.coolumn,pkt_recv.cabinetid,MAINTABLENAME);
	}
}

//接收并处理所有从服务器发来的数据库表的更新
void ListenForTableUpdate()
{
	int sockfd=InitMainServerTCP(TCP_PORT2);
	pthread_t tid[MLEN];
	int ret;
	int clfd[MLEN],count=0;
	puts("now the main server process2  is working");
	while(1)
	{
		{
			clfd[count]=accept(sockfd,NULL,NULL);
			if(clfd[count]==-1)
			{
				close(clfd[count]);
				close(sockfd);
				perror("accept error\n");
			}
			ret=pthread_create(&tid[count],NULL,DealTableUpdate,(void *)&clfd[count]);
			if(ret!=0)
			{
				close(clfd[count]);
				close(sockfd);
				perror("thread create error\n");
			}	
			if(count++!=MLEN)
				continue;
			fprintf(stderr,"too many client,please try again\n");
		}
	}
}

//根据模拟的光盘匣位置变动，更新本地数据库
void *UpdateMysqlTable()
{
	updateflag=0;
	printf("thread 1 is working\n");
	while(1)
	{
		if(updateflag==0)
		{
			rand_num();
			update(pkt_update.caddyid,pkt_update.row,pkt_update.coolumn,LOCALCABINETID,SLAVETABLENAME);
//改：无需输出	
//	printf(" main update (update local data) success\n");
			updateflag=1;
			usleep(1000000);//1000000微妙即1000毫秒更新一次数据
		}
	}
}

//没有用到
//static void deal_pipe()
//{
//	return ;
//}

//建立TCP连接，将本地更新数据发给主服务器
void *SendMysqlUpdate()
{
	int sockfd;
	DealPipe();
	printf("thread 2 is working\n");
	sockfd=InitSlaveServerTCP(TCP_PORT2);
	if(sockfd<0)
		printf("socet num error\n");
	else
	{
		while(1)
		{
			if(updateflag)
			{
//不需要新的函数
				//SendUpdateData(sockfd);//
				if(send(sockfd,&pkt_update,sizeof(pkt_update),0)<0)//为sockfd而非sock，切记！改了调用，别忘了改参数！！！
				{
					perror("send msg failed");
					close(sockfd);
				}
				updateflag=0;
				usleep(1000000);//1000000微妙即1000毫秒发送一次更新信息
				//sleep(1);
			}
		}
	}
}

//将不存活的从服务器在主服务器中的数据删除
void DealSlaveServer()
{
	int i;
	int flag,k=0;
	for(i=0;i<MAX_CABINET_NUM;i++)
	{
		k=0;
		flag=0;
		//判断是否监听到从服务器i的存活信息
		while(1)
		{
			if(pkt_exist[k].status==0)
				break;
			else
			{
				if(pkt_exist[k].cabinetid==i)
					flag=1;
			}
			k++;
		}
		if(flag==0)//未监听到，删除从服务器i在主服务器中的数据
			deletetable("*",0,0,i,MAINTABLENAME);
	}

}

//将本地数据库表中的数据同步到所有服务器信息的总表中
void SynLocalTable()
{
	int num=0;
	memset(&pkt_send,0,sizeof(pkt_send));
	selecttable(LOCALCABINETID,SLAVETABLENAME);
	for(num=0;num<pkt_send.count;num++)
		insertable(pkt_send.data[num].caddyid,pkt_send.data[num].row,pkt_send.data[num].coolumn,pkt_send.data[num].cabinetid,MAINTABLENAME);
}

//主服务器模块
void ServeAsMainServer()
{
	pid_t pid1,pid2;
	pthread_t  tid[3];
	
//初始化
	//将IP地址设置为主服务器地址
	setmainip(MAINSERVERIP);
	//清空总表
	deletetable("*",0,0,-1,MAINTABLENAME);
	//将本地"子货架表"更新至"总货架表"
	SynLocalTable();
	timeout=0;
	
	//创建进程1，接收各从服务器发来的整个数据库表信息并插入到总表
	if((pid1=fork())<0)
		perror("child process 1 fork error\n");
	else if(pid1==0)//child  process 1
		ListenForAllTable();
		else
		{
			//创建进程2，接收各从服务器发来的更新信息并插入到总表
			if((pid2=fork())<0)
				perror("child process 2 fork error");
			else if(pid2==0)  //child child process 2
				ListenForTableUpdate();
				else    //parent
				{
					//主进程创建三线程
					sleep(2);
					//创建线程1，更新本地数据库。使用生成随机数的方式，模拟光盘匣位置的变动，实际使用时，需使用RFID得到的信息
					pthread_create(&tid[0],NULL,UpdateMysqlTable,NULL);
					//创建线程2，将更新的数据库信息发送给主服务器
					pthread_create(&tid[1],NULL,SendMysqlUpdate,NULL);
					//创建线程3,广播主服务器自己的存活信息
					pthread_create(&tid[2],NULL,boardserver,NULL);
					while(1)
					{
						memset(pkt_exist,0,MAX_CABINET_NUM*sizeof(PKT_EXIST));
						timeout=0;
						if(SIG_ERR==(signal(SIGALRM,sig_alrm)))
							perror("alarm error");
						//设置定时器
						//alarm(5);
						struct itimerval timeset;//设置定时器的时间
						timeset.it_interval.tv_sec = 0;
						timeset.it_interval.tv_usec = 0;
						timeset.it_value.tv_sec = 0;
						timeset.it_value.tv_usec = 300000;//设置定时器为300ms
						if(-1 == setitimer(ITIMER_REAL,&timeset,NULL))
							perror("Setitimer error");
						//监听广播信息并保存
						listen_board();
						//将不存活的从服务器在主服务器中的数据删除
						DealSlaveServer();
					}
				}
		}
}

//向主服务器发送自己的整个数据库表
void SendLocalTable()
{
	int sockfd;
	DealPipe();
	//初始化发送TCP
	sockfd=InitSlaveServerTCP(TCP_PORT1);
	if(sockfd<0)
		printf("socket num error\n");
	//将全局变量pkt_send中的整个数据库表信息发送给主服务器
	if(send(sockfd,&pkt_send,sizeof(pkt_send),0)<0)
	{
		perror("tcp_client send message error");
	}
	close(sockfd);
}

//从服务器模块
void ServeAsSlaveServer()
{
	pthread_t tid[3];
	int num;
	//将本地整个数据库表存到全局变量pkt_send中
	selecttable(LOCALCABINETID,SLAVETABLENAME);
	//向主服务器发送自己的整个数据库表（存储在全局变量pkt_send中）
	SendLocalTable();
	
	//创建线程1，更新本地数据库。使用生成随机数的方式，模拟光盘匣位置的变动，实际使用时，需使用RFID得到的信息
	pthread_create(&tid[0],NULL,UpdateMysqlTable,NULL);
	//创建线程2，将更新的数据库信息发送给主服务器
	pthread_create(&tid[1],NULL,SendMysqlUpdate,NULL);
	//创建线程3,广播主服务器自己的存活信息
	pthread_create(&tid[2],NULL,boardserver,NULL);
	while(1)
	{
		mainserflag=0;
		memset(pkt_exist,0,MAX_CABINET_NUM*sizeof(PKT_EXIST));
		timeout=0;
		if(SIG_ERR==(signal(SIGALRM,sig_alrm)))
		perror("alarm error");
		//设置定时器
		struct itimerval timeset;//设置定时器的时间
		timeset.it_interval.tv_sec = 0;
		timeset.it_interval.tv_usec = 0;
		timeset.it_value.tv_sec = 0;
		timeset.it_value.tv_usec = 400000;//设置定时器为400ms
		if(-1 == setitimer(ITIMER_REAL,&timeset,NULL))
			perror("Setitimer error");
		//alarm(5);
		//监听广播信息并保存
		listen_board();
		//判断主服务器是否存在
		ismainexist();
		//如果主服务器不存在，取消三线程，函数返回
		if(!mainserflag)
		{
			for(num=0;num<3;num++)
				pthread_cancel(tid[num]);
			return;
		}
	}
	return;
}

