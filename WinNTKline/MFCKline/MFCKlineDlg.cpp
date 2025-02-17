// MFCKlineDlg.cpp : 实现文件
//
#include "MFCKline.h"
#include "MFCKlineDlg.h"
#include "afxdialogex.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  WINDOW_TEXT_LENGTH MAX_PATH

struct ProcessWindow
{
    DWORD dwProcessId;
    HWND hwndWindow = NULL;
} window;

// CMFCKlineDlg 对话框

CMFCKlineDlg::CMFCKlineDlg(bool ok, CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_MFCKLINE_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    isok = ok;
    // QTextCodec::setCodecForLocale(QTextCodec::codecForName("gb18030"));
}

CMFCKlineDlg::~CMFCKlineDlg()
{
    //_CrtDumpMemoryLeaks();//couse a NC.
}

void CMFCKlineDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, m_list);
}

BEGIN_MESSAGE_MAP(CMFCKlineDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_MESSAGE(WM_MSG_LIST, OnItemMsg)
    ON_BN_CLICKED(IDC_OGL, &CMFCKlineDlg::OnBnClickedOgl)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &CMFCKlineDlg::OnLvnItemchangedList)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CMFCKlineDlg::OnNMDblclkList)
    ON_BN_CLICKED(IDOK, &CMFCKlineDlg::OnBnClickedOK)
    ON_BN_CLICKED(IDCANCEL, &CMFCKlineDlg::OnBnClickedCancel)
    ON_WM_DROPFILES()
END_MESSAGE_MAP()

// CMFCKlineDlg 消息处理程序

BOOL CMFCKlineDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    InitStockMap();
    SetMenuList();
    SetBottom();

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCKlineDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    } else
    {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCKlineDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CMFCKlineDlg::OnBnClickedOgl()
{
    m_GlDlg = new MyOglDrawDlg("127.0.0.1");
    m_GlDlg->Create(IDD_OGLIMG);
    m_GlDlg->ShowWindow(SW_SHOWNORMAL);
}

void CMFCKlineDlg::OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    POSITION curPos = m_list.GetFirstSelectedItemPosition();
    CMFCKlineDlg newdlg(isok);
    const char Open[4][8] = { "K-line","Qt","WPF","OK" };
    int N = 0;
    MyOglDrawDlg mdlg;
    CString stPath = ".\\";
#ifdef _DEBUG
    stPath = "..\\Debug";
#endif
    mdlg.CallShellScript(stPath, "call.bat", NULL);
    if (!curPos)
        ::SendMessage(m_hBottom, SB_SETTEXT, 0, (LPARAM)TEXT(Open[3]));
    else if (!isok)
    {
        while (curPos)
        {
            if (tolog && ((int)curPos > item) && (curPos != (POSITION)0x00000001))
            {
                AfxMessageBox("启动错误");
                tolog = true;
                item = 65535;
                OnCancel();
                newdlg.DoModal();
                continue;
            }
            if (tolog && (item == 65535) && (curPos == (POSITION)0x00000001))
            {
                logdlg.DoModal();
                item = N;
                tolog = false;
                break;
            }
            N = item = m_list.GetNextSelectedItem(curPos);
            ::SendMessage(m_hBottom, SB_SETTEXT, 0, (LPARAM)TEXT(Open[item]));
        }
        if (strcmp(Open[item], "Qt") == 0)
            mdlg.FloatDrift("Qt框架之K线图");
    }
    *pResult = 0;
}

void CMFCKlineDlg::InitStockMap()
{
    m_Ogl.stockmap.insert(std::pair<int, OGLKview::StockDesc>(0, { "SH600747", "大连控股" }));
    m_Ogl.stockmap.insert(std::pair<int, OGLKview::StockDesc>(1, { "1", "File1" }));
}

void CMFCKlineDlg::SetMenuList()
{
    RECT m_rect;
    m_list.GetClientRect(&m_rect);
    m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    m_list.InsertColumn(0, _T("打开方式"), 0, m_rect.right / 6);
    m_list.InsertColumn(1, _T("文件"), 0, m_list.GetColumnWidth(0));
    m_list.InsertColumn(2, _T("内容"), 0, m_list.GetColumnWidth(0));
    m_list.InsertColumn(3, _T("..."), 0, m_list.GetColumnWidth(0));
    m_list.InsertItem(0, _T("MFC"));
    m_list.InsertItem(1, _T("QT"));
    m_list.InsertItem(2, _T("WPF"));
    m_list.SetItemText(0, 1, m_Ogl.stockmap[1].desc.c_str());
    m_list.SetItemText(0, 2, m_Ogl.stockmap[0].desc.c_str());
}

void CMFCKlineDlg::OnNMDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    POSITION curpos = m_list.GetFirstSelectedItemPosition();
    while (curpos)
    {
        int item = m_list.GetNextSelectedItem(curpos);
        TRACE1("Item %d was selected!\n", item);
        ::PostMessage(this->m_hWnd, WM_MSG_LIST, 0, (LPARAM)item);
    }
    *pResult = 0;
}

void CMFCKlineDlg::SetBottom()
{
    m_hBottom = CreateStatusWindow(WS_CHILD | WS_VISIBLE | WS_BORDER,
        TEXT("OK"),
        GetSafeHwnd(),
        ID_STATUS);
    int pint[4] = { 100,200,350,-1 };
    ::SendMessage(m_hBottom, SB_SETPARTS, 4, (LPARAM)pint);
}

LRESULT CMFCKlineDlg::OnItemMsg(WPARAM wParam, LPARAM lParam)
{
    RECT rt = { 0 };
    ::GetWindowRect(this->m_hWnd, &rt);
    ::SetWindowPos(this->m_hWnd, HWND_NOTOPMOST, rt.left, rt.top, 0, 0, SWP_NOMOVE || SWP_NOSIZE);
    switch (lParam)
    {
    case 0:
        OpenGL(0);
        break;
    case 1:
        OpenQt();
        break;
    case 2:
        OpenWPF();
        break;
    default:
        break;
    }
    ::SetWindowPos(this->m_hWnd, HWND_TOPMOST, rt.left, rt.top, 0, 0, SWP_NOMOVE || SWP_NOSIZE);
    return LRESULT();
}

bool CMFCKlineDlg::OpenGL(int item)
{
    for (int i = 0; i <= m_list.GetHeaderCtrl()->GetItemCount(); ++i)
    {
        if (m_list.GetItemText(item, i) == m_Ogl.stockmap[item].desc.c_str())
            OnBnClickedOgl();
    }
    return true;
}

void CMFCKlineDlg::OpenWPF()
{
    WPFhost::WPFWindow = gcnew WPFKline::MainWindow();
    WPFhost::WPFWindow->ShowDialog();
}

void CMFCKlineDlg::OpenQt()
{
    //QMfcApp::instance(this);
    HWND h_Wnd = ::FindWindow(NULL, _T("QtKline"));
    CDialog MessageBox(_T("Qt"));
    OpenQtexe();
}

BOOL CALLBACK EnumChildWindowCallBack(HWND hWnd, LPARAM lParam)
{
    DWORD dwPid = 0;
    GetWindowThreadProcessId(hWnd, &dwPid);
    if (dwPid == lParam)
    {
        printf("0x%08X    ", hWnd);
        TCHAR tmp[WINDOW_TEXT_LENGTH];
        SendMessage(hWnd, WM_GETTEXT, WINDOW_TEXT_LENGTH, (LPARAM)tmp);
        wprintf(L"%s\n", tmp);
        EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);
    }
    return TRUE;
}

BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)
{
    ProcessWindow* pProcessWindow = (ProcessWindow*)lParam;

    DWORD dwProcessId;
    GetWindowThreadProcessId(hWnd, &dwProcessId);

    if (pProcessWindow->dwProcessId == dwProcessId && IsWindowVisible(hWnd) && GetParent(hWnd) == NULL)
    {
        pProcessWindow->hwndWindow = hWnd;
    }
    return TRUE;
}

int CMFCKlineDlg::OpenQtexe()
{
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    HWND hChildWnd = NULL;
    //::GetCurrentDirectory(300, (LPTSTR)path);
    //strcat
    CHAR fulPth[256] = { NULL };
    GetModuleFileName(NULL, fulPth, sizeof(fulPth));
    CString projPth((LPCSTR)fulPth);
    projPth = projPth.Left(projPth.ReverseFind(_T('\\')));
    projPth = projPth.Left(projPth.ReverseFind(_T('\\')));
    LPTSTR cmdline = new TCHAR[projPth.GetLength() + 40];
#ifndef _DEBUG
    _tcscpy(cmdline, TEXT(projPth + "\\Release\\QtGames.exe"));
#else
    _tcscpy(cmdline, TEXT(projPth + "\\Debug\\QtGames.exe"));
#endif
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = TRUE;
    if (::CreateProcess(
        cmdline,
        "", //Unicode版本此参数不能为常量字符串
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &pi))
    {
        window.dwProcessId = pi.dwProcessId;

        EnumWindows(EnumWindowCallBack, (LPARAM)&window);
        WaitForInputIdle(pi.hProcess, 500);

        ::CloseHandle(pi.hThread);
        ::CloseHandle(pi.hProcess);
    } else
    {
        CString szMsg;
        LPVOID lpMsgBuf;
        int errNo = GetLastError();
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            errNo,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0, NULL);
        szMsg.Format("OpenQtexe: errno(%d)\n%s", errNo, (LPTSTR)lpMsgBuf);
        AfxMessageBox(szMsg);
    }
    if (cmdline) delete cmdline;
    //while (!hChildWnd)
    //{
    //	hChildWnd = ::FindWindow(/*0 & , */"QtKline", NULL);
    //	if (hChildWnd != NULL)
    //		break;
    //}
    if (hChildWnd = ::FindWindow(/*0 & , */"QtKline", NULL))
    {
        LONG style = GetWindowLong(hChildWnd, GWL_STYLE);
        style &= ~WS_CAPTION;
        style &= ~WS_THICKFRAME;
        style |= WS_CHILD;
        SetWindowLong(hChildWnd, GWL_STYLE, style);
        CWnd* pWnd = FromHandle(hChildWnd);
        pWnd->SetParent(this);
        //::SetWindowPos(hChildWnd, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOSIZE | SWP_NOMOVE);
        //pWnd->SetActiveWindow();
        //pWnd->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        pWnd->ShowWindow(SW_SHOW);
    }
    return 0;
}

void CMFCKlineDlg::OnBnClickedOK()
{
    try {
        if (m_GlDlg != NULL)
        {
            delete m_GlDlg;
            m_GlDlg = NULL;
        }
    } catch (exception e) {
        exit(-1);
    }
    CDialogEx::OnCancel();
}

void CMFCKlineDlg::OnBnClickedCancel()
{
    isok = true;
    tolog = false;
    ShowWindow(SW_HIDE);
}

void CMFCKlineDlg::OnDropFiles(HDROP hDropInfo)
{
    int count = DragQueryFile(hDropInfo, -1, NULL, 0);//取得被拖动文件的数目
    for (int i = 0; i < count; i++)
    {
        CHAR wcStr[MAX_PATH];
        DragQueryFile(hDropInfo, i, wcStr, MAX_PATH);//获得拖曳的第i个文件的文件名
        MessageBox(wcStr);
    }
    DragFinish(hDropInfo);  //拖放结束后,释放内存
    CDialogEx::OnDropFiles(hDropInfo);
}
