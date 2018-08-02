// IMlogDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCKline.h"
#include "IMlogDlg.h"
#include "afxdialogex.h"

// IMlogDlg 对话�?

IMPLEMENT_DYNAMIC(IMlogDlg, CDialogEx)

IMlogDlg::IMlogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_IMMODAL, pParent) {}

IMlogDlg::~IMlogDlg()
{
}

void IMlogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ACNT, m_editUsr);
	DDX_Control(pDX, IDC_PSW, m_editPsw);
}

BEGIN_MESSAGE_MAP(IMlogDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &IMlogDlg::OnBnClickedOk)
END_MESSAGE_MAP()



void IMlogDlg::PostNcDestroy()
{
}

BOOL IMlogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_editUsr.SetLimitText(20);
	m_editPsw.SetLimitText(20);
	return 0;
}

bool checkText(CString str)
{
	int z0 = 0;
	int zz = 0;
	int zZ = 0;
	int z_ = 0;
	for (int i = 0; i < str.GetLength(); i++)
	{
		char ansi = str[i];
		if (ansi <= '9' && ansi >= '0')
		{
			z0 = 1;
		}
		else if (ansi <= 'z' && ansi >= 'a')
		{
			zz = 1;
		}
		else if (ansi <= 'Z' && ansi >= 'A')
		{
			zZ = 1;
		}
		else if (ansi > 127)
		{
			z_ = 0;
		}
		else
		{
			z_ = 1;
		}
	}
	return (z0 + zz + zZ + z_ == 4);
}

void IMlogDlg::OnBnClickedOk()
{
	GetDlgItem(IDC_ACNT)->GetWindowText(m_strAcnt);
	GetDlgItem(IDC_PSW)->GetWindowText(m_strPsw);
	if (lstrlen(m_strAcnt) < 3) 
	{
		AfxMessageBox("������3~20���ַ���");
	}else
		if (!checkText(m_strPsw)) 
			AfxMessageBox("������������֡���ĸ�������ַ��ļ��ϣ�");
	if (SetLogInfo((LPSTR)(LPCSTR)m_strAcnt, (LPSTR)(LPCSTR)m_strPsw))
		CDialogEx::OnOK();
	SetStatus(0);
	//GetDlgItem(IDC_LISTFRND)->ShowWindow(SW_SHOW);
}
