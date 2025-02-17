#pragma once
#include "IM/client.h"

// IMlogDlg 对话框

class IMlogDlg : public CDialogEx {
    DECLARE_DYNAMIC(IMlogDlg)

public:
    IMlogDlg(CWnd* pParent = NULL);   // 标准构造函数
    explicit IMlogDlg(int(*func)(char*, char*));
    virtual ~IMlogDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_IMMODAL };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
protected:
    virtual void PostNcDestroy();
    virtual BOOL OnInitDialog();
public:
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedOk();
    virtual BOOL Create(UINT Tmp, CWnd* Wnd);
    bool getVision();
    void setVision(bool canBSee);
    char* getUsername();
    CEdit m_editUsr, m_editPsw;
private:
    CString m_strAccnt, m_strPsw;
    int(*setUsrPsw)(char*, char*);
    bool m_canBSee = 0;
};
