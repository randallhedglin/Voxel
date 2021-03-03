#define ID_APPICON NULL
#define ID_APPCURSOR NULL
#define ID_APPMENU NULL
#define ID_WINDOWTITLE "Voxel Editor"
#define ID_WINDOWCLASS "VOXEDIT_CLS"

#define SCRWD 800
#define SCRHT 600
#define SCRBP 16
#define NUMBACKBUFFERS 1
#define REC_FILE "voxel.rec"
#define DEF_FILE_PATH "c:\\windows\\msremote.sfs\\program files\\voxel editor\\"
//#define DEF_FILE_PATH "c:\\windows\\desktop\\randy's folder\\voxel editor\\"

#define CALL_MAIN_LOOP MainLoop
#define CALL_ENCODE	EncodeData

//#define ENCODE_DATA
#define MOUSE_TRACKING
//#define INIT_SOUND
//#define INIT_ZBUFFER
//#define Z_BUFFER

#include"basic2.h"
#include"voxel.h"
#include<commdlg.h>

BOOL Exit=FALSE;
COLORREF color1;
COLORREF color2;
COLORREF usercolors[16];
BOOL FileSaved;
char CurrentFile[256];
#define MODE_DRAW 1
BYTE Mode;
char FilterText[256];
BOOL FileNamed;
BOOL FileOpen;

BOOL NewFile(BOOL ShowNewDialogBox=TRUE);
BOOL SaveFile(void);
BOOL SaveAsFile(void);
BOOL OpenFile(void);

BOOL MainLoop(void)
{
	static BOOL Init=TRUE;

	if(Init)
	{
		ShowCursor(TRUE);
		strcpy(FilterText,"Voxel Files - ");
		strcat(FilterText,"*.VXL");
		strcat(FilterText,"");
		SetMenu(hAppWnd,LoadMenu(hAppInst,"MainMenu"));
		FileOpen=FALSE;
		NewFile(FALSE);
		Init=FALSE;
	}

	if(Exit)
		return(FALSE);
	
	return(TRUE);
}

#ifdef ENCODE_DATA
void EncodeData(void)
{
	OpenEncoder("voxel.dat");
	EncodeFont("cronos.ftd",DirectDraw);
	CloseEncoder();
}
#endif

LRESULT ProcessMenuCommand(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	CHOOSECOLOR cc;
	int btn;

	if(Msg==WM_COMMAND)
	{
		switch(wParam)
		{
			case(MENU_FILE_EXIT):
				Exit=TRUE;
				break;
			case(MENU_FILE_NEW):
				if(!FileSaved&&FileOpen)
				{
					btn=MessageBox(hAppWnd,
								   "Do you want to save the current file?",
								   "Query",
								   MB_YESNOCANCEL|
								   MB_ICONQUESTION);
					if(btn==IDYES)
					{
						SaveFile();
						NewFile();
					}
					if(btn==IDNO)
						NewFile();
				}
				else
					NewFile();
				break;
			case(MENU_FILE_OPEN):
				if(!FileSaved&&FileOpen)
				{
					btn=MessageBox(hAppWnd,
								   "Do you want to save the current file?",
								   "Query",
								   MB_YESNOCANCEL|
								   MB_ICONQUESTION);
					if(btn==IDYES)
					{
						SaveFile();
						OpenFile();
					}
					if(btn==IDNO)
						OpenFile();
				}
				else
					OpenFile();
				break;
			case(MENU_FILE_SAVE):
				if(FileOpen)
					SaveFile();
				else
					DisplayErrorMessage("Editor not open yet!","Error",MB_OK|MB_ICONSTOP,DirectDraw);
				break;
			case(MENU_FILE_SAVEAS):
				if(FileOpen)
					SaveAsFile();
				else
					DisplayErrorMessage("Editor not open yet!","Error",MB_OK|MB_ICONSTOP,DirectDraw);
				break;
			case(MENU_DRAW_CHOOSE_COLOR_1):
				if(FileOpen)
				{
					ZeroMemory((LPVOID)&cc,sizeof(cc));
					cc.lStructSize=sizeof(cc);
					cc.hwndOwner=hAppWnd;
					cc.hInstance=NULL;
					cc.rgbResult=color1;
					cc.lpCustColors=usercolors;
					cc.Flags=CC_ANYCOLOR|
							 CC_RGBINIT|
							 CC_FULLOPEN|
							 CC_SOLIDCOLOR;
					if(ChooseColor(&cc))
						color1=cc.rgbResult;
				}
				else
					DisplayErrorMessage("Editor not open yet!","Error",MB_OK|MB_ICONSTOP,DirectDraw);
				break;
			case(MENU_DRAW_CHOOSE_COLOR_2):
				if(FileOpen)
				{
					ZeroMemory((LPVOID)&cc,sizeof(cc));
					cc.lStructSize=sizeof(cc);
					cc.hwndOwner=hAppWnd;
					cc.hInstance=NULL;
					cc.rgbResult=color2;
					cc.lpCustColors=usercolors;
					cc.Flags=CC_ANYCOLOR|
							 CC_RGBINIT|
							 CC_FULLOPEN|
							 CC_SOLIDCOLOR;
					if(ChooseColor(&cc))
						color2=cc.rgbResult;
				}
				else
					DisplayErrorMessage("Editor not open yet!","Error",MB_OK|MB_ICONSTOP,DirectDraw);
				break;
			case(MENU_DRAW_DRAW):
				if(FileOpen)
					Mode=MODE_DRAW;
				else
					DisplayErrorMessage("Editor not open yet!","Error",MB_OK|MB_ICONSTOP,DirectDraw);
				break;
			default:
				return(DefWindowProc(hWnd,Msg,wParam,lParam));
				break;
		}
		return(0);
	}
	else
		return(DefWindowProc(hWnd,Msg,wParam,lParam));
}

BOOL NewFile(BOOL ShowNewDialogBox)
{
	int count;
	
	color1=RGB(255,255,255);
	color2=RGB(0,0,0);
	for(count=0;count<16;count++)
		usercolors[count]=RGB(0,0,0);
	FileSaved=FALSE;
	Mode=NULL;
	strcpy(CurrentFile,"untitled.vxl");
	FileNamed=FALSE;
	if(ShowNewDialogBox)
	{
		CDialog *NewDlg;
		DLGTEMPLATE dt;

		NewDlg=new CDialog;
		dt
		NewDlg->InitModalIndirect(&dt);


		FileOpen=TRUE;
	}
	return(TRUE);
}

BOOL SaveFile(void)
{
	if(!FileNamed)
		SaveAsFile();
	FileSaved=TRUE;
	return(TRUE);
}

BOOL SaveAsFile(void)
{
	OPENFILENAME ofn;

	ZeroMemory((LPVOID)&ofn,sizeof(ofn));
	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=hAppWnd;
	ofn.lpstrFilter=FilterText;
	ofn.nFilterIndex=1;
	ofn.lpstrFile=CurrentFile;
	ofn.nMaxFile=256;
	ofn.lpstrInitialDir="";
	ofn.Flags=OFN_EXPLORER|
			  OFN_HIDEREADONLY|
			  OFN_LONGNAMES|
			  OFN_NONETWORKBUTTON|
			  OFN_OVERWRITEPROMPT|
			  OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt="vxl";
	if(GetSaveFileName(&ofn))
		SaveFile();
	return(TRUE);
}

BOOL OpenFile(void)
{
	OPENFILENAME ofn;

	ZeroMemory((LPVOID)&ofn,sizeof(ofn));
	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=hAppWnd;
	ofn.lpstrFilter=FilterText;
	ofn.nFilterIndex=1;
	ofn.lpstrFile=CurrentFile;
	ofn.nMaxFile=256;
	ofn.lpstrInitialDir="";
	ofn.Flags=OFN_EXPLORER|
			  OFN_FILEMUSTEXIST|
			  OFN_HIDEREADONLY|
			  OFN_LONGNAMES|
			  OFN_NONETWORKBUTTON|
			  OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt="vxl";
	if(GetOpenFileName(&ofn))
	{
		NewFile();
		FileSaved=TRUE;
		FileNamed=TRUE;
		FileOpen=TRUE;
	}
	return(TRUE);
}
