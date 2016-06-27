
// MFCKlineDlg.h : ͷ�ļ�
//

#pragma once
#include "MyOglDrawDlg.h"
#include "Wpf/WpfHost.h"

// CMFCKlineDlg �Ի���
class CMFCKlineDlg : public CDialogEx
{
// ����
public:
	CMFCKlineDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CMFCKlineDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCKLINE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOgl();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnItemMsg(WPARAM wParam, LPARAM lParam);
public:
	CListCtrl m_list;
	MyOglDrawDlg m_Mod;
	OGLKview m_Ogl;
private:
	void SetList();
	void InitMap();
	void SetBottom();
	bool OpenGL(int item);
	void OpenWpf();
	void OpenQt();
	int OpenQtexe();
private:
	HWND m_hBottom;
};
