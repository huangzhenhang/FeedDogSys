#ifndef _FRAMEWINDLG_H
#define _FRAMEWINDLG_H
#include "sys.h"
#include "WM.h"
#include "ff.h"
#include "malloc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//STemWin GUIBuliderʹ��
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/4/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


//����������ص�RAM�е�BMPͼƬʱ��ͼƬÿ�е��ֽ���
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
