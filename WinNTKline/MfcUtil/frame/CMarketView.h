#pragma once

// CMarketView ������ͼ

class CMarketView : public CFormView
{
	DECLARE_DYNCREATE(CMarketView)

protected:
	CMarketView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CMarketView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnInitialUpdate();
	DECLARE_MESSAGE_MAP()
};


