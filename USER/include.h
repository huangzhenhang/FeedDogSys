#ifndef _INCLUDE_H
#define _INCLUDE_H

#include "includes.h"




//MsgManage任务
//任务控制块
extern OS_TCB MsgManageTaskTCB;


//EMWIN任务
//任务控制块
extern OS_TCB EmwinTaskTCB;



//LED任务
//任务控制块
extern OS_TCB LedTaskTCB;






enum TASK_ID{
	TASK_EMWIN,
	TASK_LED,
	TASK_USART,
	TASK_MOTOR,
	TASK_AUDIO

};


enum FOOD{
	FOOD_HAMBURGER,
	FOOD_PIZZA,
	FOOD_ORANGE,
	FOOD_RICE,
	FOOD_EGG,
	FOOD_TOTAL_NUM,
	FOOD_NONE
};
enum ACTION{
	ACT_OK,
	ACT_CANCEL
};
typedef struct what_t{
	
	enum FOOD food;
	enum ACTION action;
	char info[60];
}what_T;

typedef struct msg_t{
	
	enum TASK_ID srcID;
	what_T what;
	
}msg_T;

extern msg_T msgGloba;
extern u8 *i2srecbuf1;
extern u8 *i2srecbuf2; 
extern u8 rec_sta;		//录音状态

#endif

