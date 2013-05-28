/*
 * MediaSDK.c
 *
 *  Created on: 2011-12-23
 *      Author: TianPeng
   *      ������ý���SDK
   *      ��ý�����������ͷ����ת��������
 */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "MediaSDK.h"


#define MAX_MSG_SIZE 25
#define MAX_SOURCE_NUM 100
#define MAX_CAMERAID_LEN 16
#define STREAM_BUF_SIZE 1024 //������
#define HASH(hd) (((unsigned int)hd%MAX_SOURCE_NUM))
 
typedef struct MediaSourceInfo{
				int handle;
				uint16_t nStream;
				uint16_t nDecoder;
				uint16_t nDefine;
				int is_Transmitting;
				PFGETDATACALL cbGetData;
                void* cbArgs;
				int is_CBThreadrunning;
}MediaSourceInfo;

MediaSourceInfo *mediaSourceInfos[MAX_SOURCE_NUM];
pthread_rwlock_t MInfo_PRW = PTHREAD_RWLOCK_INITIALIZER;


/*������ý����Ϣ�洢��Ԫ*/
int allocMediaSourceInfo(uint16_t handle)
{
	int i;
    pthread_rwlock_rdlock(&MInfo_PRW);
	for(i = HASH(handle); i < MAX_SOURCE_NUM + HASH(handle); i++)
	{
		if(!mediaSourceInfos[HASH(i)])
			break;
	}
	if(i == MAX_SOURCE_NUM+HASH(handle))
	{
		printf("mediaSourceInfos[] is enough!\n");
        pthread_rwlock_unlock(&MInfo_PRW);
		return -1;
	}
	i = HASH(i);

	if( (mediaSourceInfos[i] = (MediaSourceInfo*)malloc(sizeof(MediaSourceInfo))) == NULL )
	{
		perror("malloc error");
        pthread_rwlock_unlock(&MInfo_PRW);
		return -1;
	}
    pthread_rwlock_unlock(&MInfo_PRW);
	return i;
}

/*������ý����Ϣ�洢��Ԫ*/
int findMediaSourceInfo(uint16_t handle)
{
	int i;
    pthread_rwlock_rdlock(&MInfo_PRW);
	for(i = HASH(handle); i < MAX_SOURCE_NUM + HASH(handle); i++)
	{
		if(!mediaSourceInfos[HASH(i)])
			continue;
		if(mediaSourceInfos[HASH(i)]->handle == handle)
		{
            pthread_rwlock_unlock(&MInfo_PRW);
            return HASH(i);
		}
	}
    pthread_rwlock_unlock(&MInfo_PRW);
	return -1;
}

/*�ͷ���ý����Ϣ�洢��Ԫ*/
void releaseMediaSourceInfo(MediaSourceInfo** mi)
{
	pthread_rwlock_wrlock(&MInfo_PRW);
	if(*mi)
	{
		bzero(*mi, sizeof(MediaSourceInfo));
		free(*mi);
		*mi = NULL;
	}
	pthread_rwlock_unlock(&MInfo_PRW); 
}

/*���㲢����У���*/
int Cal_Set_Sum(char *buf)
{
	int i,k;
	uint16_t sum = 0;
	k = buf[4];
	for(i=0; i<buf[4]; i++)
	{
		sum+=(uint16_t)buf[i]&0x00ff;
	}
	buf[k] = sum % 256;
	buf[k+1] = sum /256;
	return 0;
}

/*���ֱ������ͷ*/
int IPC_AddMediaSource( char* sHostIp, uint16_t sHostPort, uint16_t nStream, uint16_t nDecoder, uint16_t nDefine)
{

	int sockfd,i;
	struct sockaddr_in address;

   //�жϲ����Ƿ�������ȷ
 if(nStream == 0 || nDecoder == 0 || nDefine == 0 || nStream >2 || nDecoder>2 || nDefine>3)
 {
	 printf("argument error!\n");
	 return -1;
 }

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		perror("socket");
		return -1;
	}

	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(sHostPort);
	address.sin_addr.s_addr = inet_addr(sHostIp);

	if(connect(sockfd,(void *)&address, sizeof(address))<0)
	{
		perror("connect");
		return -1;
	}
    
	//printf("IPC:connect success!\n");

	//һ·��ý���Ӧһ���洢�ڴ浥Ԫ
	i = allocMediaSourceInfo(sockfd);
	if(i<0)
	{
		printf("IPC:allocMediaSourceInfo error!\n");
		return -1;
	}
	mediaSourceInfos[i]->handle = sockfd;
	mediaSourceInfos[i]->nStream = nStream;
	mediaSourceInfos[i]->nDecoder = nDecoder;
	mediaSourceInfos[i]->nDefine = nDefine;
	mediaSourceInfos[i]->is_Transmitting = 0;

	return sockfd;
}

/*���ת��������������ͷ*/
int IPC_AddTranMediaSource( char* sHostIp, uint16_t sHostPort, char* cameraId, uint16_t nStream, uint16_t nDecoder, uint16_t nDefine)
{

	int sockfd,i;
	struct sockaddr_in address;

   //�жϲ����Ƿ�������ȷ
 if(nStream == 0 || nDecoder == 0 || nDefine == 0 || nStream >2 || nDecoder>2 || nDefine>3)
 {
	 printf("argument error!\n");
	 return -1;
 }

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		perror("socket");
		return -1;
	}

	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(sHostPort);
	address.sin_addr.s_addr = inet_addr(sHostIp);

	if(connect(sockfd,(void *)&address, sizeof(address))<0)
	{
		perror("connect");
		return -1;
	}
	printf("connect success!\n");

	//һ·��ý���Ӧһ���洢�ڴ浥Ԫ
	i = allocMediaSourceInfo(sockfd);
	if(i<0)
	{
		printf("allocMediaSourceInfo error!\n");
		return -1;
	}
	mediaSourceInfos[i]->handle = sockfd;
	mediaSourceInfos[i]->nStream = nStream;
	mediaSourceInfos[i]->nDecoder = nDecoder;
	mediaSourceInfos[i]->nDefine = nDefine;
	mediaSourceInfos[i]->is_Transmitting = 0;

	if(send(sockfd,cameraId,MAX_CAMERAID_LEN,0)<0)
	{
		perror("send error");
		return -1;
	}
	return sockfd;
}

/*�Ƴ�����ͷ*/
int IPC_RemoveMediaSource(uint16_t handle)
{
	int sockfd;
    int i;
	sockfd = handle ;
	char buf[MAX_MSG_SIZE] = {0x60,0xf0,0x1,0x0,0x8,0x0,0x5,0x0,0x5e,0x1};
	Cal_Set_Sum(buf);
	if(send(sockfd,buf,MAX_MSG_SIZE,0) == -1) //���ͱ��� ,֪ͨIPC�Ͽ�����
	{
		perror("send error");
		return -1;
	}
    i=findMediaSourceInfo(sockfd);
    releaseMediaSourceInfo(&mediaSourceInfos[i]);//�ͷ���ý����Ϣ�洢�ڴ浥Ԫ
	close(sockfd);
	return 0;
}

/*��ʼ����*/
int IPC_StartTransmit(uint16_t handle)
{
	int sockfd,i;
	uint16_t nDecoder;
	uint16_t nStream;
	uint16_t nDefine;
	char resbuf[MAX_MSG_SIZE];
	char buf1[MAX_MSG_SIZE]={0x40,0xf0,0x1,0x0,0xc,0x0,0x1,0x0,0x3,0x0,0x3,0x0,0x44,0x1,\
						                  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};//�����������������ֱ���
	char buf2[MAX_MSG_SIZE]={0x40,0xf0,0x1,0x0,0xe,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,\
										            0x43,0x01,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};//�������������������������ͣ��Լ��Ƿ���������

	i = findMediaSourceInfo(handle);
	if(i<0)
	{
		printf("findMediaSourceInfo error!\n");
		return -1;
	}

	nStream = mediaSourceInfos[i]->nStream;
	nDecoder = mediaSourceInfos[i]->nDecoder;
	nDefine = mediaSourceInfos[i]->nDefine;
	sockfd = handle;

 switch (nStream)    //������������
   {
   case 1:
    buf1[6] = 0x1;
    buf2[6] = 0x1;
    break;
   case 2:
	   buf1[6] = 0x2;
	   buf2[6] = 0x2;
	   break;
   default:
	   break;
   }

 switch (nDefine)   //���÷ֱ���
    {
   case 1:
	   buf1[10] = 0x01;
	   break;
   case 2:
	   buf1[10] = 0x02;
   case 3:
	   buf1[10] = 0x03;
   default:
	   break;
    }

 switch (nDecoder)  //���ñ����ʽ
	 {
	 	 case 1:
	 		 buf2[10] = 0x01;
	 		 break;
	 	 case 2:
	 		 buf2[10] = 0x02;
	 		 break;
	 	 default:
	 		 break;
	 }

 	 buf2[12] = 0x01; //��������������־

 	 Cal_Set_Sum(buf1);
 	 Cal_Set_Sum(buf2);  //���㲢����У���

	if(send(sockfd,buf1,MAX_MSG_SIZE,0) == -1) //���ͱ���1
	{
		perror("send error");
		return -1;
	}

	if(recv(sockfd,resbuf,MAX_MSG_SIZE,0) == -1)  //���շ��ر���
	{
		perror("recv error");
		return -1;
	}

    /*
        for(j=0; j<MAX_MSG_SIZE; j++)  //�ж��Ƿ����
        {
            printf("0x%x ",resbuf[j]);
        }
        printf("\n");
    */



	if(send(sockfd,buf2,MAX_MSG_SIZE,0) == -1) //���ͱ���2
	{
		perror("send error");
		return -1;
	}


	if(recv(sockfd,resbuf,MAX_MSG_SIZE,0) == -1)  //���շ��ر���
	{
		perror("recv error");
		return -1;
	}


/*
    for(j=0; j<MAX_MSG_SIZE; j++)  //�ж��Ƿ����
    {
        printf("0x%x ",resbuf[j]);
    }
        printf("\n");

*/
		mediaSourceInfos[i]->is_Transmitting = 1; //���ô����־
		return 0;
}

/*ֹͣ����*/
int IPC_StopTransmit(uint16_t handle)
{
		int sockfd,i;
		uint16_t nDecoder;
		uint16_t nStream;
		uint16_t nDefine;
		char buf[MAX_MSG_SIZE]={0x40,0xf0,0x1,0x0,0xe,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,\
											            0x43,0x01,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};//�������������������������ͣ��Լ��Ƿ���������

		i = findMediaSourceInfo(handle);
		if(i<0)
		{
			printf("findMediaSourceInfo error!\n");
			return -1;
		}

		nStream = mediaSourceInfos[i]->nStream;
		nDecoder = mediaSourceInfos[i]->nDecoder;
		nDefine = mediaSourceInfos[i]->nDefine;
		sockfd = handle;

		 switch (nStream)    //������������
		   {
		   case 1:
		    buf[6] = 0x1;
		    break;
		   case 2:
			   buf[6] = 0x2;
			   break;
		   default:
			   break;
		   }

		 switch (nDecoder)  //���ñ����ʽ
			 {
			 	 case 1:
			 		 buf[10] = 0x01;
			 		 break;
			 	 case 2:
			 		 buf[10] = 0x02;
			 		 break;
			 	 default:
			 		 break;
			 }
		 buf[12] = 0x02; //��������ֹͣ��־
		 Cal_Set_Sum(buf);
			if(send(sockfd,buf,MAX_MSG_SIZE,0) == -1) //���ͱ���
			{
				perror("send error");
				return -1;
			}

		 sleep(1);
		 mediaSourceInfos[i]->is_Transmitting = 0; //���ô����־
		 return 0;
}

/*�������ص�����*/
int IPC_SetGetDataCB(uint16_t handle,PFGETDATACALL pfun, void* cbArgs)  	//����ص�����ָ��
{
    int i;
    i = findMediaSourceInfo(handle);
			if(i<0)
			{
				printf("findMediaSourceInfo error!\n");
				return -1;
			}
			mediaSourceInfos[i]->cbGetData = pfun;
            mediaSourceInfos[i]->cbArgs = cbArgs;
			mediaSourceInfos[i]->is_CBThreadrunning = 0;
			return 0;
}

/*���ص������߳�*/
void *GetDataCB_Thread(void *arg)
{
	uint16_t handle;
	PFGETDATACALL cbGetData;
	char streambuf[STREAM_BUF_SIZE];

	handle = ((MediaSourceInfo*)arg)->handle;
	cbGetData = ((MediaSourceInfo*)arg)->cbGetData;
    void* cbArgs = ((MediaSourceInfo*)arg)->cbArgs;

	if(pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL)!=0) //�����߳�״̬Ϊ��ȡ��,�������Ӧֹͣ�̣߳��������Ӧ�ṹ��
	{
	   perror("pthread_setcancelstate error");
	   exit(1);
	}
	while(1)
	{

		int i;
		char *p=streambuf;
		for(i=0; i<STREAM_BUF_SIZE/2; i++)
		{
			if(recv(handle,p,2,0) == -1)  //���շ��ر���,��������ر�sock,ֹͣ�̣߳��������Ӧ�ṹ��
			{
				perror("recv error");
				exit(1);
			}
			p = p + 2;
		}
		if(cbGetData(cbArgs,streambuf,STREAM_BUF_SIZE) < 0)//�ص���������
        {
            break;
        }      
	}
	return NULL;
}

/*�������ص������߳�*/
int IPC_SetGetDataCBRun(uint16_t handle, int flag)
{
	int i;
	pthread_t id;

	if(flag<0 || flag>1)  //У��flagֵ
	{
		printf("flag error!");
		return -1;
	}
	i =  findMediaSourceInfo(handle);
	if(i<0)
	{
		printf("findMediaSourceInfo error!\n");
		return -1;
	}

	if(flag == 1)   //flagΪ1
	{
			if(mediaSourceInfos[i]->is_CBThreadrunning == 0)   //���ص��߳�û��ִ��
			{
				//��ʼ���ص��߳�
				pthread_create(&id, NULL, GetDataCB_Thread,  (void *)mediaSourceInfos[i] ) ;
				mediaSourceInfos[i]->is_CBThreadrunning = 1;
				pthread_join( id, NULL);
                return -1;
			}
			else           //���ص��߳�����ִ��
			{
				 printf("GetDataCB is all ready run!");
				 return -1;
			}
	}
	else         //falgΪ0
	{
		if(mediaSourceInfos[i]->is_CBThreadrunning == 1)  //���ص��߳�����ִ��
		{
				//ֹͣ���ص��߳�
			pthread_cancel(id);
			mediaSourceInfos[i]->is_CBThreadrunning = 0;   //���ص��߳�û��ִ��
			return 0;
		}
		else
		{
			 printf("GetDataCB is all ready stop!");
			 return -1;
		}
	}
}





