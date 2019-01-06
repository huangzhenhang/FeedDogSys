#include "recorder.h" 
#include "ff.h"
#include "malloc.h"
#include "text.h"
#include "usart.h"
#include "wm8978.h"
#include "i2s.h"
#include "led.h"
#include "lcd.h"
#include "delay.h"
#include "exfuns.h"  
#include "text.h"
#include "string.h"  
#include "include.h"
#include "GUI.h"
#include "includes.h"
#include "FramewinDLG.h"
#include "WM.h"
#include "DIALOG.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//¼���� Ӧ�ô���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
  
//u8 *i2srecbuf1;
//u8 *i2srecbuf2; 
u32 cont;

FIL* f_rec=0;		//¼���ļ�	
u32 wavsize;		//wav���ݴ�С(�ֽ���,�������ļ�ͷ!!)
//u8 rec_sta=0;		//¼��״̬
					//[7]:0,û�п���¼��;1,�Ѿ�����¼��;
					//[6:1]:����
					//[0]:0,����¼��;1,��ͣ¼��;
					
//������һ֡�������ܺ�
int rec_sum(u8* path)
{
	u8 *element = path;
	msg_T* _msg;
	OS_ERR err;
	double sum;
	u16 t;
	unsigned row;
	WM_HWIN hItem;
	sum = 0;
	
	hItem = WM_GetDialogItem(hWinDialog,ID_LISTVIEW_0); 
	row = LISTVIEW_GetSel(hItem);
	_msg = &msgGloba;	//��Ϣ�����Ǿֲ��ģ�
	if((row >= FOOD_NONE))return 0;
	else{
	for(t=0; t<=4000; t++)
	{
			sum+= ((*element)/100.0);
		  element++;
//		  if(sum>=4000000000)return 1;
	}
	if(sum>=7500.54)
	{
		OSMutexPend (&TEST_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);	//���󻥳��ź���
		_msg->srcID = TASK_AUDIO;
		_msg->what.food = row;
		_msg->what.action = ACT_OK;
		OSTaskQPost((OS_TCB*	)&MsgManageTaskTCB,	//������Msgdis������Ϣ
								(void*		)_msg,
								(OS_MSG_SIZE)(sizeof(msg_T)),
								(OS_OPT		)OS_OPT_POST_FIFO,
								(OS_ERR*	)&err);	
		printf("THE DOG NEED FOOD %d\r\n",cont++);
		OSMutexPost(&TEST_MUTEX,OS_OPT_POST_NONE,&err);				//�ͷŻ����ź���	
	}
	if(cont>600)cont=0;
	}
	return 0;
}
					
					
					
					
//¼�� I2S_DMA�����жϷ�����.���ж�����д������
void rec_i2s_dma_rx_callback(void) 
{    
//	u16 bw;
	u8 res;
	if(rec_sta==0X80)//¼��ģʽ
	{  
		if(DMA1_Stream3->CR&(1<<19))
		{
//			res=f_write(f_rec,i2srecbuf1,I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//д���ļ�
			res = rec_sum(i2srecbuf1);
			
			if(res)
			{
				printf("write error:%d\r\n",res);
			}
			 
		}else 
		{
//			res=f_write(f_rec,i2srecbuf2,I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//д���ļ�
			res = rec_sum(i2srecbuf2);
			if(res)
			{
				printf("write error:%d\r\n",res);
			}
		}
		wavsize+=I2S_RX_DMA_BUF_SIZE;
	} 
}  
const u16 i2splaybuf[2]={0X0000,0X0000};//2��16λ����,����¼��ʱI2S Master����.ѭ������0.
//����PCM ¼��ģʽ 		  
void recoder_enter_rec_mode(void)
{
	WM8978_ADDA_Cfg(0,1);		//����ADC
	WM8978_Input_Cfg(1,1,0);	//��������ͨ��(MIC&LINE IN)
	WM8978_Output_Cfg(0,1);		//����BYPASS��� 
	WM8978_MIC_Gain(0);		//MIC�������� 
	
	WM8978_I2S_Cfg(2,0);		//�����ֱ�׼,16λ���ݳ���
	I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);			//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ֡���� 
	I2S2ext_Init(I2S_Standard_Phillips,I2S_Mode_SlaveRx,I2S_CPOL_Low,I2S_DataFormat_16b);		//�����ֱ�׼,�ӻ�����,ʱ�ӵ͵�ƽ��Ч,16λ֡����	
	I2S2_SampleRate_Set(16000);	//���ò����� 
 	I2S2_TX_DMA_Init((u8*)&i2splaybuf[0],(u8*)&i2splaybuf[1],1); 		//����TX DMA 
	DMA1_Stream4->CR&=~(1<<4);	//�رմ�������ж�(���ﲻ���ж�������) 
	I2S2ext_RX_DMA_Init(i2srecbuf1,i2srecbuf2,I2S_RX_DMA_BUF_SIZE/2); 	//����RX DMA
  	i2s_rx_callback=rec_i2s_dma_rx_callback;//�ص�����ָwav_i2s_dma_callback
 	I2S_Play_Start();	//��ʼI2S���ݷ���(����)
	I2S_Rec_Start(); 	//��ʼI2S���ݽ���(�ӻ�)
//	recoder_remindmsg_show(0);
}  
//����PCM ����ģʽ 		  			    
//��ʾ¼��ʱ�������
//tsec:������.

//��ʾ��Ϣ
//mode:0,¼��ģʽ;1,����ģʽ
//ͨ��ʱ���ȡ�ļ���
//������SD������,��֧��FLASH DISK����
//��ϳ�:����"0:RECORDER/REC20120321210633.wav"���ļ���

//WAV¼�� 
void wav_recorder(void)
{ 

	u8 rval=0; 
	OS_ERR err;
	i2srecbuf1=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S¼���ڴ�1����
	i2srecbuf2=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S¼���ڴ�2����  
	if(!i2srecbuf1||!i2srecbuf2)rval=1; 	if(rval==0)		
	{
		recoder_enter_rec_mode();	//����¼��ģʽ,��ʱ��������������ͷ�ɼ�������Ƶ   
		rec_sta|=0X80;	//��ʼ¼��	
 	  while(rval==0)
		{	
				OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//��ʱ500ms
		}			 
	}    
//	myfree(SRAMIN,i2srecbuf1);	//�ͷ��ڴ�
//	myfree(SRAMIN,i2srecbuf2);	//�ͷ��ڴ�  
}



































