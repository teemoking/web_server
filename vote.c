#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <mysql/mysql.h>
#include  <stdlib.h>

#define NAME_BUFF 512
#define BUFF_SIZE 512
#define MYSQL_IP  "127.0.0.1"
#define MYSQL_PORT 3306
#define ITEM_SIZE 4

void out_messge(char* result,int* type);
char get_messge(char* mesg,char* name);
void my_insert(MYSQL* conn,char* name,char item);
int  my_select(MYSQL* conn,char item);
int my_select_name(MYSQL* conn,char* name);

int main(int argc,char *argv[])
{
	if(argc != 2)
	{
		exit(0);
	}
	MYSQL conn;
	mysql_init(&conn);
	if(!mysql_real_connect(&conn,"127.0.0.1","teemo","123456","web_vote",3306,NULL,0))
	{
		printf("connect error\n");
	}
		
		
	char backup[BUFF_SIZE] = {0};
	strcpy(backup,argv[1]);
	char result[BUFF_SIZE] = {0};
	int type[ITEM_SIZE] = {0};
	char name[NAME_BUFF] = {0};
	char item = get_messge(backup,name);
	if(my_select_name(&conn,name) > 0 )
	{
		strcpy(result,"您已投票，不能重复投票");
	}	
	else
	{
		strcpy(result,"投票成功");
		my_insert(&conn,name,item);
	}
	int i = 0;
	char kind[ITEM_SIZE] = "ABCD";
	for( ; i<ITEM_SIZE; i++)
	{
		type[i] = my_select(&conn,kind[i]);
	} 
	out_messge(result,type);	
	mysql_close(&conn);
}

void out_messge(char* result,int* type)
{
	printf("<html>\n<head>\n<meta charset=\"utf-8\">\n <title>网上投票</title>\n</head>\n<body>\n<body background= A.jpg>\n<center>\n<h2>体育项目投票</h2>\n<br><br><hr>\n");
	printf("%s\n<br>\n<br>\n",result);
	printf("A.篮球比赛  %d\n<br>\n<br>\n",type[0]);
	printf("B.足球比赛  %d\n<br>\n<br>\n",type[1]);
	printf("C.排球比赛  %d\n<br>\n<br>\n",type[2]);
	printf("D.网球比赛  %d\n<br>\n<br>\n",type[3]);
	printf("</center>\n</body>\n</html>\n");

}
char get_messge(char* mesg,char* name)
{
	char item;
	char *s = strtok(mesg,"=");	
	s = strtok(NULL,"&");
	item = *s;
	
	char *p = strtok(NULL,"=");
	p = strtok(NULL,"\n");
	strcpy(name,p);
	return item;
}

void my_insert(MYSQL* conn,char* name,char item)
{
	char query[BUFF_SIZE] = {0};
	sprintf(query,"insert into voter(name,item) values('%s','%c\')",name,item);
	
	int res = mysql_real_query(conn,query,strlen(query));
	if(res != 0 )
	{
		printf("query error\n");	
	}	

}
int  my_select(MYSQL* conn,char item)
{
	MYSQL_RES* res;
	MYSQL_ROW  row;
	char query[BUFF_SIZE] = {0};
	sprintf(query,"select count(*) from voter where item ='%c' ",item);
	int reg = mysql_real_query(conn,query,strlen(query));
	if(reg != 0)
	{
		printf("select error\n");
	}
	
	res = mysql_store_result(conn);
	row = mysql_fetch_row(res);
	int my_row= atoi(row[0]);
	mysql_free_result(res);
	return my_row;
}
int my_select_name(MYSQL* conn,char* name)
{
	MYSQL_ROW row;
	MYSQL_RES* res;
	
	char query[BUFF_SIZE] = {0};
	sprintf(query,"select count(*) from voter where name='%s'",name);
	int reg = mysql_real_query(conn,query,strlen(query));
	if(reg != 0)
	{
		printf("name query error\n");
	}
	res = mysql_store_result(conn);
	row = mysql_fetch_row(res);
	int my_row = atoi(row[0]);	

	mysql_free_result(res);
	return my_row;
}
