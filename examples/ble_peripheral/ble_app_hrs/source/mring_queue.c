/* Copyright (c) 2016 MegaHealth. All Rights Reserved.
 * mring_queue.h
 * AUTHOR:zhao mengshou
 * DATE:2016-11-14 10:00
 * http://www.megahealth.cn
 *
 */

#include "mring_queue.h"
#include "mring_it.h"

//static tListNode mringEvtQueue;
//static tListNode mringEvtPool;

static mrQueue_t	mringQueue[QUEUE_MAX];

static stEvtBuffer evtMemBuffer[MRING_QUEUE_NUM];
static queue_element evtQueBuffer[MRING_QUEUE_NUM];

//static mring_queue mringEvtMemBuffer[MRING_QUEUE_NUM];


//ACC G-SENSOR FIFO define
static sixaxis_fifo accMemBuffer[ACC_FIFO_NUM];
static queue_element accQueBuffer[ACC_FIFO_NUM];

//AFE4404 FIFO define
//static afe_data afeMemBuffer[AFE_FIFO_NUM];
//static queue_element afeQueBuffer[AFE_FIFO_NUM];

//AFE4405 FIFO define
static afe4405_fifo_data afeMemBuffer[AFE4405_FIFO_NUM];
static queue_element afeQueBuffer[AFE4405_FIFO_NUM];

#define ASSERT_MRING_QUEUE(X)		do{\
										uint32_t err_code;\
										err_code = (X==NULL)?NRF_ERROR_NULL:NRF_SUCCESS;\
										APP_ERROR_CHECK(err_code);\
										}while(0)

mrQueueHandle_t mring_queueInit(enum queueType queTp)
{
	int index;
	if(queTp >= QUEUE_MAX)
		return NULL;
	
	MR_ENTER_CRITICAL();
	mrQueueHandle_t xQueue = &mringQueue[queTp];
	list_init_head (&(xQueue->EvtQueue));
	list_init_head (&(xQueue->EvtPool));

	switch(queTp)
	{
	case SYS_EVT_QUEUE:
		for(index=0; index < MRING_QUEUE_NUM; index++)
		{
			evtQueBuffer[index].buffer = &evtMemBuffer[index];
			list_insert_tail(&(xQueue->EvtPool), (tListNode *)&evtQueBuffer[index]);
		}
		xQueue->poolEleNum = MRING_QUEUE_NUM;
		xQueue->poolEleSize = sizeof(stEvtBuffer);
		break;

	case AFE_I2C_QUEUE:
		for(index=0; index < /*AFE_FIFO_NUM*/AFE4405_FIFO_NUM; index++)
		{
			afeQueBuffer[index].buffer = &afeMemBuffer[index];
			list_insert_tail(&(xQueue->EvtPool), (tListNode *)&afeQueBuffer[index]);
		}
		xQueue->poolEleNum = /*AFE_FIFO_NUM*/AFE4405_FIFO_NUM;
		xQueue->poolEleSize = sizeof(afe4405_fifo_data);
		break;

	case ACC_I2C_QUEUE:
		for(index=0; index < ACC_FIFO_NUM; index++)
		{
			accQueBuffer[index].buffer = &accMemBuffer[index];
			list_insert_tail(&(xQueue->EvtPool), (tListNode *)&accQueBuffer[index]);
		}
		xQueue->poolEleNum = ACC_FIFO_NUM;
		xQueue->poolEleSize = sizeof(sixaxis_fifo);
		break;
		
	default:
		break;
	}
	
	MR_EXIT_CRITICAL();

	return xQueue;
}

bool mring_queueSendToFront(mrQueueHandle_t xQueue, const void * const pvBuffer)
{
	ASSERT_MRING_QUEUE(xQueue);
	ASSERT_MRING_QUEUE(pvBuffer);
	
	MR_ENTER_CRITICAL();
	if(list_is_empty(&(xQueue->EvtPool)) == true)
	{
		MR_EXIT_CRITICAL();
		return false;
	}
	queue_element *mrValidPkt;
	list_remove_head(&(xQueue->EvtPool), (pListNode*)&mrValidPkt);
	(void) memcpy((void*)(mrValidPkt->buffer), pvBuffer, xQueue->poolEleSize);
	list_insert_head(&(xQueue->EvtQueue), (pListNode)mrValidPkt);
	MR_EXIT_CRITICAL();

	return true;
}

bool mring_queueSendToBack(mrQueueHandle_t xQueue, const void * const pvBuffer)
{
	ASSERT_MRING_QUEUE(xQueue);
	ASSERT_MRING_QUEUE(pvBuffer);
	
	MR_ENTER_CRITICAL();
	if(list_is_empty(&(xQueue->EvtPool)) == true)
	{
		MR_EXIT_CRITICAL();
		return false;
	}
	queue_element *mrValidPkt;
	list_remove_head(&(xQueue->EvtPool), (pListNode*)&mrValidPkt);
	(void) memcpy((void*)(mrValidPkt->buffer), pvBuffer, xQueue->poolEleSize);
	list_insert_tail(&(xQueue->EvtQueue), (pListNode)mrValidPkt);
	MR_EXIT_CRITICAL();

	return true;;
}

bool mring_queueSend(mrQueueHandle_t xQueue, const void * const pvBuffer)
{
	return mring_queueSendToBack(xQueue, pvBuffer);
}

bool mring_queueReceive(mrQueueHandle_t xQueue, void * const pvBuffer)
{
	ASSERT_MRING_QUEUE(xQueue);
	ASSERT_MRING_QUEUE(pvBuffer);
	
	MR_ENTER_CRITICAL();
	if(true == list_is_empty(&(xQueue->EvtQueue)))
	{
		//pvBuffer = NULL;
		( void ) memset( ( void * ) pvBuffer, 0xFF, xQueue->poolEleSize);		
		MR_EXIT_CRITICAL();
		return false;
	}
	queue_element *mrValidPkt;
	list_remove_head(&(xQueue->EvtQueue), (pListNode*)&mrValidPkt);
	( void ) memcpy( ( void * ) pvBuffer, (void*)(mrValidPkt->buffer), xQueue->poolEleSize);
	list_insert_tail(&(xQueue->EvtPool), (pListNode)mrValidPkt);
	MR_EXIT_CRITICAL();

	return true;
}

bool mring_queuePeek(mrQueueHandle_t xQueue, void * const pvBuffer)
{
	ASSERT_MRING_QUEUE(xQueue);
	ASSERT_MRING_QUEUE(pvBuffer);
	
	MR_ENTER_CRITICAL();
	if(true == list_is_empty(&(xQueue->EvtQueue)))
	{
		//pvBuffer = NULL;
		( void ) memset( ( void * ) pvBuffer, 0xFF, xQueue->poolEleSize);	
		MR_EXIT_CRITICAL();
		return false;
	}
	queue_element *mrValidPkt;
	list_remove_head(&(xQueue->EvtQueue), (pListNode*)&mrValidPkt);
	( void ) memcpy( ( void * ) pvBuffer, (void*)(mrValidPkt->buffer), xQueue->poolEleSize);
	list_insert_head(&(xQueue->EvtQueue), (pListNode)mrValidPkt);
	MR_EXIT_CRITICAL();

	return true;
}

int mring_queueWaiting(mrQueueHandle_t xQueue)
{
	ASSERT_MRING_QUEUE(xQueue);
	
	int num; 
	MR_ENTER_CRITICAL();
	num = list_get_size(&(xQueue->EvtQueue));
	MR_EXIT_CRITICAL();
	return num;
}

bool mring_queueDeInit(mrQueueHandle_t xQueue)
{
	ASSERT_MRING_QUEUE(xQueue);
	MR_ENTER_CRITICAL();
	list_init_head(&(xQueue->EvtQueue));
	list_init_head(&(xQueue->EvtPool));
	MR_EXIT_CRITICAL();
	return true;
}

bool mring_queueIsFull(mrQueueHandle_t xQueue)
{
	ASSERT_MRING_QUEUE(xQueue);
	
	return list_get_size(&(xQueue->EvtQueue)) == xQueue->poolEleSize;
}

bool mring_queueIsEmpty(mrQueueHandle_t xQueue)
{
	ASSERT_MRING_QUEUE(xQueue);
	
	return list_is_empty(&(xQueue->EvtQueue));
}

