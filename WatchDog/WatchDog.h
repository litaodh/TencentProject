
// WatchDog.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CWatchDogApp: 
// �йش����ʵ�֣������ WatchDog.cpp
//

class CWatchDogApp : public CWinApp
{
public:
	CWatchDogApp();

// ��д
public:
	virtual BOOL InitInstance();
	bool IsUniqueCopyInProc();
	HANDLE m_Mutex;
// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CWatchDogApp theApp;