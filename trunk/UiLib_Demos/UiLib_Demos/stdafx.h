// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"
#include "resource.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <atlbase.h>
#include <atlstr.h>
#include <limits>

#define _CRT_SECURE_NO_DEPRECATE
// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

#include "../../Common/Include/UiLib/UIlib.h"

#ifdef _VC2008
#ifdef _DEBUG
#ifdef _LIB
#pragma comment(lib, "../../Common/Static_Debug_2008/UiLib_d.lib")
#else
#pragma comment(lib, "../../Common/DLL_Debug_2008/UiLib_d.lib")
#endif
#else
#ifdef _LIB
#pragma comment(lib, "../../Common/Static_Release_2008/UiLib.lib")
#else
#pragma comment(lib, "../../Common/DLL_Release_2008/UiLib.lib")
#endif
#endif
#endif

#ifdef _VC201X
#ifdef _DEBUG
#ifdef _LIB
#pragma comment(lib, "../../Common/Static_Debug_201X/UiLib_d.lib")
#else
#pragma comment(lib, "../../Common/DLL_Debug_201X/UiLib_d.lib")
#endif
#else
#ifdef _LIB
#pragma comment(lib, "../../Common/Static_Release_201X/UiLib.lib")
#else
#pragma comment(lib, "../../Common/DLL_Release_201X/UiLib.lib")
#endif
#endif
#endif