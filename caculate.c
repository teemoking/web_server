#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>


#define BUFF_SIZE 512
#define ARGV_COUNT 5

int main(int argc,char* argv[])
{
	if(argc != 2)
	{
		exit(0);
	}
	
	char backup[BUFF_SIZE] = {0};
	strcpy(backup,argv[1]);
	char *myargv[ARGV_COUNT] = {0};

	char *s = strtok(backup,"=");
	if(s == NULL) exit(0);
	s =  strtok(NULL,"=");
	int t1 = atoi(s);
	s = strtok(NULL,"=");
	int t2 = atoi(s);
	//myargv[2] = s;

	printf("<html>\r\n<head>\r\n<meta charset=\"utf-8\">\r\n<title>主页</title>\r\n</head>\r\n\r\n");
	printf("<body>\r\n<center>\r\n网页计算器\r\n");
	printf("<form action=caculate method=\"post\">\r\n");
	printf("<input type=\"TEXT\" size=20 name=t1 value=%d>\r\n+\r\n",t1);
	printf("<input type=\"TEXT\" size=20 name=t2 value=%d>\r\n=\r\n",t2);
	printf("<input type=\"TEXT\" size=20 name=t3 value=%d>\r\n",t1+t2);
	printf("<br>\r\n<hr>\r\n<br>\r\n");
	printf("<input type=\"submit\" value=\"提交\">\r\n</form>\r\n</center>\r\n</body>\r\n</html>\r\n");

	exit(0);
}
