#include<signal.h>
#include"Mysql.h"
#include"mysocket.h"
#include"mysocket_data.h"
#include"tcp_server.h"
#include <string.h>

extern void *boardserver();
int mainserflag=0;//�����������ڱ�־
void ServeAsMainServer();
int CompeteForMainServer();
void ServeAsSlaveServer();
void setmainip(const char *IP);
void setlocalip();
void *SendMysqlUpdate();

//�ж����������Ƿ����
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
//�������й����У��������ź�SIGPIPE�������ѹرյ�SOCK_STREAM�׽���ʱ���ᷢ�͸��ź�
//�������źţ���������͸����̵��źż������߳�����Ҫʹ��pthread_sigmask����
	sigset_t signal_mask;
	sigemptyset(&signal_mask);
	sigaddset(&signal_mask,SIGPIPE);
	int rc=pthread_sigmask(SIG_BLOCK,&signal_mask,NULL);
	if(rc!=0)
		printf("block sigpipe error\n");
}

//��ʱ��ʱ�䵽ʱ���źŴ�����
static void sig_alrm(int signo)
{
	timeout=1;
//	printf("alarm\n");
}
	
int main()
{
//��
	int flag=0;//�����Ƿ�Ϊ���������ı�־
//��ʼ��
	//ȫ�ֱ����洢�������Ĺ㲥��Ϣ
	memset(pkt_exist,0,MAX_CABINET_NUM*sizeof(PKT_EXIST));
	timeout=0;
	mysqlinit();
//	pthread_mutex_init(&mutex,NULL);//���û��ʹ��
	setmainip(LOCALHOST);//���Գ���ʹ�ã���ģ����ֹ������ip��ַ��Ϊԭ��ַ�����ղ�Ӧ���д���䡣
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
	alarm(5);
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

//�жϱ���IP�Ƿ�Ϊ��СIP
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
		return 1;//����СIP����ѡ�ɹ���������
	else
		return 0;//������СIP����ѡʧ��
}

//�޸�Ϊ�����Ƿ�ѡ�ɹ���1��ʾ�ɹ���0��ʾʧ��
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
	alarm(4);
//	Udp_board_init();
	listen_board();
	pthread_cancel(tid);
	flag=IsIPSminimum();
//����Ϊ�޸ģ�
//	if(flag)
//		ServeAsMainServer();
//	else
//		ServeAsSlaveServer();
	return flag;
}

//��������ַ����Ϊ����������ַ
void setmainip(const char *IP)
{
	char cmd[100];
	sprintf(cmd,"sudo ifconfig eth0 %s netmask 255.255.255.0",IP);
	system(cmd);
	system("sudo route add default gw 192.168.1.1");
//	system("sudo ifdown eth0;sudo ifup eth0");
}

//��������ַ����Ϊ�޸�ǰ��ַ�����ڵ���ʹ�ã���ʽ�汾����Ҫ
void setlocalip()
{
	char cmd[100];
	sprintf(cmd,"sudo ifconfig eth0 %s netmask 255.255.255.0",LOCALHOST);
	system(cmd);
	system("sudo route add default gw 192.168.1.1");
	exit(1);
//	system("sudo ifdown eth0;sudo ifup eth0");
}

//���մӷ������������������ݿ��
void ListenForAllTable()
{
	//��ʼ������TCP����
	int sockfd=InitMainServerTCP(TCP_PORT1);
	pthread_t tid[MLEN];
	int ret;
	int clfd[MLEN]��//ÿ�����Ӷ�Ӧ���׽�������
	int count=0;
	puts("now the main server process 1 is working");
	while(1)
	{
		{
			//clfd[count]Ϊ�˴����Ӷ�Ӧ���׽���
			clfd[count]=accept(sockfd,NULL,NULL);
			if(clfd[count]==-1)
			{
				close(clfd[count]);
				close(sockfd);
				perror("accept error\n");
			}
			//Ϊÿһ�����ӽ���һ���̣߳������䷢�������ݿ��
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

//ģ�����ϻλ�ñ䶯
void rand_num()
{
	int  caddyid;
	srand((int)time(NULL));
	memset(&pkt_update,0,sizeof(pkt_update));
	caddyid=rand()%255;
	sprintf(pkt_update.caddyid,"%d",caddyid);
	pkt_update.row=rand()%MAXROW;
	pkt_update.coolumn=rand()%MAXCOLUMN;
	pkt_update.cabinetid=LOCALCABINETID;
//�ģ����������޸�����
//printf("%d %d %d \n",caddyid,pkt_update.row,pkt_update.coolumn);
}

//���ղ��������ݿ��ĸ���
void *DealTableUpdate(void *arg)
{
	int clfd=*((int *)arg);
//�ģ������
//	printf("%d\n",clfd);
	DealPipe();
	PKT_UPDATE  pkt_recv;
	int ret;
	while(1)
	{
		ret=recv(clfd,&pkt_recv,sizeof(pkt_recv),0);
//�ģ��������
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

//���ղ��������дӷ��������������ݿ��ĸ���
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

//����ģ��Ĺ���ϻλ�ñ䶯�����±������ݿ�
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
//�ģ��������	
//	printf(" main update (update local data) success\n");
			updateflag=1;
			sleep(1);
		}
	}
}

//û���õ�
//static void deal_pipe()
//{
//	return ;
//}

//����TCP���ӣ������ظ������ݷ�����������
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
//����Ҫ�µĺ���
				//SendUpdateData(sockfd);//
				if(send(sockfd,&pkt_update,sizeof(pkt_update),0)<0)//Ϊsockfd����sock���мǣ����˵��ã������˸Ĳ���������
				{
					perror("send msg failed");
					close(sockfd);
				}
				updateflag=0;
				sleep(1);
			}
		}
	}
}

//�������Ĵӷ����������������е�����ɾ��
void DealSlaveServer()
{
	int i;
	int flag,k=0;
	for(i=0;i<MAX_CABINET_NUM;i++)
	{
		k=0;
		flag=0;
		//�ж��Ƿ�������ӷ�����i�Ĵ����Ϣ
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
		if(flag==0)//δ��������ɾ���ӷ�����i�����������е�����
			deletetable("*",0,0,i,MAINTABLENAME);
	}

}

//���������ݿ���е�����ͬ�������з�������Ϣ���ܱ���
void SynLocalTable()
{
	int num=0;
	memset(&pkt_send,0,sizeof(pkt_send));
	selecttable(LOCALCABINETID,SLAVETABLENAME);
	for(num=0;num<pkt_send.count;num++)
		insertable(pkt_send.data[num].caddyid,pkt_send.data[num].row,pkt_send.data[num].coolumn,pkt_send.data[num].cabinetid,MAINTABLENAME);
}

//��������ģ��
void ServeAsMainServer()
{
	pid_t pid1,pid2;
	pthread_t  tid[3];
	
//��ʼ��
	//��IP��ַ����Ϊ����������ַ
	setmainip(MAINSERVERIP);
	//����ܱ�
	deletetable("*",0,0,-1,MAINTABLENAME);
	//������"�ӻ��ܱ�"������"�ܻ��ܱ�"
	SynLocalTable();
	timeout=0;
	
	//��������1�����ո��ӷ������������������ݿ����Ϣ�����뵽�ܱ�
	if((pid1=fork())<0)
		perror("child process 1 fork error\n");
	else if(pid1==0)//child  process 1
		ListenForAllTable();
		else
		{
			//��������2�����ո��ӷ����������ĸ�����Ϣ�����뵽�ܱ�
			if((pid2=fork())<0)
				perror("child process 2 fork error");
			else if(pid2==0)  //child child process 2
				ListenForTableUpdate();
				else    //parent
				{
					//�����̴������߳�
					sleep(2);
					//�����߳�1�����±������ݿ⡣ʹ������������ķ�ʽ��ģ�����ϻλ�õı䶯��ʵ��ʹ��ʱ����ʹ��RFID�õ�����Ϣ
					pthread_create(&tid[0],NULL,UpdateMysqlTable,NULL);
					//�����߳�2�������µ����ݿ���Ϣ���͸���������
					pthread_create(&tid[1],NULL,SendMysqlUpdate,NULL);
					//�����߳�3,�㲥���������Լ��Ĵ����Ϣ
					pthread_create(&tid[2],NULL,boardserver,NULL);
					while(1)
					{
						memset(pkt_exist,0,MAX_CABINET_NUM*sizeof(PKT_EXIST));
						timeout=0;
						if(SIG_ERR==(signal(SIGALRM,sig_alrm)))
							perror("alarm error");
						//���ö�ʱ��
						alarm(5);
						//�����㲥��Ϣ������
						listen_board();
						//�������Ĵӷ����������������е�����ɾ��
						DealSlaveServer();
					}
				}
		}
}

//���������������Լ����������ݿ��
void SendLocalTable()
{
	int sockfd;
	DealPipe();
	//��ʼ������TCP
	sockfd=InitSlaveServerTCP(TCP_PORT1);
	if(sockfd<0)
		printf("socket num error\n");
	//��ȫ�ֱ���pkt_send�е��������ݿ����Ϣ���͸���������
	if(send(sockfd,&pkt_send,sizeof(pkt_send),0)<0)
	{
		perror("tcp_client send message error");
	}
	close(sockfd);
}

//�ӷ�����ģ��
void ServeAsSlaveServer()
{
	pthread_t tid[3];
	int num;
	//�������������ݿ��浽ȫ�ֱ���pkt_send��
	selecttable(LOCALCABINETID,SLAVETABLENAME);
	//���������������Լ����������ݿ���洢��ȫ�ֱ���pkt_send�У�
	SendLocalTable();
	
	//�����߳�1�����±������ݿ⡣ʹ������������ķ�ʽ��ģ�����ϻλ�õı䶯��ʵ��ʹ��ʱ����ʹ��RFID�õ�����Ϣ
	pthread_create(&tid[0],NULL,UpdateMysqlTable,NULL);
	//�����߳�2�������µ����ݿ���Ϣ���͸���������
	pthread_create(&tid[1],NULL,SendMysqlUpdate,NULL);
	//�����߳�3,�㲥���������Լ��Ĵ����Ϣ
	pthread_create(&tid[2],NULL,boardserver,NULL);
	while(1)
	{
		mainserflag=0;
		memset(pkt_exist,0,MAX_CABINET_NUM*sizeof(PKT_EXIST));
		timeout=0;
		if(SIG_ERR==(signal(SIGALRM,sig_alrm)))
		perror("alarm error");
		//���ö�ʱ��
		alarm(5);
		//�����㲥��Ϣ������
		listen_board();
		//�ж����������Ƿ����
		ismainexist();
		//����������������ڣ�ȡ�����̣߳���������
		if(!mainserflag)
		{
			for(num=0;num<3;num++)
				pthread_cancel(tid[num]);
			return;
		}
	}
	return;
}

