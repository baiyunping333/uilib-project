#include "StdAfx.h"
#include "DuiHardwareInfo.h"

namespace UiLib
{
	//************************************
	// ��������: GetMonitorCount
	// ��������: DWORD
	// ������Ϣ: bool _bReCheck
	// ����˵��: 
	//************************************
	DWORD DuiMonitor::GetMonitorCount(bool _bReCheck/* = false*/)
	{
		static DWORD mMonitorCount = 0;

		if(mMonitorCount && !_bReCheck)
			return mMonitorCount;

		DEVMODE mDevMode;
		memset(&mDevMode,0, sizeof(mDevMode));
		DISPLAY_DEVICE mDisplayDevice;
		DWORD mCheckCount = 0;

		while(true)
		{
			memset(&mDisplayDevice,0, sizeof(mDisplayDevice));
			mDisplayDevice.cb = sizeof(DISPLAY_DEVICE);
			if(EnumDisplayDevices(NULL, mCheckCount, &mDisplayDevice,0) == FALSE)
				break;

			if(EnumDisplaySettings(mDisplayDevice.DeviceName,ENUM_CURRENT_SETTINGS,&mDevMode))
				mMonitorCount++;

			mCheckCount++;
		}

		return mMonitorCount;
	}

	//************************************
	// ��������: GetMainMonitorIndex
	// ��������: DWORD
	// ����˵��: 
	//************************************
	DWORD DuiMonitor::GetMainMonitorIndex()
	{
		DWORD nMonitorCount = GetMonitorCount();
		for(DWORD nMonitorIndex = 1;nMonitorIndex <= nMonitorCount;nMonitorIndex++)
		{
			POINTL nPoint = GetMonitorXY(nMonitorIndex);
			if(0 == nPoint.x && 0 == nPoint.y)
				return nMonitorIndex;
		}
		return 1;
	}

	//************************************
	// ��������: GetMonitorXY
	// ��������: POINTL
	// ������Ϣ: DWORD _MonitorIndex
	// ����˵��: 
	//************************************
	POINTL DuiMonitor::GetMonitorXY( DWORD _MonitorIndex /*= 1*/ )
	{
		if(_MonitorIndex <= 0 || _MonitorIndex > GetMonitorCount())
			return POINTL();

		DEVMODE& mDevMode = GetMonitorDevMode(_MonitorIndex);
		if(!mDevMode.dmDriverVersion)
			return POINTL();

		return mDevMode.dmPosition;
	}

	//************************************
	// ��������: GetMonitorSize
	// ��������: SIZE
	// ������Ϣ: DWORD _MonitorIndex
	// ����˵��: 
	//************************************
	SIZE DuiMonitor::GetMonitorSize( DWORD _MonitorIndex /*= 1*/ )
	{
		if(_MonitorIndex <= 0 || _MonitorIndex > GetMonitorCount())
			return SIZE();

		DEVMODE& mDevMode = GetMonitorDevMode(_MonitorIndex);
		if(!mDevMode.dmDriverVersion)
			return SIZE();
		
		SIZE mSize = { mDevMode.dmPelsWidth, mDevMode.dmPelsHeight};
		return mSize;
	}

	//************************************
	// ��������: GetMonitorDisplayDevice
	// ��������: DISPLAY_DEVICE&
	// ������Ϣ: DWORD _MonitorIndex
	// ����˵��: 
	//************************************
	DISPLAY_DEVICE& DuiMonitor::GetMonitorDisplayDevice( DWORD _MonitorIndex /*= 1*/ )
	{
		static DISPLAY_DEVICE mDisplayDevice;
		memset(&mDisplayDevice,0, sizeof(mDisplayDevice));

		if(_MonitorIndex <= 0 || _MonitorIndex > GetMonitorCount())
			return mDisplayDevice;

		DEVMODE mDevMode;
		memset(&mDevMode,0, sizeof(mDevMode));
		DWORD mCheckCount = 0,mMonitorCount = 0;

		while(mMonitorCount != _MonitorIndex)
		{
			mDisplayDevice.cb = sizeof(DISPLAY_DEVICE);
			if(EnumDisplayDevices(NULL, mCheckCount, &mDisplayDevice,0) == FALSE)
				break;

			if(EnumDisplaySettings(mDisplayDevice.DeviceName,ENUM_CURRENT_SETTINGS,&mDevMode))
				mMonitorCount++;
			
			mCheckCount++;
		}

		return mDisplayDevice;
	}

	//************************************
	// ��������: GetMonitorDevMode
	// ��������: DEVMODE&
	// ������Ϣ: DWORD _MonitorIndex
	// ����˵��: 
	//************************************
	DEVMODE& DuiMonitor::GetMonitorDevMode( DWORD _MonitorIndex /*= 1*/ )
	{
		static DEVMODE mDevMode;
		memset(&mDevMode,0, sizeof(mDevMode));

		if(_MonitorIndex <= 0 || _MonitorIndex > GetMonitorCount())
			return mDevMode;

		mDevMode.dmSize = sizeof(mDevMode);

		DISPLAY_DEVICE& mDisplayDevice = GetMonitorDisplayDevice(_MonitorIndex);
		mDisplayDevice.cb = sizeof(DISPLAY_DEVICE);

		if(EnumDisplaySettings(mDisplayDevice.DeviceName,ENUM_CURRENT_SETTINGS,&mDevMode))
			return mDevMode;

		memset(&mDevMode,0, sizeof(mDevMode));
		return mDevMode;
	}

}