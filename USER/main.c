#include <string.h>
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "sram.h"
#include "malloc.h"
#include "ILI93xx.h"
#include "led.h"
#include "timer.h"
#include "touch.h"
#include "sdio_sdcard.h"
#include "GUI.h"
#include "ff.h"
#include "exfuns.h"
#include "w25qxx.h"
#include "includes.h"
#include "FramewinDLG.h"
#include "WM.h"
#include "DIALOG.h"
#include "pwm.h"
#include "include.h"
#include "wm8978.h"	 
#include "recorder.h"
/************************************************
 ALIENTEK ̽����STM32F407������ 
 STemWin GUIBuliderʹ��
 
 UCOSIII���������ȼ��û�������ʹ�ã�ALIENTEK
 ����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
 ���ȼ�0���жϷ������������� OS_IntQTask()
 ���ȼ�1��ʱ�ӽ������� OS_TickTask()
 ���ȼ�2����ʱ���� OS_TmrTask()
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com  
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

//�������ȼ�
#define START_TASK_PRIO				3
//�����ջ��С	
#define START_STK_SIZE 				1024
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);



//TOUCH����
//�����������ȼ�
#define TOUCH_TASK_PRIO				4
//�����ջ��С
#define TOUCH_STK_SIZE				128
//������ƿ�
OS_TCB TouchTaskTCB;
//�����ջ
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touch����
void touch_task(void *p_arg);

#define TASK_Q_NUM	10		//�������ڽ���Ϣ���еĳ���

//MsgManage����
//�����������ȼ�
#define MSGMANAGE_TASK_PRIO			5
//�����ջ��С
#define MSGMANAGE_STK_SIZE			1024
//������ƿ�
OS_TCB MsgManageTaskTCB;
//�����ջ
CPU_STK MSGMANAGE_TASK_STK[MSGMANAGE_STK_SIZE];
//msgManage_task����
void msgManage_task(void *p_arg);




//EMWIN����
//�����������ȼ�
#define EMWIN_TASK_PRIO			6
//�����ջ��С
#define EMWIN_STK_SIZE			2048
//������ƿ�
OS_TCB EmwinTaskTCB;
//�����ջ
CPU_STK EMWIN_TASK_STK[EMWIN_STK_SIZE];
//emwindemo_task����
void emwin_task(void *p_arg);



//LED����
//�����������ȼ�
#define LED_TASK_PRIO 				7
//�����ջ��С
#define LED_STK_SIZE				128
//������ƿ�
OS_TCB LedTaskTCB;
//�����ջ
CPU_STK LED_TASK_STK[LED_STK_SIZE];
//led����
void led_task(void *p_arg);


//motor����
//�����������ȼ�
#define MOTOR_TASK_PRIO 				8
//�����ջ��С
#define MOTOR_STK_SIZE				128
//������ƿ�
OS_TCB MotorTaskTCB;
//�����ջ
CPU_STK MOTOR_TASK_STK[MOTOR_STK_SIZE];
//motor����
void motor_task(void *p_arg);



//��������
//�����������ȼ�
#define USART_TASK_PRIO 				9
//�����ջ��С
#define USART_STK_SIZE				512
//������ƿ�
OS_TCB UsartTaskTCB;
//�����ջ
CPU_STK USART_TASK_STK[USART_STK_SIZE];
//usart����
void usart_task(void *p_arg);


//����������
//�����������ȼ�
#define ECHO_TASK_PRIO 				8
//�����ջ��С
#define ECHO_STK_SIZE				128
//������ƿ�
OS_TCB EchoTaskTCB;
//�����ջ
CPU_STK ECHO_TASK_STK[ECHO_STK_SIZE];
//echo����
void echo_task(void *p_arg);



msg_T msgGloba;
static char motor_flag = 1; 

int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init(168);       	//��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//�жϷ�������
	uart_init(115200);    	//���ڲ���������
	TFTLCD_Init();			//��ʼ��LCD
	W25QXX_Init();			//��ʼ��W25Q128
	LED_Init();   			//LED��ʼ��
	WM8978_Init();				//��ʼ��WM8978
	WM8978_HPvol_Set(40,40);	//������������
	WM8978_SPKvol_Set(0);		//������������
	FSMC_SRAM_Init(); 		//SRAM��ʼ��	
	mem_init(SRAMIN); 		//�ڲ�RAM��ʼ��
	mem_init(SRAMEX); 		//�ⲿRAM��ʼ��
	mem_init(SRAMCCM);		//CCM��ʼ��
	TIM14_PWM_Init(20000-1,84-1);//50hz Ϊ�˿��ƶ��
	
	exfuns_init();			//Ϊfatfs�ļ�ϵͳ�����ڴ�
	f_mount(fs[0],"0:",1);	//����SD��
	f_mount(fs[1],"1:",1);	//����FLASH

	TP_Init();				//��ʼ��������
	
	

	
	
	OSInit(&err);			//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���
	//������ʼ����
	OSTaskCreate(  (OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				         (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
	while(1);
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);//����CRCʱ��
	WM_SetCreateFlags(WM_CF_MEMDEV); //�������д��ڵĴ洢�豸
	GUI_Init();  			//STemWin��ʼ��
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	//STemWin ����	
	OSTaskCreate(  (OS_TCB*     )&EmwinTaskTCB,		
				         (CPU_CHAR*   )"Emwin task", 		
                 (OS_TASK_PTR )emwin_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )EMWIN_TASK_PRIO,     
                 (CPU_STK*    )&EMWIN_TASK_STK[0],	
                 (CPU_STK_SIZE)EMWIN_STK_SIZE/10,	
                 (CPU_STK_SIZE)EMWIN_STK_SIZE,		
                 (OS_MSG_QTY  )TASK_Q_NUM/2,			
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	//����������
	OSTaskCreate(  (OS_TCB*     )&TouchTaskTCB,		
				         (CPU_CHAR*   )"Touch task", 		
                 (OS_TASK_PTR )touch_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )TOUCH_TASK_PRIO,     
                 (CPU_STK*    )&TOUCH_TASK_STK[0],	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE/10,	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);			 
								 
								 
								 
								 
								 
  //MsgManage����
	OSTaskCreate(   (OS_TCB*     )&MsgManageTaskTCB,
				          (CPU_CHAR*   )"MsgManage task",
						      (OS_TASK_PTR )msgManage_task,
								  (void*       )0,	
								  (OS_PRIO	  )MSGMANAGE_TASK_PRIO,  
								  (CPU_STK*    )&MSGMANAGE_TASK_STK[0],
								  (CPU_STK_SIZE)MSGMANAGE_STK_SIZE/10,
									(CPU_STK_SIZE)MSGMANAGE_STK_SIZE,
									(OS_MSG_QTY  )TASK_Q_NUM,
									(OS_TICK	  )0,
									(void*       )0,		
								  (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                  (OS_ERR*     )&err);	
									
										 
	//LED����
	OSTaskCreate(  (OS_TCB*     )&LedTaskTCB,		
				         (CPU_CHAR*   )"Led task", 		
                 (OS_TASK_PTR )led_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )LED_TASK_PRIO,     
                 (CPU_STK*    )&LED_TASK_STK[0],	
                 (CPU_STK_SIZE)LED_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED_STK_SIZE,		
                 (OS_MSG_QTY  )TASK_Q_NUM/2,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);				 
								 
								 
  //Motor����
	OSTaskCreate(  (OS_TCB*     )&MotorTaskTCB,		
				         (CPU_CHAR*   )"Motor task", 		
                 (OS_TASK_PTR )motor_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )MOTOR_TASK_PRIO,     
                 (CPU_STK*    )&MOTOR_TASK_STK[0],	
                 (CPU_STK_SIZE)MOTOR_STK_SIZE/10,	
                 (CPU_STK_SIZE)MOTOR_STK_SIZE,		
                 (OS_MSG_QTY  )TASK_Q_NUM/2,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);		

								 
	//usart����
	OSTaskCreate(  (OS_TCB*     )&UsartTaskTCB,		
				         (CPU_CHAR*   )"Usart task", 		
                 (OS_TASK_PTR )usart_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )USART_TASK_PRIO,     
                 (CPU_STK*    )&USART_TASK_STK[0],	
                 (CPU_STK_SIZE)USART_STK_SIZE/10,	
                 (CPU_STK_SIZE)USART_STK_SIZE,		
                 (OS_MSG_QTY  )TASK_Q_NUM/2,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);				 							 							 		
	
		//ECHO����
	OSTaskCreate(  (OS_TCB*     )&EchoTaskTCB,		
				         (CPU_CHAR*   )"Echo task", 		
                 (OS_TASK_PTR )echo_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )ECHO_TASK_PRIO,     
                 (CPU_STK*    )&ECHO_TASK_STK[0],	
                 (CPU_STK_SIZE)ECHO_STK_SIZE/10,	
                 (CPU_STK_SIZE)ECHO_STK_SIZE,		
                 (OS_MSG_QTY  )TASK_Q_NUM/2,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);				 							 								
								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
}






//EMWIN����
void emwin_task(void *p_arg)
{
	msg_T *msg;
	OS_MSG_SIZE size;
	OS_ERR err; 
	
	unsigned row;
	WM_HWIN hItem;
	WM_MESSAGE message;
	
	
	//����Ƥ��
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX); 
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	MENU_SetDefaultSkin(MENU_SKIN_FLEX);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	SPINBOX_SetDefaultSkin(SPINBOX_SKIN_FLEX);
	
	
  hWinDialog = CreateFramewin();
	while(1)
	{
		msg=(msg_T *)OSTaskQPend((OS_TICK		)0,
							(OS_OPT		)OS_OPT_PEND_NON_BLOCKING,
							(OS_MSG_SIZE*	)&size,
							(CPU_TS*		)0,
							(OS_ERR*      )&err );
		if(msg != NULL ){
			
		  if(msg->what.action == ACT_CANCEL){
				
				//������Ϣ����ť
				hItem = WM_GetDialogItem(hWinDialog,ID_BUTTON_1);
				message.hWinSrc = hItem;
				message.hWin = 	hWinDialog;		
				message.MsgId = WM_MSGUSER;
				message.Data.v = WM_NOTIFICATION_RELEASED;
				
			}else if(msg->what.action == ACT_OK){
				//����ѡ��
				hItem = WM_GetDialogItem(hWinDialog,ID_LISTVIEW_0);

				row = LISTVIEW_GetSel(hItem);
				
				
				if(row == msg->what.food){
					message.hWinSrc = hItem;//���bug��viewlist��ǰ�Ѿ������У���ȡ����״̬��ͨ�����ڷ�������ѡ���״̬����Ϊ�в�������LISTVIEW_SetSel���ᴥ��change�¼���Ҫ�ֶ�����
					message.hWin = 	hWinDialog;		
					message.MsgId = WM_MSGUSER;
					message.Data.v = WM_NOTIFICATION_RELEASED;
					WM_SendMessage(WM_GetClientWindow(hWinDialog),&message);
				}else{
					row=msg->what.food;
					LISTVIEW_SetSel(hItem,row);//���listview��change��Ϣ
				}
				//������Ϣ����ť
				hItem = WM_GetDialogItem(hWinDialog,ID_BUTTON_0);
				message.hWinSrc = hItem;
				message.hWin = 	hWinDialog;		
				message.MsgId = WM_MSGUSER;
				message.Data.v = WM_NOTIFICATION_RELEASED;
				
			}	
			
			 //����WM_GetClientWindow Ҫ������Ϣ��framewin��client(�ͻ�������),���ô��ڵĻص��������ܽ�ȥ��hWinDialog��framewin�ľ������client���
			 WM_SendMessage(WM_GetClientWindow(hWinDialog),&message);
			
		}	
		GUI_Delay(1);
	}
}

//TOUCH����
void touch_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//��ʱ5ms
	}
}


const char food[5][FOOD_TOTAL_NUM]= {"����","����","����","�׷�","����"};




//msgManage_task����
void msgManage_task(void *p_arg)
{
  WM_HWIN hItem;
  unsigned int row;
	
	msg_T *msg;
	OS_MSG_SIZE size;
	OS_ERR err; 
	int lastAct=ACT_CANCEL+1;
	while(1){
		//������Ϣ
		msg=(msg_T *)OSTaskQPend((OS_TICK		)0,
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE*	)&size,
                      (CPU_TS*		)0,
                      (OS_ERR*      )&err );
		
		hItem = WM_GetDialogItem(hWinDialog,ID_LISTVIEW_0);
		row=msg->what.food;
		
	  if(msg->what.action == lastAct && msg->what.food == FOOD_NONE){
			sprintf(msg->what.info,"�Ѿ�ֹͣͶ�� %s,�����ظ�����",food[row]);
			
			OSTaskQPost((OS_TCB*	)&UsartTaskTCB,	//�����񴮿ڷ�����Ϣ
							  	(void*		)msg,
									(OS_MSG_SIZE)(sizeof(msg_T)),
									(OS_OPT		)OS_OPT_POST_FIFO,
									(OS_ERR*	)&err);			
			continue;
		}else if(msg->what.action == lastAct && msg->what.food == row){
			//����ִ�иö�����
			sprintf(msg->what.info,"�Ѿ���Ͷ�� %s,�����ظ�����",food[row]);
			
				
			OSTaskQPost((OS_TCB*	)&UsartTaskTCB,	//�����񴮿ڷ�����Ϣ
                    (void*		)msg,
                    (OS_MSG_SIZE)(sizeof(msg_T)),
                    (OS_OPT		)OS_OPT_POST_FIFO,
										(OS_ERR*	)&err);			
			continue;
		}else	if(msg->srcID != TASK_USART){//������Դ�Ĳ�ȥ���¶�������Ϊ������Ϣ��ȥִ�л�֪ͨbutton��buttonִ�н���������������,��Ȼbutton��������ʾ�����Ѿ�������
			
			
 
			 lastAct = msg->what.action;//���¶���
//			  
//			 if(msg->what.action == ACT_OK){
//				 sprintf(msg->what.info,"����Ͷ�� %s",food[row]);
//			 }else if(msg->what.action == ACT_CANCEL){
//				 sprintf(msg->what.info,"ֹͣͶ��%s",food[row]);
//			 }	 
//			 OSTaskQPost((OS_TCB*	)&UsartTaskTCB,	//�����񴮿ڷ�����Ϣ
//										(void*		)msg,
//										(OS_MSG_SIZE)(sizeof(msg_T)),
//										(OS_OPT		)OS_OPT_POST_FIFO,
//										(OS_ERR*	)&err);			
		}			
		if(msg->srcID == TASK_EMWIN){
			
			OSTaskQPost((OS_TCB*	)&LedTaskTCB,	//������Led������Ϣ
                    (void*		)msg,
                    (OS_MSG_SIZE)(sizeof(msg_T)),
                    (OS_OPT		)OS_OPT_POST_FIFO,
										(OS_ERR*	)&err);
	    OSTaskQPost((OS_TCB*	)&MotorTaskTCB,	//������motor������Ϣ
                    (void*		)msg,
                    (OS_MSG_SIZE)(sizeof(msg_T)),
                    (OS_OPT		)OS_OPT_POST_FIFO,
										(OS_ERR*	)&err);							
			
		}else if(msg->srcID == TASK_USART || msg->srcID == TASK_AUDIO){		
				OSTaskQPost((OS_TCB*	)&EmwinTaskTCB,	//������Led������Ϣ
                    (void*		)msg,
                    (OS_MSG_SIZE)(sizeof(msg_T)),
                    (OS_OPT		)OS_OPT_POST_FIFO,
										(OS_ERR*	)&err);

		}
		//OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //��ʱ1s
	}
	
}





//LED����
void led_task(void *p_arg)
{

	msg_T *msg;
	OS_MSG_SIZE size;
	OS_ERR err; 
	int i;	
	
	
	while(1)
	{
		msg=(msg_T *)OSTaskQPend((OS_TICK		)0,
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE*	)&size,
                      (CPU_TS*		)0,
                      (OS_ERR*      )&err );
		

		
		//0 ���� ���LED0 �ұ�LED1 
	  if(msg->what.food == FOOD_NONE){
			LED0 = 1;LED1 = 1;//00
		}
		else if(msg->what.food == FOOD_HAMBURGER){
			LED0 = 1;LED1 = 0;//01
		}else if(msg->what.food == FOOD_PIZZA){
			LED0 = 0;LED1 = 1 ;//10
		}	else if(msg->what.food == FOOD_ORANGE){
			LED0 = 0;LED1 = 0 ;//11
		}	else if(msg->what.food == FOOD_RICE){
			  LED0 = 1;LED1 = 1;
				for(i=6;i>0;i--){
					
					LED1 = !LED1;
					OSTimeDlyHMSM(0,0,0,250,OS_OPT_TIME_PERIODIC,&err);//��ʱ500ms
				}
		}	else if(msg->what.food == FOOD_EGG){
			  LED0 = 1;LED1 = 1;
				for(i=6;i>0;i--){
					
					LED0 = !LED0;
					OSTimeDlyHMSM(0,0,0,250,OS_OPT_TIME_PERIODIC,&err);//��ʱ500ms
				}
		}
		
	}
}



//motor����
void motor_task(void *p_arg)
{

	msg_T *msg;
	OS_MSG_SIZE size;
	OS_ERR err; 
	int i;	
	 TIM_SetCompare1(TIM14,19000);	//�ر�
	
		//TIM14_PWM_Init(20000-1,84-1);//50hz
	 while(1) //ʵ�ֱȽ�ֵ��0-300��������300���300-0�ݼ���ѭ��
	{
		msg=(msg_T *)OSTaskQPend((OS_TICK		)0,
										(OS_OPT		)OS_OPT_PEND_BLOCKING,
										(OS_MSG_SIZE*	)&size,
										(CPU_TS*		)0,
										(OS_ERR*      )&err );
		if(msg->what.action == ACT_OK && msg->what.food != FOOD_NONE)
			//TIM_SetCompare1(TIM14,18000);	//�޸ıȽ�ֵ���޸�ռ�ձ�
		 TIM_SetCompare1(TIM14,18000);	//�޸ıȽ�ֵ���޸�ռ�ձ�
	  else if(msg->what.action == ACT_CANCEL && msg->what.food == FOOD_NONE)
		 // TIM_SetCompare1(TIM14,19999);	//�޸ıȽ�ֵ���޸�ռ�ձ�
			
		 TIM_SetCompare1(TIM14,19000);	//�޸ıȽ�ֵ���޸�ռ�ձ�
	}
}











//USART����
unsigned int cmd_Parse(char* buf)
{
		int i;
		for(i=0;i<FOOD_TOTAL_NUM;i++)
		{
				if(!strcmp(food[i],buf))
					return i;
		}
		return FOOD_NONE;
}
//usart����
void usart_task(void *p_arg)
{
	
	msg_T *msg;
	OS_MSG_SIZE size;
	OS_ERR err; 

	u8 i;
	u8 len;	
	u16 times=0;  
  char rec_buf[20];

	unsigned int food_id = 0;

  while(1)
	{
			
		 msg=(msg_T *)OSTaskQPend((OS_TICK		)0,
							(OS_OPT		)OS_OPT_PEND_NON_BLOCKING,
							(OS_MSG_SIZE*	)&size,
							(CPU_TS*		)0,
							(OS_ERR*      )&err );
		 if(msg != NULL){
			 printf("%s",msg->what.info);
			 printf("\r\n\r\n");//���뻻��	
		 } 
				
			if(USART_RX_STA&0x8000)//�����յ���Ϣ
		 {					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			for(i=0;i<len;i++)
			{
				rec_buf[i]=USART_RX_BUF[i];
			}
			strncpy(rec_buf,(char *)USART_RX_BUF,len);
			rec_buf[len]='\0';
			
			
			
		
			food_id = cmd_Parse(rec_buf);
			if(food_id !=FOOD_NONE){
				
				//printf("%s ����Ͷ��",food[food_id]);
				msg=&msgGloba;
			  msg->srcID = TASK_USART;
				msg->what.food = food_id;
				msg->what.action = ACT_OK;
				OSTaskQPost((OS_TCB*	)&MsgManageTaskTCB,	//������Msgdis������Ϣ
											(void*		)msg,
											(OS_MSG_SIZE)(sizeof(msg_T)),
											(OS_OPT		)OS_OPT_POST_FIFO,
											(OS_ERR*	)&err);	
			}else if(!strcmp(rec_buf,"ֹͣ")){
				//printf("ֹͣͶ�ݣ�");
			  msg=&msgGloba;
			  msg->srcID = TASK_USART;
				msg->what.food = FOOD_NONE;
				msg->what.action = ACT_CANCEL;
				OSTaskQPost((OS_TCB*	)&MsgManageTaskTCB,	//������Msgdis������Ϣ
											(void*		)msg,
											(OS_MSG_SIZE)(sizeof(msg_T)),
											(OS_OPT		)OS_OPT_POST_FIFO,
											(OS_ERR*	)&err);	
			
			}
			else{
				printf("wrong code!!");
			}
			printf("\r\n\r\n");//���뻻��	
			USART_RX_STA=0;
		}else{
			times++;
			if(times%5000==0)
			{
				printf("\r\n����ι��ϵͳ\r\n");
			}
			if(times%200==0)printf("������ʳ��,�Իس�������\r\n");  
      OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//��ʱ10ms			
		}
		
	}
}

//ECHO����
void echo_task(void *p_arg)
{
//	OS_ERR err;
	while(1)
	{
		wav_recorder();
//		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//��ʱ500ms
		
	}
	
}