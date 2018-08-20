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

IMlogDlg::IMlogDlg(int(*func)(char *, char *))
{
	setUsrPsw = func;
}

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

void IMlogDlg::OnBnClickedOk()
{
	GetDlgItem(IDC_ACNT)->GetWindowText(m_strAcnt);
	GetDlgItem(IDC_PSW)->GetWindowText(m_strPsw);
	if (lstrlen(m_strAcnt) < 3) 
	{
		AfxMessageBox("������3~20���ַ���");
		return;
	} else
		if (!checkPswVilid((LPSTR)(LPCSTR)m_strPsw)) {
			AfxMessageBox("������������֡���ĸ�������ַ��ļ��ϣ�");
			return;
		}
	if (setUsrPsw((LPSTR)(LPCSTR)m_strAcnt, (LPSTR)(LPCSTR)m_strPsw))
		//GetDlgItem(IDC_LISTFRND)->ShowWindow(SW_SHOW);
		CDialogEx::OnOK();
}
