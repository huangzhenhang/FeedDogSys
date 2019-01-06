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
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//录音机 应用代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
  
//u8 *i2srecbuf1;
//u8 *i2srecbuf2; 
u32 cont;

FIL* f_rec=0;		//录音文件	
u32 wavsize;		//wav数据大小(字节数,不包括文件头!!)
//u8 rec_sta=0;		//录音状态
					//[7]:0,没有开启录音;1,已经开启录音;
					//[6:1]:保留
					//[0]:0,正在录音;1,暂停录音;
					
//计算这一帧语音的总和
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
	_msg = &msgGloba;	//消息不能是局部的！
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
		OSMutexPend (&TEST_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);	//请求互斥信号量
		_msg->srcID = TASK_AUDIO;
		_msg->what.food = row;
		_msg->what.action = ACT_OK;
		OSTaskQPost((OS_TCB*	)&MsgManageTaskTCB,	//向任务Msgdis发送消息
								(void*		)_msg,
								(OS_MSG_SIZE)(sizeof(msg_T)),
								(OS_OPT		)OS_OPT_POST_FIFO,
								(OS_ERR*	)&err);	
		printf("THE DOG NEED FOOD %d\r\n",cont++);
		OSMutexPost(&TEST_MUTEX,OS_OPT_POST_NONE,&err);				//释放互斥信号量	
	}
	if(cont>600)cont=0;
	}
	return 0;
}
					
					
					
					
//录音 I2S_DMA接收中断服务函数.在中断里面写入数据
void rec_i2s_dma_rx_callback(void) 
{    
//	u16 bw;
	u8 res;
	if(rec_sta==0X80)//录音模式
	{  
		if(DMA1_Stream3->CR&(1<<19))
		{
//			res=f_write(f_rec,i2srecbuf1,I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//写入文件
			res = rec_sum(i2srecbuf1);
			
			if(res)
			{
				printf("write error:%d\r\n",res);
			}
			 
		}else 
		{
//			res=f_write(f_rec,i2srecbuf2,I2S_RX_DMA_BUF_SIZE,(UINT*)&bw);//写入文件
			res = rec_sum(i2srecbuf2);
			if(res)
			{
				printf("write error:%d\r\n",res);
			}
		}
		wavsize+=I2S_RX_DMA_BUF_SIZE;
	} 
}  
const u16 i2splaybuf[2]={0X0000,0X0000};//2个16位数据,用于录音时I2S Master发送.循环发送0.
//进入PCM 录音模式 		  
void recoder_enter_rec_mode(void)
{
	WM8978_ADDA_Cfg(0,1);		//开启ADC
	WM8978_Input_Cfg(1,1,0);	//开启输入通道(MIC&LINE IN)
	WM8978_Output_Cfg(0,1);		//开启BYPASS输出 
	WM8978_MIC_Gain(0);		//MIC增益设置 
	
	WM8978_I2S_Cfg(2,0);		//飞利浦标准,16位数据长度
	I2S2_Init(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16b);			//飞利浦标准,主机发送,时钟低电平有效,16位帧长度 
	I2S2ext_Init(I2S_Standard_Phillips,I2S_Mode_SlaveRx,I2S_CPOL_Low,I2S_DataFormat_16b);		//飞利浦标准,从机接收,时钟低电平有效,16位帧长度	
	I2S2_SampleRate_Set(16000);	//设置采样率 
 	I2S2_TX_DMA_Init((u8*)&i2splaybuf[0],(u8*)&i2splaybuf[1],1); 		//配置TX DMA 
	DMA1_Stream4->CR&=~(1<<4);	//关闭传输完成中断(这里不用中断送数据) 
	I2S2ext_RX_DMA_Init(i2srecbuf1,i2srecbuf2,I2S_RX_DMA_BUF_SIZE/2); 	//配置RX DMA
  	i2s_rx_callback=rec_i2s_dma_rx_callback;//回调函数指wav_i2s_dma_callback
 	I2S_Play_Start();	//开始I2S数据发送(主机)
	I2S_Rec_Start(); 	//开始I2S数据接收(从机)
//	recoder_remindmsg_show(0);
}  
//进入PCM 放音模式 		  			    
//显示录音时间和码率
//tsec:秒钟数.

//提示信息
//mode:0,录音模式;1,放音模式
//通过时间获取文件名
//仅限在SD卡保存,不支持FLASH DISK保存
//组合成:形如"0:RECORDER/REC20120321210633.wav"的文件名

//WAV录音 
void wav_recorder(void)
{ 

	u8 rval=0; 
	OS_ERR err;
	i2srecbuf1=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S录音内存1申请
	i2srecbuf2=mymalloc(SRAMIN,I2S_RX_DMA_BUF_SIZE);//I2S录音内存2申请  
	if(!i2srecbuf1||!i2srecbuf2)rval=1; 	if(rval==0)		
	{
		recoder_enter_rec_mode();	//进入录音模式,此时耳机可以听到咪头采集到的音频   
		rec_sta|=0X80;	//开始录音	
 	  while(rval==0)
		{	
				OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//延时500ms
		}			 
	}    
//	myfree(SRAMIN,i2srecbuf1);	//释放内存
//	myfree(SRAMIN,i2srecbuf2);	//释放内存  
}



































