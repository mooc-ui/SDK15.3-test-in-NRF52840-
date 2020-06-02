/* Copyright (c) 2016 MegaHealth. All Rights Reserved.
 * mring_queue.h
 * AUTHOR:zhao mengshou
 * DATE:2016-11-14 10:00
 * http://www.megahealth.cn
 *
 */

#ifndef MRING_QUEUE_H  
#define MRING_QUEUE_H  
  
#ifdef __cplusplus  
extern "C"  
{  
#endif

#include "list.h"
#include <string.h>

//typedef tListNode* mrQueueHandle_t;
enum queueType
{
	SYS_EVT_QUEUE = 0,
	AFE_I2C_QUEUE = 1,
	ACC_I2C_QUEUE = 2,
    NON_EVT_QUEUE = 3,

	QUEUE_MAX,
};

typedef struct	_tmrQueue_t
{
	tListNode EvtQueue;
	tListNode EvtPool;	
	int poolEleNum;
	int poolEleSize;
}mrQueue_t, *mrQueueHandle_t;

typedef struct _t_queue_element
{
	tListNode currentNode;
	void *buffer;
} queue_element;

/************************************** 
 * structure used to queue
 */
#define MRING_QUEUE_NUM	(32)
typedef struct _stEvtBuffer
{
	uint16_t	evt;
	void* payload;//task的参数      //该变量只接受另外一个变量的地址eg: stEvtBuffer temp;  temp.payload = &othor_var;
}stEvtBuffer;

/***************************************
 * structure used to acc
 */
#define ACC_FIFO_NUM	(4)
typedef struct _acc_axis
{
	int16_t x;				//	asix_x of data raw acceleration
	int16_t y;				//	asix_y of data raw acceleration
	int16_t z;				//	asix_z of data raw acceleration
	int16_t gyro[3];		//	data raw angular rate
}acc_axis;

#define SIXAXIS_SIZE_NUM	(32)
typedef struct _sixaxis_fifo
{
	int valid_num; 
	acc_axis axis[SIXAXIS_SIZE_NUM];
}sixaxis_fifo;
/***************************************
 * structure used to afe4404
 */
#define AFE_FIFO_NUM	(12)
typedef struct _afe_data
{
	int ledRed;				
	int ledGreen;
	int ledambient1;
}afe_data;

#define AFE4405_FIFO_NUM		(2)
#define AFE4405_ARRAY_SIZE		(50)
typedef struct _afe4405_fifo_data
{
	struct {
        int ledGreen;
		int ledRed;
        int ledInfra;
		int ledambient1;
	} recipes[AFE4405_ARRAY_SIZE];
}afe4405_fifo_data;

/*
typedef struct _tafe_queue
{
	tListNode currentNode;
	afe_data afeData;
} afe_queue;
*/

extern mrQueueHandle_t mrEvtQueue;
extern mrQueueHandle_t mrAccQueue;
extern mrQueueHandle_t mrAfeQueue;   

mrQueueHandle_t mring_queueInit(enum queueType queTp);
bool mring_queueSendToFront(mrQueueHandle_t xQueue, const void * const pvBuffer);
bool mring_queueSendToBack(mrQueueHandle_t xQueue, const void * const pvBuffer);
bool mring_queueSend(mrQueueHandle_t xQueue, const void * const pvBuffer);
bool mring_queueReceive(mrQueueHandle_t xQueue, void * const pvBuffer);
bool mring_queuePeek(mrQueueHandle_t xQueue, void * const pvBuffer);
int	 mring_queueWaiting(mrQueueHandle_t xQueue);
bool mring_queueDeInit(mrQueueHandle_t xQueue);
bool mring_queueIsFull(mrQueueHandle_t xQueue);
bool mring_queueIsEmpty(mrQueueHandle_t xQueue);

#ifdef __cplusplus  
}  
#endif  
#endif /* MRING_QUEUE_H */  

