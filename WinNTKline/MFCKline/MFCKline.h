
// MFCKline.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once
#include "stdafx.h"

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFCKlineApp: 
// �йش����ʵ�֣������ MFCKline.cpp
//

class CMFCKlineApp : public CWinApp
{
public:
	CMFCKlineApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMFCKlineApp theApp;
