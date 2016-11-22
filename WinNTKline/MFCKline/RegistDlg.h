#pragma once

#include "ctl/CWebBrowser2.h"
#include "afxdialogex.h"

// CRegist �Ի���

class CRegistDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRegistDlg)

public:
	CRegistDlg(char* ip, CWnd* pParent = NULL);   // ��׼���캯��
	virtual BOOL OnInitDialog();
	virtual ~CRegistDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BROWSER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CWebBrowser2 m_browser;
	char ip[32];
private:
	void Init();
};
