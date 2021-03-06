#include"Mysql.h"
#include"mysocket_data.h"
int mysqlinit()
{
	int value=1;
	if(!mysql_init(&mysql))
	{
		fprintf(stderr,"mysql_init error\n");
		return -1;
	}
	mysql_options(&mysql,MYSQL_OPT_RECONNECT,(char *)&value);
	if(!mysql_real_connect(&mysql,CONNECT_DB,0,NULL,CLIENT_MULTI_RESULTS))
	{
		fprintf(stderr,"mysql connect error\n");
		return -1;
	}
	return 0;
}
void  selecttable(int cabinetid,const char *tbname)
{
	char query[100];
	MYSQL_RES  *result=NULL;
	MYSQL_ROW  line;
	int i=0,col,num=0;
	sprintf(query,"select * from %s where cabinetid=%d",tbname,cabinetid);
	if(mysql_real_query(&mysql,query,strlen(query)))
		perror("select query error");
	else
	{
		result=mysql_store_result(&mysql);
		pkt_send.count=mysql_num_rows(result);
		while((line=mysql_fetch_row(result)))
		{
			strcpy(pkt_send.data[num].caddyid,line[0]);
			pkt_send.data[num].row=line[1][0]-48;
			if(strlen(line[2])==1)
				pkt_send.data[num].coolumn=line[2][0]-48;
			else
				pkt_send.data[num].coolumn=(line[2][0]-48)*10+(line[2][1]-48);
			pkt_send.data[num].cabinetid=line[3][0]-48;
//				printf("%s %d %d %d\n",pkt_send.data[num].caddyid,pkt_send.data[num].row,pkt_send.data[num].coolumn,pkt_send.data[num].cabinetid);
			num++;
		}
		mysql_free_result(result);
	}
}
		


int exist(const char *caddyid,int row,int coolumn,int cabinetid,const char *tbname)
{
	char query[100];
	MYSQL_RES  *result=NULL;
	if(0==strcmp(caddyid,"-1"))
	{
		sprintf(query,"select * from %s where row=%d and coolumn=%d and cabinetid=%d",tbname,row,coolumn,cabinetid);
		if(0!=mysql_real_query(&mysql,query,strlen(query)))
//改：
//		{	fprintf(stderr,"row and coolumn query error\n");
//			puts(mysql_error(&mysql));
//		}
			return -1;
		else
		{
			result=mysql_store_result(&mysql);
			if(result)
			{
				if(mysql_num_rows(result)==0)
				{
					mysql_free_result(result);
					return 0;
				}
				else
				{
					mysql_free_result(result);
					return 1;
				}
			}
			else
				return 0;
		}
	}
	else
	{
		sprintf(query,"select * from %s where caddyid='%s'",tbname,caddyid);
		
		if(0!=mysql_real_query(&mysql,query,strlen(query)))
//			fprintf(stderr,"row and coolumn query error\n");
			return -1;
		else
		{
			result=mysql_store_result(&mysql);
			if(result)
			{
				if(mysql_num_rows(result)==0)
				{
					mysql_free_result(result);
					return 0;
				}
				else
				{
					mysql_free_result(result);
					return 1;
				}
			}
			else
			{
				return 0;
			}
		}
	}
}
int deletetable(const char *caddyid,int row,int coolumn,int cabinetid,const char *tbname)
{
	char query[100];
	if(0==strcmp(caddyid,"*"))
	{
		if(-1==cabinetid)
		{
			sprintf(query,"delete from %s",tbname);
			return real_operate(query);
		}
		else
		{
			sprintf(query,"delete from %s where cabinetid=%d",tbname,cabinetid);
			return real_operate(query);
		}
	}
	else if(0==strcmp(caddyid,"-1"))
	{
		sprintf(query,"delete from %s where row=%d and coolumn=%d and cabinetid=%d",tbname,row,coolumn,cabinetid);
		return real_operate(query);
	}
	else
	{
		sprintf(query,"delete from %s where caddyid=%s and cabinetid=%d",tbname,caddyid,cabinetid);
		return real_operate(query);
	}
}
int insertable(const char *caddyid,int row,int coolumn,int cabinetid,const char *tbname)
{
	char query[100];
	sprintf(query,"insert into %s values('%s',%d,%d,%d)",tbname,caddyid,row,coolumn,cabinetid);
	return real_operate(query);
}
int updatetable(const char *caddyid,int row,int coolumn,int cabinetid,const char *tbname)
{
	char query[100];
	sprintf(query,"update %s set row=%d, coolumn=%d,cabinetid=%d where caddyid='%s'",tbname,row,coolumn,cabinetid,caddyid);
	return real_operate(query);
}
int real_operate(const char *query)
{
//初始化
	MYSQL_RES *result=NULL;
//	pthread_mutex_lock(&mutex);
	if(mysql_real_query(&mysql,query,strlen(query)))
	{
//改：不输出错误信息
//		fprintf(stderr,"query operating error:");
//		puts(mysql_error(&mysql));
//		pthread_mutex_unlock(&mutex);
		return -1;
	}
	else
	{
//改：具体细节无需输出
		//printf("operating \"%s\"success\n",query);
		result=mysql_store_result(&mysql);
		if(result)
			mysql_free_result(result);
		return 0;
	}
}
void update(const char caddyid[10],int row,int coolumn,int cabinetid,const char *tbname)
{
	if(-1 == exist(caddyid,row,coolumn,cabinetid,tbname))
		return;
	if(exist("-1",row,coolumn,cabinetid,tbname))
	{
		if(0==strcmp(caddyid,"-1"))
			deletetable("-1",row,coolumn,cabinetid,tbname);
		//else
			//fprintf(stderr,"there is one caddy in the palce");
	}
	else
	{
		if(0==strcmp(caddyid,"-1"))
			fprintf(stderr,"delete error\n");
		else
		{
			if(!exist(caddyid,row,coolumn,cabinetid,tbname))
				insertable(caddyid,row,coolumn,cabinetid,tbname);
			else
				updatetable(caddyid,row,coolumn,cabinetid,tbname);
		}
	}
}
/*void rand_num()
{
	int  caddyid;
	srand((int)time(NULL));
	memset(&pkt_u,0,sizeof(pkt_update));
	caddyid=rand()%255;
	sprintf(pkt_u.caddyid,"%d",caddyid);
	pkt_u.row=rand()%MAXROW;
	pkt_u.coolumn=rand()%MAXCOLUMN;
	pkt_u.cabinetid=LOCALCABINETID;
}
int main()
{
	rand_num();
	printf("%s %d %d %d\n",pkt_u.caddyid,pkt_u.row,pkt_u.coolumn,pkt_u.cabinetid);
	update(pkt_u.caddyid,pkt_u.row,pkt_u.coolumn,pkt_u.cabinetid,"slave_caddy_address_map");
	return 0;
}*/
