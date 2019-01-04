/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.30                          *
*        Compiled Jul  1 2015, 10:50:32                              *
*        (c) 2015 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
// USER END

#include "DIALOG.h"
#include "FramewinDLG.h"
#include "include.h"

#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif


/*********************************************************************
*
*       Defines
*
**********************************************************************
*/


// USER START (Optionally insert additional defines)
// USER END



// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 800, 480, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 90, 300, 150, 50, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 330, 300, 150, 50, 0, 0x0, 0 },
  { IMAGE_CreateIndirect, "Image", ID_IMAGE_0, 582, 67, 152, 126, 0, 0, 0 },//��ʽ��
  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 80, 65, 450, 180, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetImageById
*/


// USER START (Optionally insert additional static code)


static char bmpBuffer[BMPPERLINESIZE];
static int BmpGetData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) 
{
	static int readaddress=0;
	FIL * phFile;
	UINT NumBytesRead;
	#if SYSTEM_SUPPORT_OS
		CPU_SR_ALLOC();
	#endif
	
	phFile = (FIL *)p;
	
	if (NumBytesReq > sizeof(bmpBuffer)) 
	{
		NumBytesReq = sizeof(bmpBuffer);
	}

	//�ƶ�ָ�뵽Ӧ�ö�ȡ��λ��
	if(Off == 1) readaddress = 0;
	else readaddress=Off;
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_ENTER();	//�ٽ���
	#endif
	f_lseek(phFile,readaddress); 
	
	//��ȡ���ݵ���������
	f_read(phFile,bmpBuffer,NumBytesReq,&NumBytesRead);
	#if SYSTEM_SUPPORT_OS
		OS_CRITICAL_EXIT();	//�˳��ٽ��� 
	#endif
	*ppData = (U8 *)bmpBuffer;
	return NumBytesRead;//���ض�ȡ�����ֽ���
}



// USER END

/*********************************************************************
*
*       _cbDialog
*/
//static IMAGE_Handle SET_hImage;
static FIL bmp;

static void _cbDialog(WM_MESSAGE * pMsg) {
//  const void * pData;
  WM_HWIN      hItem;
	WM_HWIN      hWin; 
//  U32          FileSize;
  int          NCode;
  int          Id;
  // USER START (Optionally insert additional variables)
	
	WM_MESSAGE message;


	HEADER_Handle hHeader;
	unsigned int row;
	msg_T* msg;
//	OS_MSG_SIZE size;
	OS_ERR err; 
	
	//_msg =(msg_T *)mymalloc(SRAMIN,sizeof(msg_T));	//��Ϣ�����Ǿֲ��ģ�
   
	msg=&msgGloba;
	
  // USER END

  switch (pMsg->MsgId) {
		
		case WM_PAINT:
			break;
		case WM_INIT_DIALOG:
			//
			// Initialization of 'Framewin'
			//
			hItem = pMsg->hWin;
			FRAMEWIN_SetFont(hItem, GUI_FONT_32B_ASCII);
			FRAMEWIN_SetText(hItem, "System");
			FRAMEWIN_SetTitleHeight(hItem, 40);
			FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			FRAMEWIN_SetTextColor(hItem, 0x000000FF);
			//
			// Initialization of 'Button'
			//
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
			BUTTON_SetText(hItem, "ok");
			BUTTON_SetFont(hItem, GUI_FONT_32_ASCII);
			//
			// Initialization of 'Button'
			//
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
			BUTTON_SetText(hItem, "cancel");
			BUTTON_SetFont(hItem, GUI_FONT_32_ASCII);
			
			
			
			
			//
			// Initialization of 'Image'
			//
			hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
		 	f_open(&bmp,"/PICTURE/BMP/KFC.bmp",FA_READ);  
			IMAGE_SetBMPEx(hItem,BmpGetData,(void *)&bmp);  
			GUI_Exec();//����ȥ��Ⱦһ�Σ����ڹر��ļ�ǰȥ��Ⱦ����Ȼ������
			//f_close(&bmp); //�򿪺��һ����Ⱦ�Ḳ�ǵ�֮ǰ��ʾ�ġ�ֱ��ϵͳȥ��Ⱦ
			
			
			
			
			
			//
			// Initialization of 'Listview'
			//

			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
			LISTVIEW_AddColumn(hItem, 70, "num", GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(hItem, 230, "food", GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(hItem, 150, "price", GUI_TA_HCENTER | GUI_TA_VCENTER);
			
			LISTVIEW_SetGridVis(hItem, 1);
			//LISTVIEW_SetAutoScrollV(hItem, 1);//�Զ����ӻ�����
			//LISTVIEW_SetAutoScrollH(hItem, 0);
			LISTVIEW_SetHeaderHeight(hItem, 50);
			hHeader = LISTVIEW_GetHeader(hItem);
			HEADER_SetFont(hHeader, GUI_FONT_32B_ASCII);
			
			
			LISTVIEW_SetRowHeight(hItem, 35);
			LISTVIEW_SetFont(hItem, GUI_FONT_24_ASCII);
			
			//LISTVIEW_SetItemBkColor(hItem, 0, 0, LISTVIEW_CI_UNSEL, 0x00FFFF00);
			
			//SCROLLBAR_CreateAttached(hItem, 0);//��LISTVIEW����ˮƽ������
			//hWin = WM_GetScrollbarH(hItem);
			//SCROLLBAR_SetWidth(hWin, 18);
			//SCROLLBAR_SetThumbSizeMin(18);//�����Ͽ���С����
			SCROLLBAR_CreateAttached(hItem, SCROLLBAR_CF_VERTICAL);//��ֱ������
			hWin = WM_GetScrollbarV(hItem);
			SCROLLBAR_SetWidth(hWin, 18);//���ù���������


			LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_UNSEL,GUI_CYAN);
			LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SEL,GUI_RED);
			LISTVIEW_SetBkColor(hItem, LISTVIEW_CI_SELFOCUS,GUI_BLUE);
			
			LISTVIEW_AddRow(hItem, NULL);
			LISTVIEW_SetItemText(hItem, 0, 0, "1");
			LISTVIEW_SetItemText(hItem, 1, 0, "hamburger");
			LISTVIEW_SetItemText(hItem, 2, 0, "$1.00");
			
			LISTVIEW_AddRow(hItem, NULL);
			LISTVIEW_SetItemText(hItem, 0, 1, "2");
			LISTVIEW_SetItemText(hItem, 1, 1, "pizza");
			LISTVIEW_SetItemText(hItem, 2, 1, "$2.00");
			
			LISTVIEW_AddRow(hItem, NULL);
			LISTVIEW_SetItemText(hItem, 0, 2, "3");
			LISTVIEW_SetItemText(hItem, 1, 2, "orange");
			LISTVIEW_SetItemText(hItem, 2, 2, "$3.00");
			
			LISTVIEW_AddRow(hItem, NULL);
			LISTVIEW_SetItemText(hItem, 0, 3, "4");
			LISTVIEW_SetItemText(hItem, 1, 3, "rice");
			LISTVIEW_SetItemText(hItem, 2, 3, "$4.78");
			
			LISTVIEW_AddRow(hItem, NULL);
			LISTVIEW_SetItemText(hItem, 0, 4, "5");
			LISTVIEW_SetItemText(hItem, 1, 4, "egg");
			LISTVIEW_SetItemText(hItem, 2, 4, "$5.21");
		
		
			break;
			
		case WM_MSGUSER:
//			Id    = WM_GetId(pMsg->hWinSrc);
//			NCode = pMsg->Data.v;
//		  message.hWinSrc =pMsg->hWinSrc;
//      message.hWin =pMsg->hWin;		
//			message.MsgId = pMsg->MsgId;
//		  message.Data.v = pMsg->Data.v;
//		  break;
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
			switch(Id) {
			case ID_BUTTON_0: // Notifications sent by 'Button' ok
				switch(NCode) {
				case WM_NOTIFICATION_CLICKED:
					break;
				case WM_NOTIFICATION_RELEASED:
					// USER START (Optionally insert code for reacting on notification message)
				    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
						row=LISTVIEW_GetSel(hItem);
				    if(row >= FOOD_NONE)
							break;
				    msg->srcID = TASK_EMWIN;
				    msg->what.food = row;
				    msg->what.action = ACT_OK;
				
						OSTaskQPost((OS_TCB*	)&MsgManageTaskTCB,	//������MsgMange������Ϣ
												(void*		)msg,
												(OS_MSG_SIZE)(sizeof(msg_T)),
												(OS_OPT		)OS_OPT_POST_FIFO,
												(OS_ERR*	)&err);	
										
						hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
						row=LISTVIEW_GetSel(hItem);
						LISTVIEW_SetItemBkColor(hItem,0,row,LISTVIEW_CI_SEL,GUI_RED);
						LISTVIEW_SetItemBkColor(hItem,1,row,LISTVIEW_CI_SEL,GUI_RED);	
						LISTVIEW_SetItemBkColor(hItem,2,row,LISTVIEW_CI_SEL,GUI_RED);							

					  break;
				}
				break;
			case ID_BUTTON_1: // Notifications sent by 'Button' cancle
				hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
				switch(NCode) {
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
					   
//							f_open(&bmp,"0:/PICTURE/BMP/KFC.bmp",FA_READ);  
//							IMAGE_SetBMPEx(hItem,BmpGetData,(void *)&bmp); 
//							GUI_Exec();//����ȥ��Ⱦһ�Σ����ڹر��ļ�ǰȥ��Ⱦ����Ȼ������
//							f_close(&bmp);			

						
						  hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
						  row=LISTVIEW_GetSel(hItem);
						  if(row >= FOOD_NONE)
							   break;
							msg->srcID = TASK_EMWIN;
							msg->what.food = FOOD_NONE;
              msg->what.action = ACT_CANCEL;

							OSTaskQPost((OS_TCB*	)&MsgManageTaskTCB,	//������Msgdis������Ϣ
												(void*		)msg,
												(OS_MSG_SIZE)(sizeof(msg_T)),
												(OS_OPT		)OS_OPT_POST_FIFO,
												(OS_ERR*	)&err);	
							
							hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
							row=LISTVIEW_GetSel(hItem);
							LISTVIEW_SetItemBkColor(hItem,0,row,LISTVIEW_CI_SEL,GUI_BLUE);
							LISTVIEW_SetItemBkColor(hItem,1,row,LISTVIEW_CI_SEL,GUI_BLUE);	
							LISTVIEW_SetItemBkColor(hItem,2,row,LISTVIEW_CI_SEL,GUI_BLUE);												
							break;
				
				}
				break;
			case ID_LISTVIEW_0: // Notifications sent by 'Listview'
				switch(NCode) {
				case WM_NOTIFICATION_CLICKED:
					break;
				case WM_NOTIFICATION_RELEASED:

			
			
				case WM_NOTIFICATION_SEL_CHANGED:
					
				  hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
				  row = LISTVIEW_GetSel(hItem);
			  
				
				  hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
				//LISTVIEW_GetSelUnsorted
					switch(row){
						case 0:
							f_open(&bmp,"0:/PICTURE/BMP/hamburger.bmp",FA_READ);  
							break;
						case 1:
								f_open(&bmp,"0:/PICTURE/BMP/pizza.bmp",FA_READ); 
							break;
						
						case 2:
							f_open(&bmp,"0:/PICTURE/BMP/orange.bmp",FA_READ); 
							break;
						
						case 3:
							f_open(&bmp,"0:/PICTURE/BMP/rice.bmp",FA_READ); 
							break;
						
						case 4:
							f_open(&bmp,"0:/PICTURE/BMP/egg.bmp",FA_READ); 
							break;
					}
				 
					IMAGE_SetBMPEx(hItem,BmpGetData,(void *)&bmp); 
					GUI_Exec();//����ȥ��Ⱦһ�Σ����ڹر��ļ�ǰȥ��Ⱦ����Ȼ������
					f_close(&bmp);					
					break;
		
				}
				break;
			}
			break;

		default:
			break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateFramewin
*/

//WM_HWIN CreateFramewin(void);
WM_HWIN hWinDialog;
WM_HWIN CreateFramewin(void) {


  hWinDialog = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
	
	
	//��ʽһ��ֱ�Ӵ�����Ҳ����_aDialogCreate���б�����
	/*SET_hImage = IMAGE_CreateEx(582, 150, 125, 125,hWin,WM_CF_SHOW,IMAGE_CF_TILE,ID_IMAGE_0);//ע��Ҫָ��parent
	f_open(&bmp,"/PICTURE/BMP/KFC.bmp",FA_READ);  
		
	IMAGE_SetBMPEx(SET_hImage,BmpGetData,(void *)&bmp);  
	GUI_Exec();//����ȥ��Ⱦһ�Σ����ڹر��ļ�ǰȥ��Ⱦ����Ȼ������
	f_close(&bmp); 
	*/	
  return hWinDialog;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/