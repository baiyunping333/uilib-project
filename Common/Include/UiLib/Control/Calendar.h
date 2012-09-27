#ifndef Calendar_h__
#define Calendar_h__

#include <map>
#pragma once

namespace UiLib
{
	typedef struct tag_CalendarStyle
	{
		CDuiString	nCalenderBorderColor;		//�����߿���ɫ
		CDuiString	nCalendarBkColor;			//����������ɫ
		CDuiString	nMainTitleBkColor;			//���������ⱳ����ɫ
		CDuiString	nSubTitleBkColor;			//���������ⱳ����ɫ
		CDuiString	nWeekendColorA;				//��ĩ�����ڸ��б�����ɫ
		CDuiString	nWeekendColorB;				//��ĩ�����ڸ��б�����ɫ
		CDuiString	nToDayColor;				//���ڵ�ǰ��ı�����ɫ
		CDuiString	nDayHotColor;				//���ڻ�ý���ʱ������ɫ
		CDuiString	nDayPushedColor;			//���ڱ�����ʱ������ɫ
		CDuiString	nDaySelectColor;			//���ڱ�ѡ��ʱ������ɫ
		CDuiString	nDayDisabledColor;			//���ڱ�����ʱ�ı���ɫ
		CDuiString	nNoCurMonthDayColor;		//�Ǳ������ڵı�����ɫ
		CDuiString	nWeekIntervalColorA;		//�ܸ�����ɫA
		CDuiString	nWeekIntervalColorB;		//�ܸ�����ɫB
	}TCalendarStyle;
	
	typedef struct tag_SubTitleString
	{
		CDuiString	nSundayStr;
		CDuiString	nMondayStr;
		CDuiString	nTuesdayStr;
		CDuiString	nWednesdayStr;
		CDuiString	nThursdayStr;
		CDuiString	nFridayStr;
		CDuiString	nSaturdayStr;
	}TSubTitleString;

	class CCalendar : public CHorizontalLayoutUI
	{
	public:
		CCalendar(void);
		~CCalendar(void);

	public:
		bool AddSubTitleString(LPCTSTR _Name,TSubTitleString& _SubTitleString);
		bool AddSubTitleString(LPCTSTR _Name,LPCTSTR _Sunday,LPCTSTR _Monday,LPCTSTR _Tuesday,LPCTSTR _Wednesday,LPCTSTR _Thursday,LPCTSTR _Friday,LPCTSTR _Saturday);
		TSubTitleString& GetSubTitleString(LPCTSTR _Name = NULL);
		bool RemoveAtSubTitleString(LPCTSTR _Name);
	public:
		TCalendarStyle	m_DefaultStyle;

	private:
		CHorizontalLayoutUI*	pMainTitleHoriz;
		CHorizontalLayoutUI*	pSubTitleHoriz;
		CVerticalLayoutUI*		pDayPanelVert;

		struct tm*				pCurDateTime;

		TSubTitleString			mSubTitleString;
		CStdStringPtrMap		mSubTitleStringArray;
	};
}



#endif // Calendar_h__
