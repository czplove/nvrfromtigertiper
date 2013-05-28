/*
 * MediaSDK.h
 *
 *  Created on: 2011-12-23
 *      Author: TianPeng 
 */

#ifndef MEDIASDK_H_
#define MEDIASDK_H_

#include <stdint.h>

typedef int (* PFGETDATACALL) (void* cbArgs, char* pData, int nSize);

int IPC_AddTranMediaSource(							//���ת���������ϵ�����ͷ,ʵ���׽������ӣ���������,������ý������ sockt��
				  char*	sHostIp,				//	ת��������IP���˿�
				  uint16_t	sHostPort,
				  char* cameraId,               //����ͷID
					uint16_t		nStream,			//	��������		1:������,	 2: ������
					uint16_t		nDecoder,				//	����������		1: H264, 2: MJPEG
				  uint16_t		nDefine					//	�����С		1: 1980, 2: 1080; 3: 720
					);

int IPC_AddMediaSource(							//	���ֱ������ͷ, ʵ���׽������ӣ���������,������ý������ sockt��
				  char*	sHostIp,				//	�����IP���˿�
				  uint16_t	sHostPort,
					uint16_t		nStream,			//	��������		1:������,	 2: ������
					uint16_t		nDecoder,				//	����������		1: H264, 2: MJPEG
				  uint16_t		nDefine					//	�����С		1: 1980, 2: 1080; 3: 720
					);
int  IPC_RemoveMediaSource(uint16_t handle); //ȥ����ý�壬�ر��׽�������

int IPC_StartTransmit(uint16_t handle); //��ʼ����

int IPC_StopTransmit(uint16_t handle);  //ֹͣ����

int IPC_SetGetDataCB(uint16_t handle,PFGETDATACALL pfun, void* cbArgs); //�ص��������ڽ������ݣ�ת���ת��

int IPC_SetGetDataCBRun(uint16_t handle, int flag); //�����ص������߳�

#endif /* MEDIASDK_H_ */
