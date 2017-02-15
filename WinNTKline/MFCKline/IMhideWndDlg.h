#pragma once
#include "IM/IMClient.h"
#include "afxwin.h"

// CIMhideWndDlg �Ի���

class CIMhideWndDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CIMhideWndDlg)

public:
	CIMhideWndDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CIMhideWndDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMHIDEWND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	//{{AFX_MSG(CIMhideWndDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	//�����ƶ�ʱ���ڵĴ�С
	void FixMoving(UINT fwSide, LPRECT pRect);
	//������״̬��ʾ����
	void DoShow();
	//����ʾ״̬��������
	void DoHide();
	//���غ���,ֻ��Ϊ�˷������
	BOOL SetWindowPos(const CWnd* pWndInsertAfter,
		LPCRECT pCRect, UINT nFlags = SWP_SHOWWINDOW);
	afx_msg void OnSysCommand(UINT nID, LPARAM lparam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_isSizeChanged;   //���ڴ�С�Ƿ�ı��� 
	BOOL m_isSetTimer;      //�Ƿ������˼������Timer

	INT  m_oldWndHeight;    //�ɵĴ��ڿ��
	INT  m_taskBarHeight;   //�������߶�
	INT  m_edgeHeight;      //��Ե�߶�
	INT  m_edgeWidth;       //��Ե���

	INT  m_hideMode;        //����ģʽ
	BOOL m_hsFinished;      //���ػ���ʾ�����Ƿ����
	BOOL m_hiding;          //�ò���ֻ����!m_hsFinished����Ч
							//��:��������,��:������ʾ
public:
	CStatic m_timeStatus;
};
