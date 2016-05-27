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
			//对方关闭呀
			printf("peer close\n");
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


#endif //__COMMON__

