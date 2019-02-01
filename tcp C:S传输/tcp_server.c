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
	printf("socketfd=%d\n",socketfd);
	struct sockaddr_in ser;
	memset(&ser,0,sizeof(ser));//先把ser清空
	ser.sin_family=AF_INET;//把ser初始化,告诉别人我是ipv4
	ser.sin_port=htons(atoi(argv[2]));//转成整型数
	ser.sin_addr.s_addr=inet_addr(argv[1]);//把ip转成32位网络字节
	int ret;
	ret=bind(socketfd,(struct sockaddr*)&ser,sizeof(ser));
	if(-1==ret)
	{
		perror("bind");
		return -1;
	}
	listen(socketfd,10);//数字运维会说
	int new_fd;//通过new_fd与客户端发送与接收数据
	struct sockaddr_in client;//客户端
	socklen_t addrlen=sizeof(client);
	memset(&client,0,sizeof(client));
	new_fd=accept(socketfd,(struct sockaddr*)&client,&addrlen);
	if(-1==new_fd)
	{
		perror("accept");
		return -1;
	}
	printf("clinet ip=%s,port=%d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
	char buf[128]={0};
	fd_set rdset;
	while(1)
	{
		FD_ZERO(&rdset);
		FD_SET(new_fd,&rdset);
		FD_SET(STDIN_FILENO,&rdset);
		ret=select(new_fd+1,&rdset,NULL,NULL,NULL);//写,异常,时间
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
				send(new_fd,buf,strlen(buf)-1,0);//成功返回值是长度
				
			}
			if(FD_ISSET(new_fd,&rdset))
			{
				memset(buf,0,sizeof(buf));
				ret=recv(new_fd,buf,sizeof(buf)-1,0);
				if(ret==0)
				{
					printf("client close,bye!\n");
					break;
				}
				printf("%s\n",buf);
			}
		}
	}
	close(new_fd);//四次挥手
	close(socketfd);//不允许客户端连接,我不再监听
	return 0;
}
