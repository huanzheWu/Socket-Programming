#ifndef __COMMON__
#define __COMMON__
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
# define err_exit(m)\
	do\
	{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0)

void handler(int sig)
{
	printf("recv a sig = %d \n",sig);
	exit(EXIT_SUCCESS);
}


struct packet
{
	unsigned int msgLen ;
	char data [1024];
};
/*
readn 函数
读取count字节数据
*/
ssize_t readn(int fd,void *buf, size_t count)
{
	int left = count ; //剩下的字节
	char * ptr = (char*)buf ;
	while(left>0)
	{
		int readBytes = read(fd,ptr,left);
		if(readBytes< 0)//read函数小于0有两种情况：1中断 2出错
		{
			if(errno == EINTR)//读被中断
			{
				continue;
			}	
			return -1;
		}
		if(readBytes == 0)//读到了EOF
		{
			return count - left;
		}	
		left -= readBytes;
		ptr += readBytes ;
	}
	return count ;
}

/*
writen 函数
写入count字节的数据
*/
ssize_t writen(int fd, void *buf, size_t count)
{
	int left = count ;
	char * ptr = (char *)buf; 			
	while(left >0)
	{
		int writeBytes = write(fd,ptr,left);
		if(writeBytes<0)
		{
			if(errno == EINTR)
				continue;
			return -1;
		}
		else if(writeBytes == 0)
			continue;
		left -= writeBytes;
		ptr += writeBytes; 
	}
	return count;
}

/*
从缓冲区中读取指定长度的数据，但不清楚缓冲区内容
*/
ssize_t read_peek(int sockfd, void *buf, size_t len )
{
	while(1)
	{
		int ret = recv (sockfd , buf ,len ,MSG_PEEK);
		if(ret  == -1)
		{
			if(errno ==EINTR)  //出现中断
				continue ;
		}
		return ret ;
	}		
}

/*
按行读取数据
参数说明：
	sockfd:套接字
	buf ：应用层缓冲区，保存读取到的数据
	maxline:所规定的一行的长度
返回值说明：	
*/
ssize_t readLine (int sockfd , void *buf ,size_t maxline)
{
	int ret;
	int nRead = 0;
	int left = maxline ; //剩下的字节数
	char * pbuf = (char *) buf ; 
	int count = 0;
	while(1)
	{
		ret = read_peek ( sockfd, pbuf, left);   //读取长度为left的socket缓冲区内容
		if(ret <0)
		{
			return ret;
		}
		nRead = ret;
		for(int i = 0;i<nRead;++i)//看看读取出来的数据中是否有换行符\n
		{
			if(pbuf[i]=='\n')//如果有换行符
			{
				ret = readn(sockfd , pbuf , i+1);//读取一行
				if(ret != i+1)	//一定会读到i+1个字符，否则是读取出错
				{
					exit(EXIT_FAILURE);	
				}
				return ret + count ;
			}
		}
		//窥探的数据中并没有换行符
		//把这段没有\n的读出来
		ret = readn(sockfd , pbuf,nRead); 
		if(ret != nRead )
		{
			exit(EXIT_FAILURE);	
		}
		pbuf += nRead;
		left -= nRead;
		count += nRead;	
	}
	return -1;
	
}
#endif //__COMMON__
