#ifndef  INCLUDE_MYSQL_H
#define INCLUDE_MYSQL_H
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<mysql/mysql.h>
//connect
#define CONNECT_DB  \
    "localhost","root","123456","optical_jukebox"
#define MAINTABLENAME  "main_caddy_address_map"
#define SLAVETABLENAME  "slave_caddy_address_map"
#define LOCALHOST  "192.168.1.21"
#define MAINSERVERIP  "192.168.1.2"
#define LOCALCABINETID  1
#define MAXROW   4
#define MAXCOLUMN  4

//#define MAINSERVERIP  "192.168.1.2"
MYSQL mysql;
int  updateflag;
//pthread_mutex_t  mutex;//最后未使用
//function
int mysqlinit();
void selecttable(int cabinetid,const char *tbname);
int exist(const char *caddyid,int row,int coolumn,int cabinetid,const char *tbname);
int deletetable(const char *caddyid,int row,int coolumn,int cabinetid,const char *tbname);
int insertable(const char *caddyid,int row,int coolumn,int cabinetid,const char *tbname);
int updatetable(const char *caddyid,int row,int coolumn,int cabinetid,const char *tbname);
int real_operate(const char *query);
void update(const char caddyid[10],int row,int coolumn,int cabinetid,const char *tbname);
#endif
