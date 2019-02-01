#include "func.h"

int main(int argc,char *argv[])
{
	args_check(argc,3);//传第一个是ip,第二个端口号
	int socketfd=socket(AF_INET,SOCK_STREAM,0);
	if(socketfd==-1)
	{
		perror("socket");
		return -1;
	}
	printf("socket=%d\n",socketfd);
	struct sockaddr_in ser;
	memset(&ser,0,sizeof(ser));//先把ser清空
	ser.sin_family=AF_INET;//把ser初始化,告诉别人我是ipv4
	ser.sin_port=htons(atoi(argv[2]));//转成整型数
	ser.sin_addr.s_addr=inet_addr(argv[1]);//把ip转成32位网络字节
	int ret;
	ret=connect(socketfd,(struct sockaddr*)&ser,sizeof(struct sockaddr));
	if(ret==-1)
	{
		perror("connect");
		return -1;
	}
	char buf[128]={0};
	fd_set rdset;
	while(1)
	{
		FD_ZERO(&rdset);
		FD_SET(socketfd,&rdset);
		FD_SET(STDIN_FILENO,&rdset);
		ret=select(socketfd+1,&rdset,NULL,NULL,NULL);//写,异常,时间
		if(ret>0)
		{
			if(FD_ISSET(STDIN_FILENO,&rdset))
			{
				memset(buf,0,sizeof(buf));
				ret=read(STDIN_FILENO,buf,sizeof(buf)-1);
				if(ret==0)
				{
					printf("I will close\n");
					break;
				}
				send(socketfd,buf,strlen(buf)-1,0);//成功返回值是长度
				
			}
			if(FD_ISSET(socketfd,&rdset))
			{
				memset(buf,0,sizeof(buf));
				ret=recv(socketfd,buf,sizeof(buf)-1,0);
				if(ret==0)
				{
					printf("server close,bye\n");
					break;
				}
				printf("%s\n",buf);
			}
		}
	}
	close(socketfd);
	return 0;
}
