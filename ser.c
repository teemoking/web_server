#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#define BUFF_SIZE 	512
#define MYARGV_SIZE  5
#define PATH 	"/home/teemo/mytest/programe/web"
#define FORK_SIZE  	2
#define FILE_SIZE  	20
#define HOST  	"127.0.0.1"
#define PORT  	80
#define LISTEN_MAX 	5
#define TEM_SIZE   	10

#define PTHREAD 1


int create_socket();
void get_info(char *buff,char *argv[],char* method);
void send_suc(int c,char* path);
void send_404(int c);
void* work(void *arg);
void  myprocess(int c,char* path,char* argv[]);

int main()
{
	int socket = create_socket();//绑定监听套接字，并监听
	assert(socket != -1);
	
	while(1)
	{
		struct sockaddr_in sa;
		int len = sizeof(sa);
				int c = accept(socket,(struct sockaddr*)&sa,&len);//连接套接字
				if( c <=0 ) continue;

				if(PTHREAD)//多线程处理
				{
					pthread_t pth;
					int res = pthread_create(&pth,NULL,work,(void*)c);
					if(res == -1)
					{
						printf("pthread_create error\n");
					}
				}

			}	
		}

		void* work(void* arg)
		{
			int c = (int)arg;
			printf("%d\n",c);
			char buff[BUFF_SIZE] = {0};
			char *myargv[MYARGV_SIZE] = {0};
			while(1)
			{
				int reg = recv(c,buff,BUFF_SIZE-1,0);
				if( reg <=0 ) break;
				printf("%s\n",buff);		

				char method[TEM_SIZE] = {0};
				get_info(buff,myargv,method);

				char path[BUFF_SIZE] = {PATH};
				strcat(path,myargv[0]);
			
				if( strcmp(method,"GET") == 0 )
				{
					send_suc(c,path);
				}
				else if( strcmp(method,"POST") == 0 )
				{
					myprocess(c,path,myargv);
				}
				else
				{
					;
				}
				break;	
			}
			int res = close(c);
			//int res = closesocket(c);
			//shutdown(c,SHUT_RDWR);
			printf("close %d\n",c);
			pthread_exit(0);
		}

		void get_info(char* buff,char* argv[],char* method)
		{
			char backup[BUFF_SIZE] = {0};
			strcpy(backup,buff);

			char *p = NULL;
			char *s = strtok_r(backup,"\r\n",&p);
			if(s == NULL)
			{
				return;
			}
			
			char tem[BUFF_SIZE] = {0};
			strcpy(tem,s);

			char* line = s;
			while( (s = strtok_r(NULL,"\r\n",&p)) != NULL )
			{
				line = s;
			}
			
			s = strtok_r(tem," ",&p);
			strcpy(method,s);

			s = strtok_r(NULL," ",&p);
			argv[0] = s;
			argv[1] = line;

			return;
		}
		void myprocess(int c,char* path,char* argv[])
		{
			int fd[FORK_SIZE] = {0};
			pipe(fd);	

			/*
			char path[BUFF_SIZE]  = {PATH};//argv[1]会被覆盖掉，原因未知
			strcat(path,argv[0]);*/
				
			pid_t pid = fork();
			assert(pid != -1);
			if(pid == 0)
			{
				close(fd[0]);
				dup2(fd[1],1);
				
				int res = execv(path,argv);
				if( res == -1 )	printf("error\n"); 
				
				close(fd[1]);
				exit(0);
			}	
			close(fd[1]);
			wait(NULL);
			
			char buff[BUFF_SIZE] = {0};
			//用临时文件保存执行的结果
			int size = 0;
			int tmpfd = open("./temp",O_CREAT|O_WRONLY,0600);	

			while((size = read(fd[0],buff,BUFF_SIZE))  > 0)
			{
				if(strncmp(buff,"error",5) == 0)
				{
					send_404(c);
					return;
				}
				write(tmpfd,buff,size);
			}
			close(tmpfd);
			send_suc(c,"./temp");
			remove("./temp");//处理临时文件

			close(fd[0]);
		}

		void send_suc(int c,char* path)
		{
			int fd = open(path,O_RDONLY);
			if(fd == -1)
			{
				send_404(c);
				return ;
			}

			int size = lseek(fd,0,SEEK_END);
			lseek(fd,0,SEEK_SET);
			char filesize[FILE_SIZE]  = {0};
			//回应头信息如何确定
			char buff[BUFF_SIZE] = "HTTP/1.0 200 OK\r\n";
			strcat(buff,"Server: Ser_http\r\nContent-Length: ");

			sprintf(filesize,"%d\r\n\r\n",size);
			strcat(buff,filesize);	

			/*strcat(buff,"Content-Type: text/html;charset =gdk\r\n");
		Set-Cookie: BAIDUID=A5B6C72D68CF639CE8896FD7903FBD8:FG=1; expires=Web,04 -Jul-42 00:10:47 GMT; path=/; domain=.baidu,com\r\nVia: 1.0 localhost (squid/3.0 STABLE18)\r\n";
			*/
			//printf("send 200 ok:%s\r\n",buff);//测试专用
	send(c,buff,strlen(buff),0);//回复头信息
	
	while( sendfile(c,fd,0,BUFF_SIZE) > 0 )//浏览器请求
	{
		;
	}
	close(fd);
	return;
}
 
void send_404(int c)
{
	char buff[BUFF_SIZE] = {"HTTP/1.0 404 Not Found\r\n"};
	strcat(buff,"Server:ser_http\r\n");
	strcat(buff,"Content-Length:9\r\n");
	strcat(buff,"\r\n");
	strcat(buff,"Not Found");
	//printf("send 404:%s\r\n",buff);
	send(c,buff,strlen(buff),0);
}

int create_socket()
{
	int fd = socket(AF_INET,SOCK_STREAM,0);
	assert(fd != -1);

	struct sockaddr_in sa;
	memset(&sa,0,sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);
	sa.sin_addr.s_addr = inet_addr(HOST);

	bind(fd,(struct sockaddr*)&sa,sizeof(sa));
	
	listen(fd,LISTEN_MAX);
	return fd;
}

