#pragma once

// CMfcKView ������ͼ

class CMfcKView : public CFormView
{
	DECLARE_DYNCREATE(CMfcKView)

protected:
	CMfcKView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CMfcKView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCKLINE };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	virtual void OnInitialUpdate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};


