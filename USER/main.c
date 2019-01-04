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
 ALIENTEK 探索者STM32F407开发板 
 STemWin GUIBulider使用
 
 UCOSIII中以下优先级用户程序不能使用，ALIENTEK
 将这些优先级分配给了UCOSIII的5个系统内部任务
 优先级0：中断服务服务管理任务 OS_IntQTask()
 优先级1：时钟节拍任务 OS_TickTask()
 优先级2：定时任务 OS_TmrTask()
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com  
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//任务优先级
#define START_TASK_PRIO				3
//任务堆栈大小	
#define START_STK_SIZE 				1024
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);



//TOUCH任务
//设置任务优先级
#define TOUCH_TASK_PRIO				4
//任务堆栈大小
#define TOUCH_STK_SIZE				128
//任务控制块
OS_TCB TouchTaskTCB;
//任务堆栈
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touch任务
void touch_task(void *p_arg);

#define TASK_Q_NUM	10		//发任务内建消息队列的长度

//MsgManage任务
//设置任务优先级
#define MSGMANAGE_TASK_PRIO			5
//任务堆栈大小
#define MSGMANAGE_STK_SIZE			1024
//任务控制块
OS_TCB MsgManageTaskTCB;
//任务堆栈
CPU_STK MSGMANAGE_TASK_STK[MSGMANAGE_STK_SIZE];
//msgManage_task任务
void msgManage_task(void *p_arg);




//EMWIN任务
//设置任务优先级
#define EMWIN_TASK_PRIO			6
//任务堆栈大小
#define EMWIN_STK_SIZE			2048
//任务控制块
OS_TCB EmwinTaskTCB;
//任务堆栈
CPU_STK EMWIN_TASK_STK[EMWIN_STK_SIZE];
//emwindemo_task任务
void emwin_task(void *p_arg);



//LED任务
//设置任务优先级
#define LED_TASK_PRIO 				7
//任务堆栈大小
#define LED_STK_SIZE				128
//任务控制块
OS_TCB LedTaskTCB;
//任务堆栈
CPU_STK LED_TASK_STK[LED_STK_SIZE];
//led任务
void led_task(void *p_arg);


//motor任务
//设置任务优先级
#define MOTOR_TASK_PRIO 				8
//任务堆栈大小
#define MOTOR_STK_SIZE				128
//任务控制块
OS_TCB MotorTaskTCB;
//任务堆栈
CPU_STK MOTOR_TASK_STK[MOTOR_STK_SIZE];
//motor任务
void motor_task(void *p_arg);



//串口任务
//设置任务优先级
#define USART_TASK_PRIO 				9
//任务堆栈大小
#define USART_STK_SIZE				512
//任务控制块
OS_TCB UsartTaskTCB;
//任务堆栈
CPU_STK USART_TASK_STK[USART_STK_SIZE];
//usart任务
void usart_task(void *p_arg);


//捕获音任务
//设置任务优先级
#define ECHO_TASK_PRIO 				8
//任务堆栈大小
#define ECHO_STK_SIZE				128
//任务控制块
OS_TCB EchoTaskTCB;
//任务堆栈
CPU_STK ECHO_TASK_STK[ECHO_STK_SIZE];
//echo任务
void echo_task(void *p_arg);



msg_T msgGloba;
static char motor_flag = 1; 

int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init(168);       	//延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//中断分组配置
	uart_init(115200);    	//串口波特率设置
	TFTLCD_Init();			//初始化LCD
	W25QXX_Init();			//初始化W25Q128
	LED_Init();   			//LED初始化
	WM8978_Init();				//初始化WM8978
	WM8978_HPvol_Set(40,40);	//耳机音量设置
	WM8978_SPKvol_Set(0);		//喇叭音量设置
	FSMC_SRAM_Init(); 		//SRAM初始化	
	mem_init(SRAMIN); 		//内部RAM初始化
	mem_init(SRAMEX); 		//外部RAM初始化
	mem_init(SRAMCCM);		//CCM初始化
	TIM14_PWM_Init(20000-1,84-1);//50hz 为了控制舵机
	
	exfuns_init();			//为fatfs文件系统分配内存
	f_mount(fs[0],"0:",1);	//挂载SD卡
	f_mount(fs[1],"1:",1);	//挂载FLASH

	TP_Init();				//初始化触摸屏
	
	

	
	
	OSInit(&err);			//初始化UCOSIII
	OS_CRITICAL_ENTER();	//进入临界区
	//创建开始任务
	OSTaskCreate(  (OS_TCB 	* )&StartTaskTCB,		//任务控制块
				         (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
	while(1);
}

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);//开启CRC时钟
	WM_SetCreateFlags(WM_CF_MEMDEV); //启动所有窗口的存储设备
	GUI_Init();  			//STemWin初始化
	
	OS_CRITICAL_ENTER();	//进入临界区
	//STemWin 任务	
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
	//触摸屏任务
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
								 
								 
								 
								 
								 
  //MsgManage任务
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
									
										 
	//LED任务
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
								 
								 
  //Motor任务
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

								 
	//usart任务
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
	
		//ECHO任务
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
								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//退出临界区
}






//EMWIN任务
void emwin_task(void *p_arg)
{
	msg_T *msg;
	OS_MSG_SIZE size;
	OS_ERR err; 
	
	unsigned row;
	WM_HWIN hItem;
	WM_MESSAGE message;
	
	
	//更换皮肤
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
				
				//发送消息给按钮
				hItem = WM_GetDialogItem(hWinDialog,ID_BUTTON_1);
				message.hWinSrc = hItem;
				message.hWin = 	hWinDialog;		
				message.MsgId = WM_MSGUSER;
				message.Data.v = WM_NOTIFICATION_RELEASED;
				
			}else if(msg->what.action == ACT_OK){
				//设置选中
				hItem = WM_GetDialogItem(hWinDialog,ID_LISTVIEW_0);

				row = LISTVIEW_GetSel(hItem);
				
				
				if(row == msg->what.food){
					message.hWinSrc = hItem;//解决bug：viewlist当前已经在这行，是取消的状态，通过串口发送这行选择的状态，因为行不变所以LISTVIEW_SetSel不会触发change事件，要手动触发
					message.hWin = 	hWinDialog;		
					message.MsgId = WM_MSGUSER;
					message.Data.v = WM_NOTIFICATION_RELEASED;
					WM_SendMessage(WM_GetClientWindow(hWinDialog),&message);
				}else{
					row=msg->what.food;
					LISTVIEW_SetSel(hItem,row);//会给listview发change消息
				}
				//发送消息给按钮
				hItem = WM_GetDialogItem(hWinDialog,ID_BUTTON_0);
				message.hWinSrc = hItem;
				message.hWin = 	hWinDialog;		
				message.MsgId = WM_MSGUSER;
				message.Data.v = WM_NOTIFICATION_RELEASED;
				
			}	
			
			 //必须WM_GetClientWindow 要发送消息到framewin的client(客户窗口区),调该窗口的回调函数才能进去，hWinDialog是framewin的句柄，非client句柄
			 WM_SendMessage(WM_GetClientWindow(hWinDialog),&message);
			
		}	
		GUI_Delay(1);
	}
}

//TOUCH任务
void touch_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//延时5ms
	}
}


const char food[5][FOOD_TOTAL_NUM]= {"汉堡","披萨","橘子","米饭","鸡蛋"};




//msgManage_task任务
void msgManage_task(void *p_arg)
{
  WM_HWIN hItem;
  unsigned int row;
	
	msg_T *msg;
	OS_MSG_SIZE size;
	OS_ERR err; 
	int lastAct=ACT_CANCEL+1;
	while(1){
		//请求消息
		msg=(msg_T *)OSTaskQPend((OS_TICK		)0,
                      (OS_OPT		)OS_OPT_PEND_BLOCKING,
                      (OS_MSG_SIZE*	)&size,
                      (CPU_TS*		)0,
                      (OS_ERR*      )&err );
		
		hItem = WM_GetDialogItem(hWinDialog,ID_LISTVIEW_0);
		row=msg->what.food;
		
	  if(msg->what.action == lastAct && msg->what.food == FOOD_NONE){
			sprintf(msg->what.info,"已经停止投递 %s,无需重复动作",food[row]);
			
			OSTaskQPost((OS_TCB*	)&UsartTaskTCB,	//向任务串口发送消息
							  	(void*		)msg,
									(OS_MSG_SIZE)(sizeof(msg_T)),
									(OS_OPT		)OS_OPT_POST_FIFO,
									(OS_ERR*	)&err);			
			continue;
		}else if(msg->what.action == lastAct && msg->what.food == row){
			//正在执行该动作。
			sprintf(msg->what.info,"已经在投递 %s,无需重复动作",food[row]);
			
				
			OSTaskQPost((OS_TCB*	)&UsartTaskTCB,	//向任务串口发送消息
                    (void*		)msg,
                    (OS_MSG_SIZE)(sizeof(msg_T)),
                    (OS_OPT		)OS_OPT_POST_FIFO,
										(OS_ERR*	)&err);			
			continue;
		}else	if(msg->srcID != TASK_USART){//串口来源的不去更新动作，因为串口消息若去执行会通知button，button执行进来再做动作更新,不然button进来就显示动作已经做过了
			
			
 
			 lastAct = msg->what.action;//更新动作
//			  
//			 if(msg->what.action == ACT_OK){
//				 sprintf(msg->what.info,"正在投递 %s",food[row]);
//			 }else if(msg->what.action == ACT_CANCEL){
//				 sprintf(msg->what.info,"停止投递%s",food[row]);
//			 }	 
//			 OSTaskQPost((OS_TCB*	)&UsartTaskTCB,	//向任务串口发送消息
//										(void*		)msg,
//										(OS_MSG_SIZE)(sizeof(msg_T)),
//										(OS_OPT		)OS_OPT_POST_FIFO,
//										(OS_ERR*	)&err);			
		}			
		if(msg->srcID == TASK_EMWIN){
			
			OSTaskQPost((OS_TCB*	)&LedTaskTCB,	//向任务Led发送消息
                    (void*		)msg,
                    (OS_MSG_SIZE)(sizeof(msg_T)),
                    (OS_OPT		)OS_OPT_POST_FIFO,
										(OS_ERR*	)&err);
	    OSTaskQPost((OS_TCB*	)&MotorTaskTCB,	//向任务motor发送消息
                    (void*		)msg,
                    (OS_MSG_SIZE)(sizeof(msg_T)),
                    (OS_OPT		)OS_OPT_POST_FIFO,
										(OS_ERR*	)&err);							
			
		}else if(msg->srcID == TASK_USART || msg->srcID == TASK_AUDIO){		
				OSTaskQPost((OS_TCB*	)&EmwinTaskTCB,	//向任务Led发送消息
                    (void*		)msg,
                    (OS_MSG_SIZE)(sizeof(msg_T)),
                    (OS_OPT		)OS_OPT_POST_FIFO,
										(OS_ERR*	)&err);

		}
		//OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err); //延时1s
	}
	
}





//LED任务
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
		

		
		//0 点亮 左边LED0 右边LED1 
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
					OSTimeDlyHMSM(0,0,0,250,OS_OPT_TIME_PERIODIC,&err);//延时500ms
				}
		}	else if(msg->what.food == FOOD_EGG){
			  LED0 = 1;LED1 = 1;
				for(i=6;i>0;i--){
					
					LED0 = !LED0;
					OSTimeDlyHMSM(0,0,0,250,OS_OPT_TIME_PERIODIC,&err);//延时500ms
				}
		}
		
	}
}



//motor任务
void motor_task(void *p_arg)
{

	msg_T *msg;
	OS_MSG_SIZE size;
	OS_ERR err; 
	int i;	
	 TIM_SetCompare1(TIM14,19000);	//关闭
	
		//TIM14_PWM_Init(20000-1,84-1);//50hz
	 while(1) //实现比较值从0-300递增，到300后从300-0递减，循环
	{
		msg=(msg_T *)OSTaskQPend((OS_TICK		)0,
										(OS_OPT		)OS_OPT_PEND_BLOCKING,
										(OS_MSG_SIZE*	)&size,
										(CPU_TS*		)0,
										(OS_ERR*      )&err );
		if(msg->what.action == ACT_OK && msg->what.food != FOOD_NONE)
			//TIM_SetCompare1(TIM14,18000);	//修改比较值，修改占空比
		 TIM_SetCompare1(TIM14,18000);	//修改比较值，修改占空比
	  else if(msg->what.action == ACT_CANCEL && msg->what.food == FOOD_NONE)
		 // TIM_SetCompare1(TIM14,19999);	//修改比较值，修改占空比
			
		 TIM_SetCompare1(TIM14,19000);	//修改比较值，修改占空比
	}
}











//USART任务
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
//usart任务
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
			 printf("\r\n\r\n");//插入换行	
		 } 
				
			if(USART_RX_STA&0x8000)//串口收到消息
		 {					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			for(i=0;i<len;i++)
			{
				rec_buf[i]=USART_RX_BUF[i];
			}
			strncpy(rec_buf,(char *)USART_RX_BUF,len);
			rec_buf[len]='\0';
			
			
			
		
			food_id = cmd_Parse(rec_buf);
			if(food_id !=FOOD_NONE){
				
				//printf("%s 正在投递",food[food_id]);
				msg=&msgGloba;
			  msg->srcID = TASK_USART;
				msg->what.food = food_id;
				msg->what.action = ACT_OK;
				OSTaskQPost((OS_TCB*	)&MsgManageTaskTCB,	//向任务Msgdis发送消息
											(void*		)msg,
											(OS_MSG_SIZE)(sizeof(msg_T)),
											(OS_OPT		)OS_OPT_POST_FIFO,
											(OS_ERR*	)&err);	
			}else if(!strcmp(rec_buf,"停止")){
				//printf("停止投递！");
			  msg=&msgGloba;
			  msg->srcID = TASK_USART;
				msg->what.food = FOOD_NONE;
				msg->what.action = ACT_CANCEL;
				OSTaskQPost((OS_TCB*	)&MsgManageTaskTCB,	//向任务Msgdis发送消息
											(void*		)msg,
											(OS_MSG_SIZE)(sizeof(msg_T)),
											(OS_OPT		)OS_OPT_POST_FIFO,
											(OS_ERR*	)&err);	
			
			}
			else{
				printf("wrong code!!");
			}
			printf("\r\n\r\n");//插入换行	
			USART_RX_STA=0;
		}else{
			times++;
			if(times%5000==0)
			{
				printf("\r\n短信喂狗系统\r\n");
			}
			if(times%200==0)printf("请输入食物,以回车键结束\r\n");  
      OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//延时10ms			
		}
		
	}
}

//ECHO任务
void echo_task(void *p_arg)
{
//	OS_ERR err;
	while(1)
	{
		wav_recorder();
//		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//延时500ms
		
	}
	
}