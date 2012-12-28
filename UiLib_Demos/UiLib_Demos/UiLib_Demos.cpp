// UiLib_Demos.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "MainWnd.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	//EnableMemLeakCheck();
	CPaintManagerUI::SetInstance(hInstance);

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

#ifdef UILIB_D3D
	if( ::LoadLibrary(_T("d3d9.dll")) == NULL ) 
		::MessageBox(NULL, _T("���� d3d9.dll ʧ�ܣ�һЩ��Ч�����޷���ʾ��"), _T("��Ϣ��ʾ"),MB_OK|MB_ICONWARNING);
#endif

	CMainWnd* pFrame = new CMainWnd();
	if(pFrame == NULL)
		return 0;

	pFrame->Create(NULL,_T("UiLib Demos"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 900, 600);
	pFrame->CenterWindow();

	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();

	//_CrtDumpMemoryLeaks();
	return 0;
}