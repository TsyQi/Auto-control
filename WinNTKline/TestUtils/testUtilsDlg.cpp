#include "stdafx.h"
#include "testUtils.h"
#include "testUtilsDlg.h"
#include "afxdialogex.h"
#include <Resource.h>
#include <soapH.h>
#include "MyOglDrawDlg.h"
#include "LoginDlg.h"
#include "IMhideWndDlg.h"
#include "html\CWebBrowser2.h"
#include "IM/client.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define LOOP_TIME 1

namespace {
    StSock socks = {};
    CLoginDlg login = {};
    CTPclient* g_ctp = nullptr;
    MyOglDrawDlg* g_ogl = nullptr;
    CIMhideWndDlg* g_pIM = nullptr;
};

IMPLEMENT_DYNCREATE(CWebBrowser2, CWnd)

CtestUtilsDlg::CtestUtilsDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_TEST_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CtestUtilsDlg::~CtestUtilsDlg()
{
    LeakCheck(g_ogl);
    LeakCheck(g_ctp);
    LeakCheck(g_pIM);
}

void CtestUtilsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_IPCTRL, m_ipAddr);
    DDX_Control(pDX, IDC_MPORT, m_Port);
}

BEGIN_MESSAGE_MAP(CtestUtilsDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_TOIM, &CtestUtilsDlg::OnBnClickedToim)
    ON_BN_CLICKED(IDC_REGIDST, &CtestUtilsDlg::OnBnClickedRegist)
    ON_BN_CLICKED(IDC_TESTLOG, &CtestUtilsDlg::OnBnClickedTestlog)
    ON_BN_CLICKED(IDC_KLINE, &CtestUtilsDlg::OnBnClickedKline)
    ON_BN_CLICKED(IDC_CTP, &CtestUtilsDlg::OnBnClickedCtp)
    ON_BN_CLICKED(IDC_SIMBTN, &CtestUtilsDlg::OnBnClickedSimbtn)
    ON_BN_CLICKED(IDC_IMSER, &CtestUtilsDlg::OnBnClickedImser)
    ON_BN_CLICKED(IDOK, &CtestUtilsDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CtestUtilsDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_QTCASE, &CtestUtilsDlg::OnBnClickedQtcase)
    ON_STN_CLICKED(IDC_IP_TEXT, &CtestUtilsDlg::OnStnClickedIpText)
END_MESSAGE_MAP()

BOOL CtestUtilsDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    m_ipAddr.GetWindowText(a_IP, 16);
    if (a_IP[0] == '\0' || a_IP[0] == '\x30') {
        sprintf_s(a_IP, 16, "192.168.137.7");
        m_ipAddr.SetAddress(ntohl(inet_addr(a_IP)));
    }

    m_Port.GetWindowText(a_Port);
    if (a_Port.IsEmpty()) {
        a_Port = "8080";
        m_Port.SetWindowText(a_Port);
    }

    return TRUE;
}

void CtestUtilsDlg::OnPaint()
{
    if (IsIconic()) {
        CPaintDC dc(this);

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    } else {
        CDialogEx::OnPaint();
    }
}

HCURSOR CtestUtilsDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CtestUtilsDlg::OnBnClickedToim()
{
    m_ipAddr.GetWindowText(a_IP, 16);
    memcpy(socks.IP, a_IP, 16);
    g_pIM = new CIMhideWndDlg(&socks);
    g_pIM->Create(IDD_IMHIDEWND);
    g_pIM->ShowWindow(SW_SHOWNORMAL);
}

void CtestUtilsDlg::OnBnClickedRegist()
{
    m_ipAddr.GetWindowText(a_IP, 16);
    login.testRegist(a_IP);
}

void CtestUtilsDlg::OnBnClickedTestlog()
{
    m_Port.GetWindowText(a_Port);
    m_ipAddr.GetWindowText(a_IP, 16);
    sprintf_s(socks.addr, 64, "http://%s:%d/myweb.cgi", a_IP, atoi(a_Port));
    sprintf_s(socks.form, 28, "trans@&acc=local&test=8888");
    for (int i = 0; i < LOOP_TIME; i++) {
        login.testLogin(socks);
        Sleep(10);
    }
}

void CtestUtilsDlg::OnBnClickedKline()
{
    g_ogl = new MyOglDrawDlg();
    g_ogl->Create(IDD_OGLIMG);
    g_ogl->ShowWindow(SW_SHOWNORMAL);
}

void CtestUtilsDlg::OnBnClickedCtp()
{
    g_ctp = new CTPclient();
    CloseHandle((HANDLE)_beginthreadex(NULL, 0, g_ctp->TradeMarket, (void*)this, 0, NULL));
}

void CtestUtilsDlg::OnBnClickedSimbtn()
{
    if (AllocConsole()) {
        if (freopen("CONOUT$", "w", stderr) != nullptr)
            Simulation();
    }
}

void CtestUtilsDlg::OnBnClickedImser()
{
    STARTUPINFO sInfo;
    PROCESS_INFORMATION prInfo;
    ZeroMemory(&prInfo, sizeof(prInfo));
    ZeroMemory(&sInfo, sizeof(sInfo));
    sInfo.cb = sizeof(sInfo);
    sInfo.dwFlags = STARTF_USESHOWWINDOW;
    sInfo.wShowWindow = SW_SHOWNORMAL;
    CString filename = _T("..\\Debug\\IM(Win32).exe");
    WIN32_FIND_DATA findFileData;
    if (FindFirstFile(filename, &findFileData) == INVALID_HANDLE_VALUE) {
        MessageBox(filename + "\nProcess 'IM' NOT found!");
        return;
    }
    ::CreateProcess(filename, _T("1"), NULL, NULL, false, 0, NULL, NULL, &sInfo, &prInfo);
}

void CtestUtilsDlg::OnBnClickedOk()
{
    CDialog::OnOK();
}

void CtestUtilsDlg::OnBnClickedCancel()
{
    OnBnClickedOk();
}

void CtestUtilsDlg::OnBnClickedQtcase()
{
    STARTUPINFO sInfo;
    PROCESS_INFORMATION prInfo;
    ZeroMemory(&prInfo, sizeof(prInfo));
    ZeroMemory(&sInfo, sizeof(sInfo));
    sInfo.cb = sizeof(sInfo);
    sInfo.dwFlags = STARTF_USESHOWWINDOW;
    sInfo.wShowWindow = SW_SHOWNORMAL;
    CString filename = _T("..\\Debug\\QtGames.exe");
    WIN32_FIND_DATA findFileData;
    if (FindFirstFile(filename, &findFileData) == INVALID_HANDLE_VALUE) {
        MessageBox(filename + "\nNo such exec file!");
    } else {
        ::CreateProcess(filename, _T("1"), NULL, NULL, false, 0, NULL, NULL, &sInfo, &prInfo);
    }
}


void CtestUtilsDlg::OnStnClickedIpText()
{
    sprintf_s(a_IP, 16, "127.0.0.1");
    m_ipAddr.SetAddress(ntohl(inet_addr(a_IP)));
}
