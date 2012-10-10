#include "StdAfx.h"
#include "UIEdit.h"

#include <regex>
#include <atltime.h>
#include "MMSystem.h"

#pragma comment(lib,"Winmm.lib")

using namespace std::tr1;

namespace UiLib
{
	class CEditWnd : public CWindowWnd
	{
	public:
		CEditWnd();

		void Init(CEditUI* pOwner);
		RECT CalPos();

		LPCTSTR GetWindowClassName() const;
		LPCTSTR GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		CEditUI* m_pOwner;
		HBRUSH m_hBkBrush;
		bool m_bInit;
	};


	CEditWnd::CEditWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false)
	{
	}

	void CEditWnd::Init(CEditUI* pOwner)
	{
		m_pOwner = pOwner;
		RECT rcPos = CalPos();
		UINT uStyle = WS_CHILD | ES_AUTOHSCROLL;
		if( m_pOwner->IsPasswordMode() ) uStyle |= ES_PASSWORD;
		Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
		SetWindowFont(m_hWnd, m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->hFont, TRUE);
		Edit_LimitText(m_hWnd, m_pOwner->GetMaxChar());
		if( m_pOwner->IsPasswordMode() ) Edit_SetPasswordChar(m_hWnd, m_pOwner->GetPasswordChar());
		Edit_SetText(m_hWnd, m_pOwner->GetText());
		Edit_SetModify(m_hWnd, FALSE);
		SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
		Edit_Enable(m_hWnd, m_pOwner->IsEnabled() == true);
		Edit_SetReadOnly(m_hWnd, m_pOwner->IsReadOnly() == true);
		//Styls
		LONG styleValue = ::GetWindowLong(m_hWnd, GWL_STYLE);
		styleValue |= pOwner->GetWindowStyls();
		::SetWindowLong(GetHWND(), GWL_STYLE, styleValue);
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		::SetFocus(m_hWnd);
		m_bInit = true;
	}

	RECT CEditWnd::CalPos()
	{
		CDuiRect rcPos = m_pOwner->GetPos();
		RECT rcInset = m_pOwner->GetTextPadding();
		rcPos.left += rcInset.left;
		rcPos.top += rcInset.top;
		rcPos.right -= rcInset.right;
		rcPos.bottom -= rcInset.bottom;
		LONG lEditHeight = m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->tm.tmHeight;
		if( lEditHeight < rcPos.GetHeight() ) {
			rcPos.top += (rcPos.GetHeight() - lEditHeight) / 2;
			rcPos.bottom = rcPos.top + lEditHeight;
		}
		return rcPos;
	}

	LPCTSTR CEditWnd::GetWindowClassName() const
	{
		return _T("EditWnd");
	}

	LPCTSTR CEditWnd::GetSuperClassName() const
	{
		return WC_EDIT;
	}

	void CEditWnd::OnFinalMessage(HWND /*hWnd*/)
	{
		m_pOwner->Invalidate();
		// Clear reference and die
		if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
		m_pOwner->m_pWindow = NULL;
		delete this;
	}

	LRESULT CEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;

		if( uMsg == WM_SETFOCUS && !m_pOwner->GetTipValue().IsEmpty() )
		{
			if(CDuiString(_T("__IsTipValue__"))+m_pOwner->GetText() == m_pOwner->GetTipValue())
				m_pOwner->SetText(_T(""));
			m_pOwner->m_RegluarSrcText = m_pOwner->GetText();
			m_pOwner->m_Timer.pEditUI = m_pOwner;
			m_pOwner->m_Timer.m_CurTickCount = ::GetTickCount();
			m_pOwner->m_Timer.Start();
		}
		
		if( uMsg == WM_KILLFOCUS )
		{
			if(m_pOwner->GetTipValue().GetLength() > 0 && m_pOwner->GetText().GetLength() == 0)
				m_pOwner->SetText(m_pOwner->GetSrcTipValue());
			m_pOwner->m_Timer.Stop();

			lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
		}
		else if( uMsg == OCM_COMMAND ) {
			if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE ) lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
			else if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE ) {
				RECT rcClient;
				::GetClientRect(m_hWnd, &rcClient);
				::InvalidateRect(m_hWnd, &rcClient, FALSE);
			}
		}
		else if( uMsg == WM_KEYDOWN ) {

			TEventUI event = { 0 };
			event.Type = UIEVENT_KEYDOWN;
			event.chKey = (TCHAR)wParam;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			m_pOwner->Event(event);

			if (TCHAR(wParam) == VK_RETURN )
				m_pOwner->GetManager()->SendNotify(m_pOwner, _T("return"));

		}
		else if( uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC ) {
			if( m_pOwner->GetNativeEditBkColor() == 0xFFFFFFFF ) return NULL;
			::SetBkMode((HDC)wParam, TRANSPARENT);
			DWORD dwTextColor = m_pOwner->GetTextColor();
			::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
			if( m_hBkBrush == NULL ) {
				DWORD clrColor = m_pOwner->GetNativeEditBkColor();
				m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
			}
			return (LRESULT)m_hBkBrush;
		}
		else bHandled = FALSE;
		if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT CEditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		PostMessage(WM_CLOSE);
		return lRes;
	}

	LRESULT CEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if( !m_bInit ) return 0;
		if( m_pOwner == NULL ) return 0;
		// Copy text back
		int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
		LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
		ASSERT(pstr);
		if( pstr == NULL ) return 0;
		::GetWindowText(m_hWnd, pstr, cchLen);
		m_pOwner->m_sText = pstr;
		m_pOwner->GetManager()->SendNotify(m_pOwner, _T("textchanged"));
		return 0;
	}

	//************************************
	// Method:    CTimer
	// FullName:  CEditUI::CTimer::CTimer
	// Access:    public 
	// Returns:   
	// Qualifier:
	// Note:	  
	//************************************
	CEditUI::CTimer::CTimer()
	{
		try
		{
			mIsRun			= false;
			pEditUI			= NULL;
			m_Delay			= 1000;
			m_CurTickCount	= ::GetTickCount();

			TIMECAPS tc;
			//���ú���timeGetDevCapsȡ��ϵͳ�ֱ��ʵ�ȡֵ��Χ������޴������;
			if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR)
			{
				m_TimerAccuracy=min(max(tc.wPeriodMin,1),tc.wPeriodMax);//�ֱ��ʵ�ֵ���ܳ���ϵͳ��ȡֵ��Χ
				timeBeginPeriod(m_TimerAccuracy);//����timeBeginPeriod�������ö�ʱ���ķֱ���
			}
		}
		catch(...)
		{
			throw "CEditUI::CTimer::CTimer";
		}
	}

	//************************************
	// Method:    ~CTimer
	// FullName:  CEditUI::CTimer::~CTimer
	// Access:    public 
	// Returns:   
	// Qualifier:
	// Note:	  
	//************************************
	CEditUI::CTimer::~CTimer()
	{
		try
		{
		}
		catch(...)
		{
			throw "CEditUI::CTimer::~CTimer";
		}
	}

	//************************************
	// Method:    Start
	// FullName:  CEditUI::CTimer::Start
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Note:	  
	//************************************
	void CEditUI::CTimer::Start()
	{
		try
		{
			if(!pEditUI || !pEditUI->GetEnableTimer())
				return;

			m_TimerID	= timeSetEvent(m_Delay,m_TimerAccuracy,TimerProc,(DWORD_PTR)this,TIME_PERIODIC);
			mIsRun = true;
		}
		catch(...)
		{
			throw "CEditUI::CTimer::Start";
		}
	}

	//************************************
	// Method:    Stop
	// FullName:  CEditUI::CTimer::Stop
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Note:	  
	//************************************
	void CEditUI::CTimer::Stop()
	{
		try
		{
			if(!mIsRun)
				return;

			timeKillEvent(m_TimerID); // ɾ�����õķֱ���
			timeEndPeriod(m_TimerAccuracy);
		}
		catch(...)
		{
			throw "CEditUI::CTimer::Stop";
		}
	}

	//************************************
	// Method:    SetDelay
	// FullName:  CEditUI::CTimer::SetDelay
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: UINT _Delay
	// Note:	  
	//************************************
	void CEditUI::CTimer::SetDelay( UINT _Delay /*= 1000*/ )
	{
		try
		{
			m_Delay = _Delay;

			if(!IsRun())
				return;

			Stop();
			Start();
		}
		catch(...)
		{
			throw "CEditUI::CTimer::SetDelay";
		}
	}

	//************************************
	// Method:    GetDelay
	// FullName:  CEditUI::CTimer::GetDelay
	// Access:    public 
	// Returns:   UINT
	// Qualifier:
	// Note:	  
	//************************************
	UINT CEditUI::CTimer::GetDelay()
	{
		try
		{
			return m_Delay;
		}
		catch(...)
		{
			throw "CEditUI::CTimer::GetDelay";
		}
	}

	//************************************
	// Method:    SetCurTickCount
	// FullName:  CEditUI::CTimer::SetCurTickCount
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Note:	  
	//************************************
	void CEditUI::CTimer::SetCurTickCount()
	{
		try
		{
			m_CurTickCount = ::timeGetTime();
		}
		catch(...)
		{
			throw "CEditUI::CTimer::SetCurTickCount";
		}
	}

	//************************************
	// Method:    CheckTickDaYuDelay
	// FullName:  CEditUI::CTimer::CheckTickDaYuDelay
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Note:	  
	//************************************
	bool CEditUI::CTimer::CheckTickDaYuDelay()
	{
		try
		{
			return ::timeGetTime() - m_CurTickCount > m_Delay;
		}
		catch(...)
		{
			throw "CEditUI::CTimer::CheckTickDaYuDelay";
		}
	}

	//************************************
	// Method:    IsRun
	// FullName:  CEditUI::CTimer::IsRun
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Note:	  
	//************************************
	bool CEditUI::CTimer::IsRun()
	{
		try
		{
			return mIsRun;
		}
		catch(...)
		{
			throw "CEditUI::CTimer::IsRun";
		}
	}

	//************************************
	// Method:    TimerProc
	// FullName:  CEditUI::CTimer::TimerProc
	// Access:    public static 
	// Returns:   void CALLBACK
	// Qualifier:
	// Parameter: UINT uTimerID
	// Parameter: UINT uMsg
	// Parameter: DWORD_PTR dwUser
	// Parameter: DWORD_PTR dw1
	// Parameter: DWORD_PTR dw2
	// Note:	  
	//************************************
	void CALLBACK CEditUI::CTimer::TimerProc( UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2 )
	{
		try
		{
			CEditUI::CTimer* pthis = (CEditUI::CTimer*)dwUser;

			if(pthis && pthis->pEditUI && pthis->CheckTickDaYuDelay() && _tcscmp(pthis->pEditUI->m_sCheckVal.GetData(),pthis->pEditUI->GetText().GetData()) != 0)
			{
				pthis->pEditUI->m_sCheckVal = pthis->pEditUI->GetText();
				pthis->pEditUI->GetManager()->SendNotify(pthis->pEditUI,_T("OnEditTimer"));
			}
		}
		catch(...)
		{
			throw "CEditUI::CTimer::TimerProc";
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CEditUI::CEditUI() : m_pWindow(NULL), m_uMaxChar(255), m_bReadOnly(false), 
		m_bPasswordMode(false), m_cPasswordChar(_T('*')), m_uButtonState(0), 
		m_dwEditbkColor(0xFFFFFFFF), m_iWindowStyls(0),m_Hwnd(0)
	{
		SetBorderSize(1);
		SetBorderColor(0xFFBAC0C5);
		SetTextPadding(CDuiRect(4, 3, 4, 3));
		SetBkColor(0xFFFFFFFF);
	}

	LPCTSTR CEditUI::GetClass() const
	{
		return _T("EditUI");
	}

	LPVOID CEditUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("Edit")) == 0 ) return static_cast<CEditUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	UINT CEditUI::GetControlFlags() const
	{
		if( !IsEnabled() ) return CControlUI::GetControlFlags();

		return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
	}

	void CEditUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETCURSOR && IsEnabled() )
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			return;
		}
		if( event.Type == UIEVENT_WINDOWSIZE )
		{
			if( m_pWindow != NULL ) m_pManager->SetFocusNeeded(this);
		}
		if( event.Type == UIEVENT_SCROLLWHEEL )
		{
			if( m_pWindow != NULL ) return;
		}
		if( event.Type == UIEVENT_SETFOCUS && IsEnabled() ) 
		{
			if( m_pWindow ) return;
			m_pWindow = new CEditWnd();
			ASSERT(m_pWindow);
			m_pWindow->Init(this);
			Invalidate();
		}
		if( event.Type == UIEVENT_KILLFOCUS && IsEnabled() ) 
		{
			if(m_RegularCheckStr.GetLength() > 0)
			{
				if(!MatchRegular(true))
					m_pManager->SendNotify(this,_T("OnEditRegex"),IDNO);
				else
					m_pManager->SendNotify(this,_T("OnEditRegex"),IDYES);

				m_pManager->SetFocus(this);

				return;
			}
			SetInternVisible(false);
			Invalidate();
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) 
		{
			if( IsEnabled() ) {
				GetManager()->ReleaseCapture();
				if( IsFocused() && m_pWindow == NULL )
				{
					m_pWindow = new CEditWnd();
					ASSERT(m_pWindow);
					m_pWindow->Init(this);

					if( PtInRect(&m_rcItem, event.ptMouse) )
					{
						int nSize = GetWindowTextLength(*m_pWindow);
						if( nSize == 0 )
							nSize = 1;

						Edit_SetSel(*m_pWindow, 0, nSize);
					}
				}
				else if( m_pWindow != NULL )
				{
#if 1
					int nSize = GetWindowTextLength(*m_pWindow);
					if( nSize == 0 )
						nSize = 1;

					Edit_SetSel(*m_pWindow, 0, nSize);
#else
					POINT pt = event.ptMouse;
					pt.x -= m_rcItem.left + m_rcTextPadding.left;
					pt.y -= m_rcItem.top + m_rcTextPadding.top;
					::SendMessage(*m_pWindow, WM_LBUTTONDOWN, event.wParam, MAKELPARAM(pt.x, pt.y));
#endif
				}
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE ) 
		{
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP ) 
		{
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		CLabelUI::DoEvent(event);
	}

	void CEditUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	void CEditUI::SetText(LPCTSTR pstrText)
	{
		m_sText = pstrText;
		if( m_pWindow != NULL ) Edit_SetText(*m_pWindow, m_sText);
		Invalidate();
	}
	
	void CEditUI::SetMaxChar(UINT uMax)
	{
		m_uMaxChar = uMax;
		if( m_pWindow != NULL ) Edit_LimitText(*m_pWindow, m_uMaxChar);
	}

	UINT CEditUI::GetMaxChar()
	{
		return m_uMaxChar;
	}

	void CEditUI::SetReadOnly(bool bReadOnly)
	{
		if( m_bReadOnly == bReadOnly ) return;

		m_bReadOnly = bReadOnly;
		if( m_pWindow != NULL ) Edit_SetReadOnly(*m_pWindow, m_bReadOnly);
		Invalidate();
	}

	bool CEditUI::IsReadOnly() const
	{
		return m_bReadOnly;
	}

	void CEditUI::SetNumberOnly(bool bNumberOnly)
	{
		if( bNumberOnly )
		{
			m_iWindowStyls |= ES_NUMBER;
		}
		else
		{
			m_iWindowStyls |= ~ES_NUMBER;
		}
	}

	bool CEditUI::IsNumberOnly() const
	{
		return m_iWindowStyls&ES_NUMBER ? true:false;
	}

	int CEditUI::GetWindowStyls() const 
	{
		return m_iWindowStyls;
	}

	void CEditUI::SetPasswordMode(bool bPasswordMode)
	{
		if( m_bPasswordMode == bPasswordMode ) return;
		m_bPasswordMode = bPasswordMode;
		Invalidate();
	}

	bool CEditUI::IsPasswordMode() const
	{
		return m_bPasswordMode;
	}

	void CEditUI::SetPasswordChar(TCHAR cPasswordChar)
	{
		if( m_cPasswordChar == cPasswordChar ) return;
		m_cPasswordChar = cPasswordChar;
		if( m_pWindow != NULL ) Edit_SetPasswordChar(*m_pWindow, m_cPasswordChar);
		Invalidate();
	}

	TCHAR CEditUI::GetPasswordChar() const
	{
		return m_cPasswordChar;
	}

	LPCTSTR CEditUI::GetNormalImage()
	{
		return m_sNormalImage;
	}

	void CEditUI::SetNormalImage(LPCTSTR pStrImage)
	{
		m_sNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CEditUI::GetHotImage()
	{
		return m_sHotImage;
	}

	void CEditUI::SetHotImage(LPCTSTR pStrImage)
	{
		m_sHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CEditUI::GetFocusedImage()
	{
		return m_sFocusedImage;
	}

	void CEditUI::SetFocusedImage(LPCTSTR pStrImage)
	{
		m_sFocusedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CEditUI::GetDisabledImage()
	{
		return m_sDisabledImage;
	}

	void CEditUI::SetDisabledImage(LPCTSTR pStrImage)
	{
		m_sDisabledImage = pStrImage;
		Invalidate();
	}

	void CEditUI::SetNativeEditBkColor(DWORD dwBkColor)
	{
		m_dwEditbkColor = dwBkColor;
	}

	DWORD CEditUI::GetNativeEditBkColor() const
	{
		return m_dwEditbkColor;
	}

	void CEditUI::SetSel(long nStartChar, long nEndChar)
	{
		if( m_pWindow != NULL ) Edit_SetSel(*m_pWindow, nStartChar,nEndChar);
	}

	void CEditUI::SetSelAll()
	{
		SetSel(0,-1);
	}

	void CEditUI::SetReplaceSel(LPCTSTR lpszReplace)
	{
		if( m_pWindow != NULL ) Edit_ReplaceSel(*m_pWindow, lpszReplace);
	}

	//************************************
	// Method:    MatchRegular
	// FullName:  CEditUI::MatchRegular
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Note:	  
	//************************************
	bool CEditUI::MatchRegular(bool isShowMsg/* = true*/)
	{
		try
		{
			if(!m_RegularCheckStr.GetLength())
				return true;
			
			try
			{
#ifndef _UNICODE
				wchar_t* mSrcRegularCheck = new wchar_t[_tclen(GetRegularCheck())*2+1]();
				wsprintfW(mSrcRegularCheck,L"%s",GetRegularCheck());
				std::tr1::wregex regExpress(mSrcRegularCheck);

				wchar_t* mSrcVal = new wchar_t[GetText().GetLength()*2+1]();
				wsprintfW(mSrcVal,L"%s",GetText().GetData());
				std::wstring mSrcText = mSrcVal;

				delete mSrcRegularCheck;
				mSrcRegularCheck = NULL;
				delete mSrcVal;
				mSrcVal = NULL;

				if(!regex_match(mSrcText,regExpress))
					goto MatchFailed;
				else
					return true;
#else
				char* mSrcRegularCheck = new char[strlen(CW2A(GetRegularCheck()))*2+1]();
				sprintf(mSrcRegularCheck,"%s",CW2A(GetRegularCheck()));
				std::tr1::regex regExpress(mSrcRegularCheck);

				char* mSrcVal = new char[GetText().GetLength()*2+1]();
				sprintf(mSrcVal,"%s",CW2A(GetText().GetData()));
				std::string mSrcText = mSrcVal;

				delete mSrcRegularCheck;
				mSrcRegularCheck = NULL;
				delete mSrcVal;
				mSrcVal = NULL;

				if(!regex_match(mSrcText,regExpress))
					goto MatchFailed;
				else
					return true;
#endif
			}
			catch(...)
			{
				goto MatchFailed;
			}

		}
		catch (...)
		{
			throw "CEditUI::MatchRegular";
		}
MatchFailed:
		{
			if(m_RegularTipStr.GetLength() > 0 && isShowMsg)
				MessageBox(GetManager()->GetPaintWindow(),m_RegularTipStr.GetData(),NULL,MB_OK);

			SetText(m_RegluarSrcText.GetData());
			return false;
		}
	}

	//************************************
	// Method:    SetRegularCheck
	// FullName:  CEditUI::SetRegularCheck
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR pRegularCheckStr
	// Note:	  
	//************************************
	void CEditUI::SetRegularCheck( LPCTSTR pRegularCheckStr )
	{
		try
		{
			m_RegularCheckStr = pRegularCheckStr;
			Invalidate();
		}
		catch(...)
		{
			throw "CEditUI::SetRegularCheck";
		}
	}

	//************************************
	// Method:    GetRegularCheck
	// FullName:  CEditUI::GetRegularCheck
	// Access:    public 
	// Returns:   LPCTSTR
	// Qualifier:
	// Note:	  
	//************************************
	LPCTSTR CEditUI::GetRegularCheck()
	{
		try
		{
			return m_RegularCheckStr;
		}
		catch(...)
		{
			throw "CEditUI::GetRegularCheck";
		}
	}

	//************************************
	// Method:    SetRegularTip
	// FullName:  CEditUI::SetRegularTip
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR pRegularTipStr
	// Note:	  
	//************************************
	void CEditUI::SetRegularTip( LPCTSTR pRegularTipStr )
	{
		try
		{
			m_RegularTipStr = pRegularTipStr;
			Invalidate();
		}
		catch(...)
		{
			throw "CEditUI::SetRegularTip";
		}
	}

	//************************************
	// Method:    GetRegularTip
	// FullName:  CEditUI::GetRegularTip
	// Access:    public 
	// Returns:   LPCTSTR
	// Qualifier:
	// Note:	  
	//************************************
	LPCTSTR CEditUI::GetRegularTip()
	{
		try
		{
			return m_RegularTipStr;
		}
		catch(...)
		{
			throw "CEditUI::GetRegularTip";
		}
	}

	//************************************
	// Method:    SetMatchCase
	// FullName:  CEditUI::SetMatchCase
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: bool bMatchCase
	// Note:	  
	//************************************
	void CEditUI::SetMatchCase( bool bMatchCase /*= false*/ )
	{
		try
		{
			m_bMatchCase = bMatchCase;
		}
		catch(...)
		{
			throw "CEditUI::SetMatchCase";
		}
	}

	//************************************
	// Method:    GetMatchCase
	// FullName:  CEditUI::GetMatchCase
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Note:	  
	//************************************
	bool CEditUI::GetMatchCase()
	{
		try
		{
			return m_bMatchCase;
		}
		catch(...)
		{
			throw "CEditUI::GetMatchCase";
		}
	}

	//************************************
	// Method:    SetTipValue
	// FullName:  CEditUI::SetTipValue
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR pStrTipValue
	// Note:	  
	//************************************
	void CEditUI::SetTipValue( LPCTSTR pStrTipValue )
	{
		try
		{
			m_sSrcTipValue	= pStrTipValue;
			m_sTipValue		= CDuiString(_T("__IsTipValue__"))+pStrTipValue;
		}
		catch(...)
		{
			throw "CEditUI::SetTipValue";
		}
	}

	//************************************
	// Method:    SetTipValueColor
	// FullName:  CEditUI::SetTipValueColor
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: LPCTSTR pStrColor
	// Note:	  
	//************************************
	void CEditUI::SetTipValueColor( LPCTSTR pStrColor )
	{
		try
		{
			if( *pStrColor == _T('#')) pStrColor = ::CharNext(pStrColor);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pStrColor, &pstr, 16);

			m_sTipValueColor = clrColor;
		}
		catch(...)
		{
			throw "CEditUI::SetTipValueColor";
		}
	}

	//************************************
	// Method:    GetTipValueColor
	// FullName:  CEditUI::GetTipValueColor
	// Access:    public 
	// Returns:   DWORD
	// Qualifier:
	// Note:	  
	//************************************
	DWORD CEditUI::GetTipValueColor()
	{
		try
		{
			return m_sTipValueColor;
		}
		catch(...)
		{
			throw "CEditUI::GetTipValueColor";
		}
	}

	//************************************
	// Method:    GetTipValue
	// FullName:  CEditUI::GetTipValue
	// Access:    public 
	// Returns:   UiLib::CDuiString
	// Qualifier:
	// Note:	  
	//************************************
	UiLib::CDuiString CEditUI::GetTipValue()
	{
		try
		{
			return m_sTipValue;
		}
		catch(...)
		{
			throw "CEditUI::GetTipValue";
		}
	}

	//************************************
	// Method:    GetSrcTipValue
	// FullName:  CEditUI::GetSrcTipValue
	// Access:    public 
	// Returns:   LPCTSTR
	// Qualifier:
	// Note:	  
	//************************************
	LPCTSTR CEditUI::GetSrcTipValue()
	{
		try
		{
			return m_sSrcTipValue.GetData();
		}
		catch(...)
		{
			throw "CEditUI::GetSrcTipValue";
		}
	}

	void CEditUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		if( m_pWindow != NULL ) {
			RECT rcPos = m_pWindow->CalPos();
			::SetWindowPos(m_pWindow->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, 
				rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);        
		}
	}

	void CEditUI::SetVisible(bool bVisible)
	{
		CControlUI::SetVisible(bVisible);
		if( !IsVisible() && m_pWindow != NULL ) m_pManager->SetFocus(NULL);
	}

	void CEditUI::SetInternVisible(bool bVisible)
	{
		if( !IsVisible() && m_pWindow != NULL ) m_pManager->SetFocus(NULL);
	}

	SIZE CEditUI::EstimateSize(SIZE szAvailable)
	{
		if( m_cxyFixed.cy == 0 ) return CSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 6);
		return CControlUI::EstimateSize(szAvailable);
	}

	void CEditUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("readonly")) == 0 ) SetReadOnly(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("numberonly")) == 0 ) SetNumberOnly(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("password")) == 0 ) SetPasswordMode(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("maxchar")) == 0 ) SetMaxChar(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("tipvalue")) == 0 ) SetTipValue(pstrValue);
		else if( _tcscmp(pstrName, _T("tipvaluecolor")) == 0 ) SetTipValueColor(pstrValue);
		else if( _tcscmp(pstrName, _T("enabletimer")) == 0) SetEnableTimer(_tcscmp(pstrValue,_T("true")) == 0);
		else if( _tcscmp(pstrName, _T("timerdelay")) == 0) SetTimerDelay(_tcstoul(pstrValue,NULL,10));
		else if( _tcscmp(pstrName, _T("regularcheck")) == 0) SetRegularCheck(pstrValue);
		else if( _tcscmp(pstrName, _T("regulartip")) == 0) SetRegularTip(pstrValue);
		else if( _tcscmp(pstrName, _T("nativebkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetNativeEditBkColor(clrColor);
		}
		else CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CEditUI::PaintStatusImage(HDC hDC)
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		if( (m_uButtonState & UISTATE_DISABLED) != 0 ) {
			if( !m_sDisabledImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sDisabledImage) ) m_sDisabledImage.Empty();
				else return;
			}
		}
		else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
			if( !m_sFocusedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sFocusedImage) ) m_sFocusedImage.Empty();
				else return;
			}
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( !m_sHotImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sHotImage) ) m_sHotImage.Empty();
				else return;
			}
		}

		if( !m_sNormalImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)m_sNormalImage) ) m_sNormalImage.Empty();
			else return;
		}
	}

	void CEditUI::PaintText(HDC hDC)
	{
		DWORD mCurTextColor = m_dwTextColor;

		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if(GetText() == m_sSrcTipValue)	mCurTextColor = m_sTipValueColor;
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		if( m_sText.IsEmpty() ) return;

		CDuiString sText = m_sText;

		if( m_bPasswordMode ) {
			sText.Empty();
			LPCTSTR p = m_sText.GetData();
			while( *p != _T('\0') ) {
				sText += m_cPasswordChar;
				p = ::CharNext(p);
			}
		}

		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;
		if( IsEnabled() ) {
			CRenderEngine::DrawText(hDC, m_pManager, rc, sText, mCurTextColor, \
				m_iFont, DT_SINGLELINE | m_uTextStyle);
		}
		else {
			CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, \
				m_iFont, DT_SINGLELINE | m_uTextStyle);
		}
	}

	//************************************
	// Method:    SetEnableTimer
	// FullName:  CEditUI::SetEnableTimer
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: bool bEnableTime
	// Note:	  
	//************************************
	void CEditUI::SetEnableTimer( bool bEnableTime )
	{
		try
		{
			m_bEnableTime = bEnableTime;
		}
		catch(...)
		{
			throw "CEditUI::SetEnableTimer";
		}
	}

	//************************************
	// Method:    GetEnableTimer
	// FullName:  CEditUI::GetEnableTimer
	// Access:    public 
	// Returns:   bool
	// Qualifier:
	// Note:	  
	//************************************
	bool CEditUI::GetEnableTimer()
	{
		try
		{
			return m_bEnableTime;
		}
		catch(...)
		{
			throw "CEditUI::GetEnableTimer";
		}
	}

	//************************************
	// Method:    SetTimerDelay
	// FullName:  CEditUI::SetTimerDelay
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: UINT nDelay
	// Note:	  
	//************************************
	void CEditUI::SetTimerDelay( UINT nDelay )
	{
		try
		{
			m_Timer.SetDelay(nDelay);
		}
		catch(...)
		{
			throw "CEditUI::SetTimerDelay";
		}
	}

	//************************************
	// Method:    GetTimerObj
	// FullName:  CEditUI::GetTimerObj
	// Access:    public 
	// Returns:   CTimer*
	// Qualifier:
	// Note:	  
	//************************************
	CEditUI::CTimer* CEditUI::GetTimerObj()
	{
		try
		{
			return &m_Timer;
		}
		catch(...)
		{
			throw "CEditUI::GetTimerObj";
		}
	}
}
