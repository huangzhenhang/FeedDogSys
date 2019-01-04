#ifndef _FRAMEWINDLG_H
#define _FRAMEWINDLG_H
#include "sys.h"
#include "WM.h"
#include "ff.h"
#include "malloc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//STemWin GUIBulider使用
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/4/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


//绘制无需加载到RAM中的BMP图片时，图片每行的字节数
#define BMPPERLINESIZE	2*1024		

#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)
#define ID_IMAGE_0 (GUI_ID_USER + 0x05)
#define ID_LISTVIEW_0 (GUI_ID_USER + 0x10)

#define ID_IMAGE_0_IMAGE_0 0x00

#define WM_MSGUSER (WM_USER+1)

extern WM_HWIN hWinDialog;
WM_HWIN CreateFramewin(void);

#endif
