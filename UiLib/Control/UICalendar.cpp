#include "StdAfx.h"
#include "UICalendar.h"

#include <time.h>
using namespace std;

namespace UiLib
{
	/************************************************************************/
	/* ��������                                                             */
	/************************************************************************/
#ifndef UICalendarDlg_h__
#define UICalendarDlg_h__

	class CCalendarDlg : public CWindowWnd,public INotifyUI,public IMessageFilterUI
	{
	public:
		CCalendarDlg(CControlUI* _pControl);
		~CCalendarDlg();
	public:
		CDuiString GetSkinFile();
		LPCTSTR GetWindowClassName() const;
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled );
		LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void Init();
		void Notify(TNotifyUI& msg);
		void OnFinalMessage(HWND /*hWnd*/);
		LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	private:
		CCalendarUI*	pCalendar;
		CControlUI*		pCControl;
		CDuiString		ZipSkinPath;
		CPaintManagerUI pm;
	};

	CCalendarDlg::CCalendarDlg(CControlUI* _pControl)
	{
		ASSERT(_pControl);
		pCalendar	= NULL;
		pCControl	= _pControl;

		CPoint point;
		::ClientToScreen(pCControl->GetManager()->GetPaintWindow(),&point);

		RECT rcControl	= pCControl->GetPos();
		RECT rcDlg;
		rcDlg.left				= (point.x+rcControl.right)-220;
		rcDlg.top				= point.y+rcControl.bottom;
		rcDlg.right				= (point.x+rcControl.right);
		rcDlg.bottom			= point.y+168;

		CWindowWnd::Create(NULL,NULL,WS_POPUP, 0L,rcDlg.left,rcDlg.top,220,168);
		SetWindowPos(m_hWnd,HWND_TOP,rcDlg.left,rcDlg.top,220,168,SWP_SHOWWINDOW);
	}

	CCalendarDlg::~CCalendarDlg()
	{

	}

	//************************************
	// Method:    GetWindowClassName
	// FullName:  CCalendarDlg::GetWindowClassName
	// Access:    public 
	// Returns:   LPCTSTR
	// Qualifier: const
	// Note:	  
	//************************************
	LPCTSTR CCalendarDlg::GetWindowClassName() const
	{
		try
		{
			return _T("CCalendarDlg");
		}
		catch (...)
		{
			throw "CCalendarDlg::GetWindowClassName";
		}
	}
	
	//************************************
	// Method:    OnCreate
	// FullName:  CCalendarDlg::OnCreate
	// Access:    public 
	// Returns:   LRESULT
	// Qualifier:
	// Parameter: UINT uMsg
	// Parameter: WPARAM wParam
	// Parameter: LPARAM lParam
	// Parameter: BOOL & bHandled
	// Note:	  
	//************************************
	LRESULT CCalendarDlg::OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		try
		{
			LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
			styleValue &= ~WS_CAPTION;
			::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			RECT rcClient;
			::GetClientRect(*this, &rcClient);
			::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left,rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

			pm.Init(m_hWnd);
			pm.AddPreMessageFilter(this);

			CDialogBuilder builder;

			if(!GetSkinFile().IsEmpty())
				pm.SetResourcePath(GetSkinFile());

			CDuiString nStrRoot;
			nStrRoot.Format(_T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n<Window size=\"220,168\" bktrans=\"false\"><Calendar name=\"CalendarDlg\" /></Window>"));

			CControlUI* pRoot = builder.Create(nStrRoot.GetData(), (UINT)0, NULL, &pm);
			pm.AttachDialog(pRoot);
			pm.AddNotifier(this);

			Init();
			return 0;
		}
		catch (...)
		{
			throw "CCalendarDlg::OnCreate";
		}
	}

	//************************************
	// Method:    MessageHandler
	// FullName:  UiLib::CCalendarDlg::MessageHandler
	// Access:    public 
	// Returns:   LRESULT
	// Qualifier:
	// Parameter: UINT uMsg
	// Parameter: WPARAM wParam
	// Parameter: LPARAM lParam
	// Parameter: bool & bHandled
	//************************************
	LRESULT CCalendarDlg::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled )
	{
		try
		{
			if (uMsg == WM_KEYDOWN)
			{
				switch (wParam)
				{
				case VK_RETURN:
				case VK_ESCAPE:
					return ResponseDefaultKeyEvent(wParam);
				default:
					break;
				}
			}
			return FALSE;
		}
		catch(...)
		{
			throw "IWindowBase::MessageHandler";
		}
	}

	//************************************
	// Method:    ResponseDefaultKeyEvent
	// FullName:  UiLib::CCalendarDlg::ResponseDefaultKeyEvent
	// Access:    public 
	// Returns:   LRESULT
	// Qualifier:
	// Parameter: WPARAM wParam
	//************************************
	LRESULT CCalendarDlg::ResponseDefaultKeyEvent(WPARAM wParam)
	{
		if (wParam == VK_RETURN)
		{
			return FALSE;
		}
		else if (wParam == VK_ESCAPE)
		{
			Close();
			return TRUE;
		}

		return FALSE;
	}

	//************************************
	// Method:    HandleMessage
	// FullName:  CCalendarDlg::HandleMessage
	// Access:    public 
	// Returns:   LRESULT
	// Qualifier:
	// Parameter: UINT uMsg
	// Parameter: WPARAM wParam
	// Parameter: LPARAM lParam
	// Note:	  
	//************************************
	LRESULT CCalendarDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch (uMsg)
		{
		case WM_CREATE:         lRes = OnCreate(uMsg, wParam, lParam, bHandled); break; 
		case WM_NCHITTEST:      lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO:  lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_SIZE:           lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_SYSCOMMAND:     lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
		case WM_KILLFOCUS:
			{
				ShowWindow(SW_HIDE);
				Close(IDOK);
				return S_OK;
			}
		default:                bHandled = FALSE; break;
		}
		if (bHandled) return lRes;
		
		if (pm.MessageHandler(uMsg, wParam, lParam, lRes))
			return lRes;

		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	void CCalendarDlg::OnFinalMessage(HWND /*hWnd*/)
	{
		pm.RemovePreMessageFilter(this);
		pm.RemoveNotifier(this);
		pm.ReapObjects(pm.GetRoot());
	}

	LRESULT CCalendarDlg::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		if( !::IsZoomed(*this) ) {
			RECT rcSizeBox = pm.GetSizeBox();
			if( pt.y < rcClient.top + rcSizeBox.top ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
				return HTTOP;
			}
			else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
				return HTBOTTOM;
			}
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
		}

		RECT rcCaption = pm.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
				CControlUI* pControl = static_cast<CControlUI*>(pm.FindControl(pt));
				if( pControl && _tcsicmp(pControl->GetClass(), _T("ButtonUI")) != 0 &&
					_tcsicmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
					_tcsicmp(pControl->GetClass(), _T("TextUI")) != 0 )
					return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT CCalendarDlg::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

		LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
		lpMMI->ptMaxPosition.x   = rcWork.left;
		lpMMI->ptMaxPosition.y   = rcWork.top;
		lpMMI->ptMaxSize.x       = rcWork.right;
		lpMMI->ptMaxSize.y       = rcWork.bottom;

		bHandled = FALSE;
		return 0;
	}

	LRESULT CCalendarDlg::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = pm.GetRoundCorner();

		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
			CDuiRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT CCalendarDlg::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam == SC_CLOSE)
		{
			bHandled = TRUE;
			return 1;
		}

		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	//************************************
	// Method:    Init
	// FullName:  CCalendarDlg::Init
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Note:	  
	//************************************
	void CCalendarDlg::Init()
	{
		try
		{
			pCalendar	= static_cast<CCalendarUI*>(pm.FindControl(_T("CalendarDlg")));

			if(pCalendar)
			{
				CButtonUI* pControl = static_cast<CButtonUI*>(pCControl);
				if(pControl->GetCalendarStyle())
					pCalendar->AnyCalendarStyle(pControl->GetCalendarStyle());
				if(pControl->GetCalendarProfile())
					pCalendar->ApplyAttributeList(pControl->GetCalendarProfile());
			}
		}
		catch (...)
		{
			throw "CCalendarDlg::Init";
		}
	}

	//************************************
	// Method:    Notify
	// FullName:  CCalendarDlg::Notify
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: TNotifyUI & msg
	// Note:	  
	//************************************
	void CCalendarDlg::Notify( TNotifyUI& msg )
	{
		try
		{
			if(msg.sType == _T("OnSelcetDay") || msg.sType == _T("OnToday"))
			{
				if(!pCalendar)
					return;
				if(pCControl)
					pCControl->SetText(pCalendar->GetCurSelDateTime());
				Close(IDOK);
			}
		}
		catch (...)
		{
			throw "CCalendarDlg::Notify";
		}
	}

	//************************************
	// Method:    GetSkinFile
	// FullName:  CCalendarDlg::GetSkinFile
	// Access:    public 
	// Returns:   UiLib::CDuiString
	// Qualifier:
	// Note:	  
	//************************************
	CDuiString CCalendarDlg::GetSkinFile()
	{
		try
		{
			if(pCControl)
				return pCControl->GetManager()->GetResourcePath();
			return _T(".\\");
		}
		catch (...)
		{
			throw "CCalendarDlg::GetSkinFile";
		}
	}
#endif // UICalendarDlg_h__

	CCalendarUI::CCalendarUI(void)
	{
		//��ʼ����ǰ����
		time_t now;	
		time(&now);
		pCurDateTime = localtime(&now);
		pCurDateTime->tm_year	= pCurDateTime->tm_year-100+2000;
		pCurDateTime->tm_mon	= pCurDateTime->tm_mon+1;
		mToday					= pCurDateTime->tm_mday;

		CalDateTime(pCurDateTime->tm_year,pCurDateTime->tm_mon,mToday);

		pDateTimeBtn		= NULL;
		pToDayBtn			= NULL;
		pICalendar			= this;
		pEnabledYearSel		= true;
		pEnabledMoothSel	= true;

		//��ʼ��ȱʡ��ʽ
		m_DefaultStyle.nMainTitleBkColor	= _T("#FFC9C9C9");
		m_DefaultStyle.nSubTitleBkColor		= _T("#FFD9D9D9");
		m_DefaultStyle.nWeekIntervalColorA	= _T("#FFF9F9F9");
		m_DefaultStyle.nWeekIntervalColorB	= _T("#EEEEEEEE");
		m_DefaultStyle.nWeekendColorA		= _T("#DFD1D1D1");
		m_DefaultStyle.nWeekendColorB		= _T("#DFCCCCCC");
		m_DefaultStyle.nDayHotColor			= _T("#FFE1E1E1");
		m_DefaultStyle.nDaySelectColor		= _T("#FFE1E1E1");
		m_DefaultStyle.nNoCurMonthDayColor	= _T("#FFD0D0D0");
		m_DefaultStyle.nStatusBkColor		= _T("#FFC9C9C9");
		
		mSubTitleString.nSundayStr			= _T("��");
		mSubTitleString.nMondayStr			= _T("һ");
		mSubTitleString.nTuesdayStr			= _T("��");
		mSubTitleString.nWednesdayStr		= _T("��");
		mSubTitleString.nThursdayStr		= _T("��");
		mSubTitleString.nFridayStr			= _T("��");
		mSubTitleString.nSaturdayStr		= _T("��");
		mSubTitleString.nToDayString		= _T("����");

		AddSubTitleString(_T("Default"),mSubTitleString);

		//��ʼ������������������Ԫ��
		pMainTitleHoriz	= new CHorizontalLayoutUI();
		pLastYearBtn	= new CButtonUI();
		pMoothSelectBtn	= new CButtonUI();
		pNextYearBtn	= new CButtonUI();
		pLastYearBtn->SetFixedWidth(60);
		pNextYearBtn->SetFixedWidth(60);
		pMainTitleHoriz->SetFixedHeight(22);
		pMoothSelectBtn->SetFixedWidth(80);
		pLastYearBtn->OnNotify	+= UiLib::MakeDelegate(this,&CCalendarUI::OnLastYear);
		pMoothSelectBtn->OnNotify	+= UiLib::MakeDelegate(this,&CCalendarUI::OnMoothSelect);
		pNextYearBtn->OnNotify	+= UiLib::MakeDelegate(this,&CCalendarUI::OnNextYear);

 		pMainTitleHoriz->Add(pLastYearBtn);
		pMainTitleHoriz->Add(new CControlUI());
		pMainTitleHoriz->Add(pMoothSelectBtn);
		pMainTitleHoriz->Add(new CControlUI());
		pMainTitleHoriz->Add(pNextYearBtn);

		//��ʼ������������������Ԫ��
		pSubTitleHoriz	= new CHorizontalLayoutUI();

		for(int nWeekIndex = 0;nWeekIndex < 7;nWeekIndex++)
		{
			CControlUI* pWeek = pICalendar->CreateWeekPanel(pSubTitleHoriz,nWeekIndex);
			if(!pWeek)
				pWeek = this->CreateWeekPanel(pSubTitleHoriz,nWeekIndex);
			InitWeekPanel(pWeek,nWeekIndex);

			if(pWeek)
				pSubTitleHoriz->Add(pWeek);
		}

		//��ʼ���·�ѡ��������Ԫ��
		CVerticalLayoutUI* pMoothSubPanelA = new CVerticalLayoutUI();
		CVerticalLayoutUI* pMoothSubPanelB = new CVerticalLayoutUI();
		pMoothPanelHorz	= new CHorizontalLayoutUI();
		pMoothPanelHorz->SetFloat(true);
		pMoothPanelHorz->Add(pMoothSubPanelA);
		pMoothPanelHorz->Add(pMoothSubPanelB);
		pMoothPanelHorz->SetFixedHeight(120);
		pMoothPanelHorz->SetMaxWidth(80);
		
		WCHAR TmpDef[64];
		for(int nMoothItem = 1;nMoothItem < 13;nMoothItem++)
		{
			wsprintf(TmpDef,_T("CalendMoothGroupName_%d"),pMoothPanelHorz);
			COptionUI* pMooth = new COptionUI();
			pMooth->SetGroup(TmpDef);
			wsprintf(TmpDef,_T("%d"),nMoothItem);
			pMooth->SetText(TmpDef);
			pMooth->SetTag(nMoothItem);
			pMooth->OnNotify+= ::UiLib::MakeDelegate(this,&CCalendarUI::OnSelectMooth);

			nMoothItem % 2 ?pMoothSubPanelA->Add(pMooth):pMoothSubPanelB->Add(pMooth);
		}

		//��ʼ������������Ԫ��
		pWeekPanelVert	= new CVerticalLayoutUI();

		wsprintf(TmpDef,_T("CalendGroupName_%d"),this);

		for(int nWeekLine = 1;nWeekLine < 7;nWeekLine++)
		{
			CHorizontalLayoutUI* pWeekLine = new CHorizontalLayoutUI();
			for(int nDayIndex = 1;nDayIndex < 8;nDayIndex++)
			{
				CHorizontalLayoutUI* pDayParent = new CHorizontalLayoutUI();

				CControlUI* pDay = static_cast<CControlUI*>(pICalendar->CreateDayPanel(nDayIndex,TmpDef));

				if(!pDay)
					pDay = this->CreateDayPanel(nDayIndex,TmpDef);

				pDayParent->Add(pDay);
				
				TCalendarInfo* pCalendarInfo	= new TCalendarInfo();
				pDayParent->SetTag((UINT_PTR)pCalendarInfo);
				mTCalendarInfoArray.Add(pCalendarInfo);

				pWeekLine->Add(pDayParent);
			}
			pWeekPanelVert->Add(pWeekLine);
		}

		//��ʼ����Ϣ��������Ԫ��
		pInfoPanelHorz	= new CHorizontalLayoutUI();
		CControlUI* pInfoPanel = pICalendar->CreateInfoPanel();
		if(!pInfoPanel)
			pInfoPanel = CreateInfoPanel();
		pInfoPanelHorz->Add(pInfoPanel);
		pInfoPanelHorz->SetFixedHeight(22);
		pInfoPanelHorz->SetAttribute(_T("bkcolor"),m_DefaultStyle.nStatusBkColor);
		
		Add(pMainTitleHoriz);
		Add(pSubTitleHoriz);
		Add(pWeekPanelVert);
		Add(pMoothPanelHorz);
		Add(pInfoPanelHorz);

		//��ʼ��Ĭ����ʽ
		SetCalendarStyle(m_DefaultStyle);

		//��ʼ����������
		InitCalendarDis(pCurDateTime->tm_year,pCurDateTime->tm_mon);

		OnCalendarChange(pCurDateTime->tm_year,pCurDateTime->tm_mon,pCurDateTime->tm_mday);
	}


	CCalendarUI::~CCalendarUI(void)
	{
		RemoveAllSubTitleString();
	}

	//************************************
	// Method:    GetClass
	// FullName:  CCalendarUI::GetClass
	// Access:    public 
	// Returns:   LPCTSTR
	// Qualifier: const
	// Note:	  
	//************************************
	LPCTSTR CCalendarUI::GetClass() const
	{
		try
		{
			return _T("CalendarUI");
		}
		catch (...)
		{
			throw "CCalendarUI::GetClass";
		}
	}

	//************************************
	// Method:    GetInterface
	// FullName:  CCalendarUI::GetInterface
	// Access:    public 
	// Returns:   LPVOID
	// Qualifier:
	// Parameter: LPCTSTR pstrName
	// Note:	  
	//************************************
	LPVOID CCalendarUI::GetInterface( LPCTSTR pstrName )
	{
		try
		{
			if( _tcscmp(pstrName, _T("Calendar")) == 0 ) return static_cast<CCalendarUI*>(this);
			return CVerticalLayoutUI::GetInterface(pstrName);
		}
		catch (...)
		{
			throw "CCalendarUI::GetInterface";
		}
	}

	//************************************
	// Method:    DoEvent
	// FullName:  CCalendarUI::DoEvent
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: TEventUI & event
	// Note:	  
	//************************************
	void CCalendarUI::DoEvent( TEventUI& event )
	{
		try
		{
			CVerticalLayoutUI::DoEvent(event);
		}
		catch (...)
		{
			throw "CCalendarUI::DoEvent";
		}
	}
	
	//************************************
	// Method:    CreateWeekPanel
	// FullName:  CCalendarUI::CreateWeekPanel
	// Access:    public 
	// Returns:   CControlUI*
	// Qualifier:
	// Parameter: CControlUI * _Panent
	// Parameter: int _Week
	// Note:	  
	//************************************
	CControlUI* CCalendarUI::CreateWeekPanel( CControlUI* _Panent,int _Week )
	{
		try
		{
			CButtonUI* pWeek	= new CButtonUI();
			_Panent->SetFixedHeight(22);
			pWeek->SetMouseEnabled(false);
			return pWeek;
		}
		catch (...)
		{
			throw "CCalendarUI::CreateWeekPanel";
		}
	}

	//************************************
	// Method:    CreateDayPanel
	// FullName:  CCalendarUI::CreateDayPanel
	// Access:    public 
	// Returns:   CControlUI*
	// Qualifier:
	// Parameter: int _Week
	// Parameter: LPCTSTR _GroupName
	// Note:	  
	//************************************
	CControlUI* CCalendarUI::CreateDayPanel( int _Week,LPCTSTR _GroupName )
	{
		try
		{
			COptionUI*	pDay = new COptionUI();
			int heighta = pDay->GetHeight();
			int hegithb = pDay->GetFixedHeight();
			pDay->SetGroup(_GroupName);
			pDay->SetName(_GroupName);
			pDay->SetAttribute(_T("algin"),_T("right"));
			pDay->OnNotify+= ::UiLib::MakeDelegate(this,&CCalendarUI::OnSelcetDay);

			return pDay;
		}
		catch (...)
		{
			throw "CCalendarUI::CreateDayPanel";
		}
	}

	//************************************
	// Method:    SetSubTitleString
	// FullName:  CCalendarUI::SetSubTitleString
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: LPCTSTR _Name
	// Note:	  
	//************************************
	bool CCalendarUI::SetSubTitleString( LPCTSTR _Name )
	{
		try
		{
			if(!_Name || !mSubTitleStringArray.Find(_Name))
				return false;

			TSubTitleString* pSubTitleString = static_cast<TSubTitleString*>(mSubTitleStringArray.Find(_Name));

			SetSubTitleString(*pSubTitleString);

			return true;
		}
		catch (...)
		{
			throw "CCalendarUI::SetSubTitleString";
		}
	}

	//************************************
	// Method:    SetSubTitleString
	// FullName:  CCalendarUI::SetSubTitleString
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: TSubTitleString & _SubTitleString
	// Note:	  
	//************************************
	void CCalendarUI::SetSubTitleString( TSubTitleString& _SubTitleString )
	{
		try
		{
			pSundayBtn->SetText(_SubTitleString.nSundayStr);
			pMondayBtn->SetText(_SubTitleString.nMondayStr);
			pTuesdayBtn->SetText(_SubTitleString.nTuesdayStr);
			pWednesdayBtn->SetText(_SubTitleString.nWednesdayStr);
			pThursdayBtn->SetText(_SubTitleString.nThursdayStr);
			pFridayBtn->SetText(_SubTitleString.nFridayStr);
			pSaturdayBtn->SetText(_SubTitleString.nSaturdayStr);
		}
		catch (...)
		{
			throw "CCalendarUI::SetSubTitleString";
		}
	}
	
	//************************************
	// Method:    SetCalendarStyle
	// FullName:  CCalendarUI::SetCalendarStyle
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: TCalendarStyle & _TCalendarStyle
	// Note:	  
	//************************************
	void CCalendarUI::SetCalendarStyle( TCalendarStyle& _TCalendarStyle )
	{
		try
		{
			this->SetAttribute(_T("bordercolor"),_TCalendarStyle.nCalenderBorderColor);
			this->SetAttribute(_T("bkcolor"),_TCalendarStyle.nCalendarBkColor);
			pMainTitleHoriz->SetAttribute(_T("bkcolor"),_TCalendarStyle.nMainTitleBkColor);
			pSubTitleHoriz->SetAttribute(_T("bkcolor"),_TCalendarStyle.nSubTitleBkColor);
			pLastYearBtn->SetAttribute(_T("hotbkcolor"),_TCalendarStyle.nDayHotColor);
			pMoothSelectBtn->SetAttribute(_T("hotbkcolor"),_TCalendarStyle.nDayHotColor);
			pNextYearBtn->SetAttribute(_T("hotbkcolor"),_TCalendarStyle.nDayHotColor);

			pMoothPanelHorz->SetAttribute(_T("bkcolor"),_TCalendarStyle.nMainTitleBkColor);

			CVerticalLayoutUI* pMoothSubPanelA = static_cast<CVerticalLayoutUI*>(pMoothPanelHorz->GetItemAt(0));
			for(int nMoothItem = 0;nMoothItem < 6;nMoothItem++)
			{
				COptionUI* pMooth = static_cast<COptionUI*>(pMoothSubPanelA->GetItemAt(nMoothItem));
				pMooth->SetAttribute(_T("hotbkcolor"),_TCalendarStyle.nDayHotColor);
				pMooth->SetAttribute(_T("selectedbkcolor"),_TCalendarStyle.nDaySelectColor);
			}
			CVerticalLayoutUI* pMoothSubPanelB = static_cast<CVerticalLayoutUI*>(pMoothPanelHorz->GetItemAt(1));
			for(int nMoothItem = 0;nMoothItem < 6;nMoothItem++)
			{
				COptionUI* pMooth = static_cast<COptionUI*>(pMoothSubPanelB->GetItemAt(nMoothItem));
				pMooth->SetAttribute(_T("hotbkcolor"),_TCalendarStyle.nDayHotColor);
				pMooth->SetAttribute(_T("selectedbkcolor"),_TCalendarStyle.nDaySelectColor);
			}

			for(int nWeekIndex = 0;nWeekIndex < 6;nWeekIndex++)
			{
				CHorizontalLayoutUI* pWeekLine = static_cast<CHorizontalLayoutUI*>(pWeekPanelVert->GetItemAt(nWeekIndex));
				if(!pWeekLine)
					continue;

				pWeekLine->SetAttribute(_T("bkcolor"),!(nWeekIndex%2)?_TCalendarStyle.nWeekIntervalColorA:_TCalendarStyle.nWeekIntervalColorB);

				for(int nDayIndex = 0;nDayIndex < 7;nDayIndex++)
				{
					CVerticalLayoutUI* pDayParent = static_cast<CVerticalLayoutUI*>(pWeekLine->GetItemAt(nDayIndex));
					if(!pDayParent)
						continue;

					if(nWeekIndex%2 && (nDayIndex == 0 || nDayIndex == 6))
						pDayParent->SetAttribute(_T("bkcolor"),_TCalendarStyle.nWeekendColorA);
					else if(!(nWeekIndex%2) && (nDayIndex == 0 || nDayIndex == 6))
						pDayParent->SetAttribute(_T("bkcolor"),_TCalendarStyle.nWeekendColorB);

					COptionUI* pDay = static_cast<COptionUI*>(pDayParent->GetItemAt(0));

					if(!pDay || CDuiString(pDay->GetClass()) != CDuiString(_T("OptionUI")))
						continue;

					pDay->SetAttribute(_T("hotbkcolor"),_TCalendarStyle.nDayHotColor);
					pDay->SetAttribute(_T("selectedbkcolor"),_TCalendarStyle.nDaySelectColor);
				}
			}
		}
		catch (...)
		{
			throw "CCalendarUI::SetCalendarStyle";
		}
	}

	//************************************
	// Method:    AddSubTitleString
	// FullName:  CCalendarUI::AddSubTitleString
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: LPCTSTR _Name
	// Parameter: TSubTitleString & _SubTitleString
	// Note:	  
	//************************************
	bool CCalendarUI::AddSubTitleString( LPCTSTR _Name,TSubTitleString& _SubTitleString )
	{
		try
		{
			if(!_Name && mSubTitleStringArray.Find(_Name) != NULL )
				return false;
			
			TSubTitleString* pSubTitleString = new TSubTitleString();
			pSubTitleString->nSundayStr		= _SubTitleString.nSundayStr;
			pSubTitleString->nMondayStr		= _SubTitleString.nMondayStr;
			pSubTitleString->nWednesdayStr	= _SubTitleString.nWednesdayStr;
			pSubTitleString->nThursdayStr	= _SubTitleString.nThursdayStr;
			pSubTitleString->nFridayStr		= _SubTitleString.nFridayStr;
			pSubTitleString->nSaturdayStr	= _SubTitleString.nSaturdayStr;

			mSubTitleStringArray.Set(_Name,pSubTitleString);

			return true;
		}
		catch (...)
		{
			throw "CCalendar::AddSubTitleString";
		}
	}

	//************************************
	// Method:    AddSubTitleString
	// FullName:  CCalendarUI::AddSubTitleString
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: LPCTSTR _Name
	// Parameter: LPCTSTR _Sunday
	// Parameter: LPCTSTR _Monday
	// Parameter: LPCTSTR _Tuesday
	// Parameter: LPCTSTR _Wednesday
	// Parameter: LPCTSTR _Thursday
	// Parameter: LPCTSTR _Friday
	// Parameter: LPCTSTR _Saturday
	// Note:	  
	//************************************
	bool CCalendarUI::AddSubTitleString( LPCTSTR _Name,LPCTSTR _Sunday,LPCTSTR _Monday,LPCTSTR _Tuesday,LPCTSTR _Wednesday,LPCTSTR _Thursday,LPCTSTR _Friday,LPCTSTR _Saturday )
	{
		try
		{
			if(!_Name || !_Sunday || !_Monday || !_Tuesday || !_Wednesday || !_Thursday || !_Friday || !_Saturday)
				return false;

			TSubTitleString* pSubTitleString = new TSubTitleString();
			pSubTitleString->nSundayStr		= _Sunday;
			pSubTitleString->nMondayStr		= _Monday;
			pSubTitleString->nWednesdayStr	= _Wednesday;
			pSubTitleString->nThursdayStr	= _Thursday;
			pSubTitleString->nFridayStr		= _Friday;
			pSubTitleString->nSaturdayStr	= _Saturday;

			if(pSubTitleString != NULL)
			{
				if( mSubTitleStringArray.Find(_Name) == NULL )
					mSubTitleStringArray.Set(_Name,pSubTitleString);
				else
					delete pSubTitleString;
				pSubTitleString = NULL;
				return true;
			}
			return false;
		}
		catch (...)
		{
			throw "CCalendar::AddSubTitleString";
		}
	}

	//************************************
	// Method:    GetSubTitleString
	// FullName:  CCalendarUI::GetSubTitleString
	// Access:    public 
	// Returns:   TSubTitleString&
	// Qualifier:
	// Parameter: LPCTSTR _Name
	// Note:	  
	//************************************
	TSubTitleString& CCalendarUI::GetSubTitleString( LPCTSTR _Name /*= NULL*/ )
	{
		try
		{
			if(!_Name)
				return mSubTitleString;

			TSubTitleString* pSubTitleString = static_cast<TSubTitleString*>(mSubTitleStringArray.Find(_Name));

			return pSubTitleString?*pSubTitleString:mSubTitleString;
		}
		catch (...)
		{
			throw "CCalendar::GetSubTitleString";
		}
	}

	//************************************
	// Method:    RemoveAtSubTitleString
	// FullName:  CCalendarUI::RemoveAtSubTitleString
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: LPCTSTR _Name
	// Note:	  
	//************************************
	bool CCalendarUI::RemoveAtSubTitleString( LPCTSTR _Name )
	{
		try
		{
			if(!_Name)
				return false;

			TSubTitleString* pSubTitleString = static_cast<TSubTitleString*>(mSubTitleStringArray.Find(_Name));

			if( !pSubTitleString )
				return false;

			delete pSubTitleString;
			return mSubTitleStringArray.Remove(_Name);
		}
		catch (...)
		{
			throw "CCalendar::RemoveAtSubTitleString";
		}
	}

	//************************************
	// Method:    RemoveAllSubTitleString
	// FullName:  CCalendarUI::RemoveAllSubTitleString
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Note:	  
	//************************************
	void CCalendarUI::RemoveAllSubTitleString()
	{
		try
		{
			TSubTitleString* pSubTitleString = NULL;
			for( int i = 0; i< mSubTitleStringArray.GetSize(); i++ ) {
				if(LPCTSTR key = mSubTitleStringArray.GetAt(i)) {
					pSubTitleString = static_cast<TSubTitleString*>(mSubTitleStringArray.Find(key));
					delete pSubTitleString;
				}
			}
		}
		catch (...)
		{
			throw "CCalendarUI::RemoveAllSubTitleString";
		}
	}


	//************************************
	// Method:    IsLeapYear
	// FullName:  CCalendarUI::IsLeapYear
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: int _Year
	// Note:	  ������ж�:(1)year��4����,����year����100���� ��(2)year��4����,����year��400����
	//************************************
	bool CCalendarUI::IsLeapYear( int _Year )
	{
		try
		{
			return ((_Year % 4 == 0 && _Year % 100 != 0) || _Year % 400 == 0);
		}
		catch (...)
		{
			throw "CCalendarUI::IsLeapYear";
		}
	}

	//************************************
	// Method:    GetNumLeapYear
	// FullName:  CCalendarUI::GetNumLeapYear
	// Access:    public 
	// Returns:   int
	// Qualifier:
	// Parameter: int _Year
	// Note:	  
	//************************************
	int CCalendarUI::GetNumLeapYear( int _Year )
	{
		try
		{
			int count=0;
			for(int i=1;i<_Year;i++)
				if(IsLeapYear(i))
					count++;
			return count;
		}
		catch (...)
		{
			throw "CCalendarUI::GetNumLeapYear";
		}
	}
	
	//************************************
	// Method:    DaysOfMonth
	// FullName:  CCalendarUI::DaysOfMonth
	// Access:    public 
	// Returns:   int
	// Qualifier:
	// Parameter: int _Mooth
	// Parameter: int _Year
	// Note:	  
	//************************************
	int CCalendarUI::DaysOfMonth( int _Mooth,int _Year /*= -1*/ )
	{
		try
		{
			int nYear = _Year == -1?pCurDateTime->tm_year:_Year;

			switch (_Mooth)
			{
			case 0: return 0;
			case 1 :
			case 3 :
			case 5 :
			case 7 :
			case 8 :
			case 10 :
			case 12 : return 31 ;
			case 4 :
			case 6 :
			case 9 :
			case 11 : return 30 ;
			case 2 : return IsLeapYear(nYear)?29:28;
			}  
			return 0 ;
		}
		catch (...)
		{
			throw "CCalendarUI::DaysOfMonth";
		}
	}

	//************************************
	// Method:    InitCalendarDis
	// FullName:  CCalendarUI::InitCalendarDis
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: int _Year
	// Parameter: int _Month
	// Note:	  
	//************************************
	void CCalendarUI::InitCalendarDis( int _Year,int _Month )
	{
		try
		{
			if(_Month > 12){
				_Month = 1;
				_Year++;
			}

			if(_Month < 1) {
				_Month = 12;
				_Year--;
			}

			pICalendar->OnCalendarUpdateStart(_Year,_Month);

			WCHAR TmpDef[64];
			wsprintf(TmpDef,_T("%.4d-%.2d"),_Year,_Month);
			pMoothSelectBtn->SetText(TmpDef);

			int nLastMaxDay		= _Month == 1 ?DaysOfMonth(12,_Year-1):DaysOfMonth(_Month-1,_Year);
			int nLastMinDay		= 0;
			int nAsMooth		= 0;
			int nDays			= DaysOfMonth(_Month,_Year);
			int nWeek			= 0;
			int nFDay			= 0;
			int nCalDay			= 0;
			for(int i=1;i<=13;i++)
			{
				nFDay += DaysOfMonth(i-1);

				int nCurFday = (nFDay+(_Year-1)*365+GetNumLeapYear(_Year))%7+1;
				if(i == _Month)
					nWeek = nCurFday;
			}

			nAsMooth	= nWeek == 0?0:_Month-1;
			nLastMinDay = nWeek == 0?0:nLastMaxDay-nWeek;
			for(int nWeekIndex = 0;nWeekIndex < 6;nWeekIndex++)
			{
				CHorizontalLayoutUI* pWeekLine = static_cast<CHorizontalLayoutUI*>(pWeekPanelVert->GetItemAt(nWeekIndex));
				
				for(int nDayIndex = 0;nDayIndex < 7;nDayIndex++)
				{
					CVerticalLayoutUI* pDayParent = static_cast<CVerticalLayoutUI*>(pWeekLine->GetItemAt(nDayIndex));
					if(!pDayParent)
						continue;

					TCalendarInfo* pTCalendarInfo = (TCalendarInfo*)pDayParent->GetTag();
					if(!pTCalendarInfo)
						continue;

					COptionUI* pDay = static_cast<COptionUI*>(pDayParent->GetItemAt(0));

					if(!(nWeek && nDayIndex < nWeek && nCalDay <= nDays))
					{
						nCalDay++;
						nAsMooth = nCalDay <= nDays?_Month:_Month+1;
					}

					if((nLastMinDay <= nLastMaxDay && nAsMooth != _Month) || nCalDay >= nDays){
						nLastMinDay++;
						nAsMooth	= _Month+1;
					}

					if(nCalDay == nDays) {
						nLastMinDay = 0;
						nAsMooth	= _Month;
					}
					
					pTCalendarInfo->nYear		= _Year;
					pTCalendarInfo->nMooth		= _Month;
					pTCalendarInfo->nDay		= ((nWeek && nDayIndex < nWeek) || nCalDay > nDays)?nLastMinDay:nCalDay;
					pTCalendarInfo->nWeekLine	= nWeekIndex;
					pTCalendarInfo->nWeek		= nWeek;
					pTCalendarInfo->nAsMooth	= _Month;

					if(nCalDay < 1)
						pTCalendarInfo->nAsMooth= _Month -1;
					else if(nCalDay > nDays)
						pTCalendarInfo->nAsMooth= _Month +1;
					
					pICalendar->InitDayPanel(pDay,pTCalendarInfo->nYear,_Month,pTCalendarInfo->nDay,pTCalendarInfo->nWeek,pTCalendarInfo->nWeekLine,pTCalendarInfo->nAsMooth);

					pDay->Selected(nCalDay == mToday);

				}
				nWeek = 0;
			}
			pICalendar->OnCalendarUpdateEnd(_Year,_Month);
		}
		catch (...)
		{
			throw "CCalendarUI::InitCalendarDis";
		}
	}

	//************************************
	// Method:    OnLastYear
	// FullName:  CCalendarUI::OnLastYear
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: void * _Param
	// Note:	  
	//************************************
	bool CCalendarUI::OnLastYear( void* _Param )
	{
		try
		{
			TNotifyUI* pMsg = (TNotifyUI*)_Param;
			if( pMsg->sType == _T("click") && pMsg->pSender == pLastYearBtn)
			{
				pCurDateTime->tm_year--;

				if(GetEnabledYearSel())
					InitCalendarDis(pCurDateTime->tm_year,pCurDateTime->tm_mon);

				this->GetManager()->SendNotify(pLastYearBtn,_T("OnCalendarLastYear"),pCurDateTime->tm_year,pCurDateTime->tm_mon);
				pMoothPanelHorz->SetVisible(false);

				return true;
			}
			return false;
		}
		catch (...)
		{
			throw "CCalendarUI::OnLastYear";
		}
	}

	//************************************
	// Method:    OnMoothSelect
	// FullName:  CCalendarUI::OnMoothSelect
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: void * _Param
	// Note:	  
	//************************************
	bool CCalendarUI::OnMoothSelect( void* _Param )
	{
		try
		{
			TNotifyUI* pMsg = (TNotifyUI*)_Param;
			if( pMsg->sType == _T("click") && pMsg->pSender == pMoothSelectBtn && GetEnabledMoothSel())
			{
				pMoothPanelHorz->SetVisible(true);
				RECT rc = pMoothSelectBtn->GetPos();
				rc.top		= rc.bottom;
				rc.bottom	= rc.top + 132;
				pMoothPanelHorz->SetPos(rc);
				pMoothPanelHorz->SetFloat(true);
			}
			return false;
		}
		catch (...)
		{
			throw "CCalendarUI::OnMoothSelect";
		}
	}

	//************************************
	// Method:    OnNextYear
	// FullName:  CCalendarUI::OnNextYear
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: void * _Param
	// Note:	  
	//************************************
	bool CCalendarUI::OnNextYear( void* _Param )
	{
		try
		{
			TNotifyUI* pMsg = (TNotifyUI*)_Param;
			if( pMsg->sType == _T("click") && pMsg->pSender == pNextYearBtn)
			{
				if(GetEnabledYearSel())
					InitCalendarDis(++pCurDateTime->tm_year,pCurDateTime->tm_mon);

				this->GetManager()->SendNotify(this,_T("OnCalendarNextYear"),pCurDateTime->tm_year,pCurDateTime->tm_mon);
				pMoothPanelHorz->SetVisible(false);
			}
			return true;
		}
		catch (...)
		{
			throw "CCalendarUI::OnNextYear";
		}
	}

	//************************************
	// Method:    OnSelectMooth
	// FullName:  CCalendarUI::OnSelectMooth
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: void * _Param
	// Note:	  
	//************************************
	bool CCalendarUI::OnSelectMooth( void* _Param )
	{
		try
		{
			TNotifyUI* pMsg = (TNotifyUI*)_Param;
			if( pMsg->sType == _T("click") && pMsg->pSender->GetTag() > 0)
			{
				if(GetEnabledMoothSel())
					InitCalendarDis(pCurDateTime->tm_year,pMsg->pSender->GetTag());

				this->GetManager()->SendNotify(this,_T("OnSelectMooth"),pCurDateTime->tm_year,pCurDateTime->tm_mon);
				pMoothPanelHorz->SetVisible(false);
			}
			return true;
		}
		catch (...)
		{
			throw "CCalendarUI::OnSelectMooth";
		}
	}
	
	//************************************
	// Method:    OnSelcetDay
	// FullName:  CCalendarUI::OnSelcetDay
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: void * _Param
	// Note:	  
	//************************************
	bool CCalendarUI::OnSelcetDay( void* _Param )
	{
		try
		{
			TNotifyUI* pMsg = (TNotifyUI*)_Param;
			if( pMsg->sType == _T("click"))
			{	
				TCalendarInfo* pTCalendarInfo = (TCalendarInfo*)pMsg->pSender->GetParent()->GetTag();

				if(GetEnabledMoothSel() && pTCalendarInfo && pTCalendarInfo->nMooth != pTCalendarInfo->nAsMooth){
					InitCalendarDis(pTCalendarInfo->nYear,pTCalendarInfo->nAsMooth);
				}
				OnCalendarChange(pTCalendarInfo->nYear,pTCalendarInfo->nMooth,pTCalendarInfo->nDay);

				CalDateTime(pTCalendarInfo->nYear,pTCalendarInfo->nMooth,pTCalendarInfo->nDay);
				this->GetManager()->SendNotify(this,_T("OnSelcetDay"),pMsg->pSender->GetTag(),(LPARAM)pMsg->pSender,true);
				pMoothPanelHorz->SetVisible(false);
			}
			return true;
		}
		catch (...)
		{
			throw "CCalendarUI::OnSelcetDay";
		}
	}

	//************************************
	// Method:    SetCallback
	// FullName:  CCalendarUI::SetCallback
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: ICalendar * _ICalendar
	// Note:	  
	//************************************
	void CCalendarUI::SetCallback( ICalendar* _ICalendar )
	{
		try
		{
			if(pICalendar)
				pICalendar = _ICalendar;
		}
		catch (...)
		{
			throw "CCalendarUI::SetCallback";
		}
	}

	//************************************
	// Method:    InitWeekPanel
	// FullName:  CCalendarUI::InitWeekPanel
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: CControlUI * _Control
	// Parameter: int _Week
	// Note:	  
	//************************************
	void CCalendarUI::InitWeekPanel( CControlUI* _Control,int _Week )
	{
		try
		{
			switch (_Week)
			{
			case 0:_Control->SetText(mSubTitleString.nSundayStr);break;
			case 1:_Control->SetText(mSubTitleString.nMondayStr);break;
			case 2:_Control->SetText(mSubTitleString.nTuesdayStr);break;
			case 3:_Control->SetText(mSubTitleString.nWednesdayStr);break;
			case 4:_Control->SetText(mSubTitleString.nThursdayStr);break;
			case 5:_Control->SetText(mSubTitleString.nFridayStr);break;
			case 6:_Control->SetText(mSubTitleString.nSaturdayStr);break;
			default:
				break;
			}
		}
		catch (...)
		{
			throw "CCalendarUI::InitWeekPanel";
		}
	}
	
	//************************************
	// Method:    InitDayPanel
	// FullName:  CCalendarUI::InitDayPanel
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: CControlUI * _Control
	// Parameter: TCalendarInfo & _CalendarInfo
	// Note:	  
	//************************************
	void CCalendarUI::InitDayPanel( CControlUI* _Control,TCalendarInfo& _CalendarInfo )
	{
		try
		{
			InitDayPanel(_Control,_CalendarInfo.nYear,_CalendarInfo.nMooth,_CalendarInfo.nDay,_CalendarInfo.nWeek,_CalendarInfo.nWeekLine,_CalendarInfo.nAsMooth);
		}
		catch (...)
		{
			throw "CCalendarUI::InitDayPanel";
		}
	}

	//************************************
	// Method:    InitDayPanel
	// FullName:  CCalendarUI::InitDayPanel
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: CControlUI * _Control
	// Parameter: int _Year
	// Parameter: int _Mooth
	// Parameter: int _Day
	// Parameter: int _Week
	// Parameter: int _WeekLine
	// Parameter: int _AsMooth
	// Note:	  
	//************************************
	void CCalendarUI::InitDayPanel( CControlUI* _Control,int _Year,int _Mooth,int _Day,int _Week,int _WeekLine,int _AsMooth )
	{
		try
		{
			if(_AsMooth != _Mooth)
				_Control->SetAttribute(_T("bkcolor"),m_DefaultStyle.nNoCurMonthDayColor);
			else 
				_Control->SetAttribute(_T("bkcolor"),_T(""));

			if(_Mooth != _AsMooth)
				_Control->SetMouseEnabled(GetEnabledMoothSel());
			else 
				_Control->SetMouseEnabled(true);
			
			WCHAR TmpDef[64];
			wsprintf(TmpDef,_T("%d"),_Day);
			_Control->SetText(TmpDef);
		}
		catch (...)
		{
			throw "CCalendarUI::InitDayPanel";
		}
	}

	//************************************
	// Method:    SetEnabledYearSel
	// FullName:  CCalendarUI::SetEnabledYearSel
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: bool _Enabled
	// Note:	  
	//************************************
	void CCalendarUI::SetEnabledYearSel( bool _Enabled /*= true*/ )
	{
		try
		{
			pEnabledYearSel = _Enabled;
		}
		catch (...)
		{
			throw "CCalendarUI::SetEnabledYearSel";
		}
	}

	//************************************
	// Method:    GetEnabledYearSel
	// FullName:  CCalendarUI::GetEnabledYearSel
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Note:	  
	//************************************
	bool CCalendarUI::GetEnabledYearSel()
	{
		try
		{
			return pEnabledYearSel;
		}
		catch (...)
		{
			throw "CCalendarUI::GetEnabledYearSel";
		}
	}

	//************************************
	// Method:    SetEnabledMoothSel
	// FullName:  CCalendarUI::SetEnabledMoothSel
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: bool _Enabled
	// Note:	  
	//************************************
	void CCalendarUI::SetEnabledMoothSel( bool _Enabled /*= true*/ )
	{
		try
		{
			pEnabledMoothSel = _Enabled;
		}
		catch (...)
		{
			throw "CCalendarUI::SetEnabledMoothSel";
		}
	}

	//************************************
	// Method:    GetEnabledMoothSel
	// FullName:  CCalendarUI::GetEnabledMoothSel
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Note:	  
	//************************************
	bool CCalendarUI::GetEnabledMoothSel()
	{
		try
		{
			return pEnabledMoothSel;
		}
		catch (...)
		{
			throw "CCalendarUI::GetEnabledMoothSel";
		}
	}

	//************************************
	// Method:    OnCalendarChangeStart
	// FullName:  CCalendarUI::OnCalendarUpdateStart
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: int _Year
	// Parameter: int _Mooth
	// Note:	  
	//************************************
	void CCalendarUI::OnCalendarUpdateStart( int _Year,int _Mooth)
	{
		try
		{
			WCHAR TmpDef[64];
			wsprintf(TmpDef,_T("%.4d-%.2d"),_Year,_Mooth);
			pMoothSelectBtn->SetText(TmpDef);
			wsprintf(TmpDef,_T("%.4d<<"),_Year-1);
			pLastYearBtn->SetText(TmpDef);
			wsprintf(TmpDef,_T(">>%.4d"),_Year+1);
			pNextYearBtn->SetText(TmpDef);
		}
		catch (...)
		{
			throw "CCalendarUI::OnCalendarChangeStart";
		}
	}

	//************************************
	// Method:    SetAttribute
	// FullName:  CCalendarUI::SetAttribute
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR pstrName
	// Parameter: LPCTSTR pstrValue
	// Note:	  
	//************************************
	void CCalendarUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		try
		{
			if( _tcscmp(pstrName, _T("enableyearsel")) == 0 ) SetEnabledYearSel(_tcscmp(pstrValue, _T("true")) == 0);
			else if( _tcscmp(pstrName, _T("enablemoothsel")) == 0 ) SetEnabledYearSel(_tcscmp(pstrValue, _T("true")) == 0);
			else if( _tcscmp(pstrName, _T("mtheight")) == 0 ) SetMainTitleHeight(_ttoi(pstrValue));
			else if( _tcscmp(pstrName, _T("stheight")) == 0 ) SetSubTitleHeight(_ttoi(pstrValue));
			else if( _tcscmp(pstrName, _T("itheight")) == 0 ) SetStatusInfoHeight(_ttoi(pstrValue));
			else if( _tcscmp(pstrName, _T("style")) == 0 ) AnyCalendarStyle(pstrValue);
			else if( _tcscmp(pstrName, _T("cbtargetname")) == 0 ) SetComboTargetName(pstrValue);
			else CVerticalLayoutUI::SetAttribute(pstrName,pstrValue);
		}
		catch (...)
		{
			throw "CCalendarUI::SetAttribute";
		}
	}

	//************************************
	// Method:    CreateInfoPanel
	// FullName:  CCalendarUI::CreateInfoPanel
	// Access:    public 
	// Returns:   CControlUI*
	// Qualifier:
	// Note:	  
	//************************************
	CControlUI* CCalendarUI::CreateInfoPanel()
	{
		try
		{
			CHorizontalLayoutUI* pControl	= new CHorizontalLayoutUI();
			pDateTimeBtn					= new CButtonUI();
			pToDayBtn						= new CButtonUI();

			pDateTimeBtn->SetMouseEnabled(false);
			pDateTimeBtn->SetFixedWidth(100);
			pControl->Add(pDateTimeBtn);
			pControl->Add(new CControlUI());
			pControl->Add(pToDayBtn);
			
			pToDayBtn->OnNotify			   += MakeDelegate(this,&CCalendarUI::OnToday);
			pToDayBtn->SetText(mSubTitleString.nToDayString);
			pToDayBtn->SetFixedWidth(60);
			pToDayBtn->SetAttribute(_T("hotbkcolor"),m_DefaultStyle.nDayHotColor);

			return pControl;
		}
		catch (...)
		{
			throw "CCalendarUI::CreateInfoPanel";
		}
	}

	//************************************
	// Method:    OnToday
	// FullName:  CCalendarUI::OnToday
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Parameter: void * _Param
	// Note:	  
	//************************************
	bool CCalendarUI::OnToday( void* _Param )
	{
		try
		{
			TNotifyUI* pMsg = (TNotifyUI*)_Param;
			if( pMsg->sType == _T("click"))
			{
				//��ʼ����ǰ����
				time_t now;	
				time(&now);
				pCurDateTime = localtime(&now);
				pCurDateTime->tm_year	= pCurDateTime->tm_year-100+2000;
				pCurDateTime->tm_mon	= pCurDateTime->tm_mon+1;
				mToday					= pCurDateTime->tm_mday;

				InitCalendarDis(pCurDateTime->tm_year,pCurDateTime->tm_mon);


				this->GetManager()->SendNotify(this,_T("OnToday"),pMsg->pSender->GetTag(),(LPARAM)pMsg->pSender,true);
				pMoothPanelHorz->SetVisible(false);
			}
			return true;
		}
		catch (...)
		{
			throw "CCalendarUI::OnToday";
		}
	}

	//************************************
	// Method:    OnCalendarChange
	// FullName:  CCalendarUI::OnCalendarChange
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: int _Year
	// Parameter: int _Mooth
	// Parameter: int _Day
	// Note:	  
	//************************************
	void CCalendarUI::OnCalendarChange( int _Year,int _Mooth,int _Day )
	{
		try
		{
			WCHAR TmpDef[64];
			wsprintf(TmpDef,_T("%.4d-%.2d-%.2d"),_Year,_Mooth,_Day);
			if(pDateTimeBtn)
				pDateTimeBtn->SetText(TmpDef);
		}
		catch (...)
		{
			throw "CCalendarUI::OnCalendarChange";
		}
	}

	//************************************
	// Method:    SetMainTitleHeight
	// FullName:  CCalendarUI::SetMainTitleHeight
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: int _Height
	// Note:	  
	//************************************
	void CCalendarUI::SetMainTitleHeight( int _Height )
	{
		try
		{
			pMainTitleHoriz->SetFixedHeight(_Height);
		}
		catch (...)
		{
			throw "CCalendarUI::SetMainTitleHeight";
		}
	}

	//************************************
	// Method:    GetMainTitleHeight
	// FullName:  CCalendarUI::GetMainTitleHeight
	// Access:    public 
	// Returns:   int
	// Qualifier:
	// Note:	  
	//************************************
	int CCalendarUI::GetMainTitleHeight()
	{
		try
		{
			return pMainTitleHoriz->GetFixedHeight();
		}
		catch (...)
		{
			throw "CCalendarUI::GetMainTitleHeight";
		}
	}

	//************************************
	// Method:    SetSubTitleHeight
	// FullName:  CCalendarUI::SetSubTitleHeight
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: int _Height
	// Note:	  
	//************************************
	void CCalendarUI::SetSubTitleHeight( int _Height )
	{
		try
		{
			pSubTitleHoriz->SetFixedHeight(_Height);
		}
		catch (...)
		{
			throw "CCalendarUI::SetSubTitleHeight";
		}
	}

	//************************************
	// Method:    GetSubTitleHeight
	// FullName:  CCalendarUI::GetSubTitleHeight
	// Access:    public 
	// Returns:   int
	// Qualifier:
	// Note:	  
	//************************************
	int CCalendarUI::GetSubTitleHeight()
	{
		try
		{
			return pSubTitleHoriz->GetFixedHeight();
		}
		catch (...)
		{
			throw "CCalendarUI::GetSubTitleHeight";
		}
	}

	//************************************
	// Method:    SetStatusInfoHeight
	// FullName:  CCalendarUI::SetStatusInfoHeight
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: int _Height
	// Note:	  
	//************************************
	void CCalendarUI::SetStatusInfoHeight( int _Height )
	{
		try
		{
			pInfoPanelHorz->SetFixedHeight(_Height);
		}
		catch (...)
		{
			throw "CCalendarUI::SetStatusInfoHeight";
		}
	}

	//************************************
	// Method:    GetStatusInfoHeight
	// FullName:  CCalendarUI::GetStatusInfoHeight
	// Access:    public 
	// Returns:   int
	// Qualifier:
	// Note:	  
	//************************************
	int CCalendarUI::GetStatusInfoHeight()
	{
		try
		{
			return pInfoPanelHorz->GetFixedHeight();
		}
		catch (...)
		{
			throw "CCalendarUI::GetStatusInfoHeight";
		}
	}
	
	//************************************
	// Method:    AnyCalendarStyle
	// FullName:  CCalendarUI::AnyCalendarStyle
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR _StyleStr
	// Parameter: TCalendarStyle * _TCalendarStyle
	// Note:	  
	//************************************
	void CCalendarUI::AnyCalendarStyle( LPCTSTR _StyleStr,TCalendarStyle* _TCalendarStyle /*= NULL*/ )
	{
		try
		{
			CDuiString sItem;
			CDuiString sValue;
			LPTSTR pstr = NULL;

			TCalendarStyle* pCalendarInfo = _TCalendarStyle?_TCalendarStyle:&m_DefaultStyle;

			while( *_StyleStr != _T('\0') ) {
				sItem.Empty();
				sValue.Empty();
				while( *_StyleStr > _T('\0') && *_StyleStr <= _T(' ') ) _StyleStr = ::CharNext(_StyleStr);
				while( *_StyleStr != _T('\0') && *_StyleStr != _T('=') && *_StyleStr > _T(' ') ) {
					LPTSTR pstrTemp = ::CharNext(_StyleStr);
					while( _StyleStr < pstrTemp) {
						sItem += *_StyleStr++;
					}
				}
				while( *_StyleStr > _T('\0') && *_StyleStr <= _T(' ') ) _StyleStr = ::CharNext(_StyleStr);
				if( *_StyleStr++ != _T('=') ) break;
				while( *_StyleStr > _T('\0') && *_StyleStr <= _T(' ') ) _StyleStr = ::CharNext(_StyleStr);
				if( *_StyleStr++ != _T('\'') ) break;
				while( *_StyleStr != _T('\0') && *_StyleStr != _T('\'') ) {
					LPTSTR pstrTemp = ::CharNext(_StyleStr);
					while( _StyleStr < pstrTemp) {
						sValue += *_StyleStr++;
					}
				}
				if( *_StyleStr++ != _T('\'') ) break;
				if( !sValue.IsEmpty() ) {
					if( sItem == _T("bbcolor") ) pCalendarInfo->nCalenderBorderColor = sValue;
					else if( sItem == _T("bkcolor") ) pCalendarInfo->nCalendarBkColor = sValue;
					else if( sItem == _T("tbkcolor") ) pCalendarInfo->nMainTitleBkColor = sValue;
					else if( sItem == _T("sbkcolor") ) pCalendarInfo->nSubTitleBkColor = sValue;
					else if( sItem == _T("webkcolora") ) pCalendarInfo->nWeekendColorA = sValue;
					else if( sItem == _T("webkcolorb") ) pCalendarInfo->nWeekendColorB = sValue;
					else if( sItem == _T("dayhotcolor") ) pCalendarInfo->nDayHotColor = sValue;
					else if( sItem == _T("daypushedcolor") ) pCalendarInfo->nDayPushedColor = sValue;
					else if( sItem == _T("dayselcolor") ) pCalendarInfo->nDaySelectColor = sValue;
					else if( sItem == _T("daydiscolor") ) pCalendarInfo->nDayDisabledColor = sValue;
					else if( sItem == _T("nocurmonthdaycolor") ) pCalendarInfo->nNoCurMonthDayColor = sValue;
					else if( sItem == _T("weekicolora") ) pCalendarInfo->nWeekIntervalColorA = sValue;
					else if( sItem == _T("weekicolorb") ) pCalendarInfo->nWeekIntervalColorB = sValue;
					else if( sItem == _T("ibkcolor") ) pCalendarInfo->nStatusBkColor = sValue;
				}
				if( *_StyleStr++ != _T(' ') ) break;
			}
			SetCalendarStyle(*pCalendarInfo);
		}
		catch (...)
		{
			throw "CCalendarUI::AnyCalendarStyle";
		}
	}
	
	//************************************
	// Method:    CalDateTime
	// FullName:  CCalendarUI::CalDateTime
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: TCalendarInfo * _TCalendarInfo
	// Note:	  
	//************************************
	void CCalendarUI::CalDateTime( TCalendarInfo* _TCalendarInfo )
	{
		try
		{
			if(_TCalendarInfo)
				CalDateTime(_TCalendarInfo->nYear,_TCalendarInfo->nMooth,_TCalendarInfo->nDay);
		}
		catch (...)
		{
			throw "CCalendarUI::CalDateTime";
		}
	}

	//************************************
	// Method:    CalDateTime
	// FullName:  CCalendarUI::CalDateTime
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: int _Year
	// Parameter: int _Mooth
	// Parameter: int _Day
	// Note:	  
	//************************************
	void CCalendarUI::CalDateTime( int _Year,int _Mooth,int _Day )
	{
		try
		{
			mDateTime.Format(_T("%.04d-%.02d-%.02d"),_Year,_Mooth,_Day);
		}
		catch (...)
		{
			throw "CCalendarUI::CalDateTime";
		}
	}

	//************************************
	// Method:    GetCurSelDateTime
	// FullName:  CCalendarUI::GetCurSelDateTime
	// Access:    public 
	// Returns:   UiLib::CDuiString
	// Qualifier:
	// Note:	  
	//************************************
	UiLib::CDuiString CCalendarUI::GetCurSelDateTime()
	{
		try
		{
			return mDateTime;
		}
		catch (...)
		{
			throw "CCalendarUI::GetCurSelDateTime";
		}
	}

	//************************************
	// Method:    SetComboTargetName
	// FullName:  CCalendarUI::SetComboTargetName
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR pstrName
	// Note:	  
	//************************************
	void CCalendarUI::SetComboTargetName( LPCTSTR pstrName )
	{
		try
		{
			m_sComboTargetName = mDateTime;
		}
		catch (...)
		{
			throw "CCalendarUI::SetComboTargetName";
		}
	}

	//************************************
	// Method:    GetComTargetName
	// FullName:  CCalendarUI::GetComTargetName
	// Access:    public 
	// Returns:   UiLib::CDuiString
	// Qualifier:
	// Note:	  
	//************************************
	UiLib::CDuiString CCalendarUI::GetComTargetName()
	{
		try
		{
			return m_sComboTargetName;
		}
		catch (...)
		{
			throw "CCalendarUI::GetComTargetName";
		}
	}

	//************************************
	// Method:    GetComTargetObj
	// FullName:  CCalendarUI::GetComTargetObj
	// Access:    public 
	// Returns:   CControlUI*
	// Qualifier:
	// Note:	  
	//************************************
	CControlUI* CCalendarUI::GetComTargetObj()
	{
		try
		{
			if(GetComTargetName().IsEmpty())
				return NULL;
			CControlUI* pControl = static_cast<CControlUI*>(GetManager()->FindControl(m_sComboTargetName));
			return pControl;
		}
		catch (...)
		{
			throw "CCalendarUI::GetComTargetObj";
		}
	}
		
	//************************************
	// Method:    ShowCalendarDlg
	// FullName:  CCalendarUI::ShowCalendarDlg
	// Access:    public static 
	// Returns:   void
	// Qualifier:
	// Parameter: CControlUI * _pControl
	// Note:	  
	//************************************
	void CCalendarUI::ShowCalendarDlg( CControlUI* _pControl )
	{
		try
		{
			CCalendarDlg mCalendarDlg(_pControl);
			mCalendarDlg.ShowModal();
		}
		catch (...)
		{
			throw "CCalendarUI::ShowCalendarDlg";
		}
	}
}
