// MyOglDrawDlg.cpp :

#include "stdafx.h"
#include "MyOglDrawDlg.h"

using namespace std;
bool net_exit = false;

IMPLEMENT_DYNAMIC(MyOglDrawDlg, CDialog)

MyOglDrawDlg::MyOglDrawDlg(const char* sIP, CWnd* pParent /*=NULL*/)
    : CDialog(IDD_OGLIMG, pParent)
{
    this->ctpIP = (char*)sIP;
    net_exit = false;
}

void MyOglDrawDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_GLTAB, m_tab);
}

BEGIN_MESSAGE_MAP(MyOglDrawDlg, CDialog)
    ON_WM_SIZE()
    ON_WM_CLOSE()
    ON_WM_TIMER()
    ON_WM_PAINT()
    //ON_WM_SETCURSOR()
    ON_WM_MOUSEMOVE()
    ON_WM_SYSCOMMAND()
    ON_WM_CONTEXTMENU()
    ON_WM_RBUTTONDOWN()
    ON_MESSAGE(WM_MSG_OGL, GetOglCmd)
    ON_MESSAGE(WM_MSG_BOX, ShowMsgOnly)
    ON_MESSAGE(WM_MSG_SHOW, OnTaskShow)
    ON_MESSAGE(WM_MSG_TITL, SetDlgTitle)
    ON_COMMAND(WM_MSG_DEP, &MyOglDrawDlg::SetDeepDeg)
    ON_COMMAND(WM_MSG_BKG, &MyOglDrawDlg::SetClientBkg)
    ON_COMMAND(WM_MSG_QUIT, &MyOglDrawDlg::PostNcDestroy)
    ON_WM_DROPFILES()
    ON_WM_CTLCOLOR()
    ON_WM_CREATE()
    ON_COMMAND(ID_SETBKG, &MyOglDrawDlg::OnSetBkg)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_COMMAND(ID_SCRN, &MyOglDrawDlg::OnScrn2Bmp)
END_MESSAGE_MAP()

namespace StockOgl
{
    int   idx = 0;
    int   li = 0;
    int   failmsg = 0;
    int   insert = 1;
    char* cotx = NULL;
    char  buff[256];
    char* token = NULL;
    bool  isNext = false;
    char* div_stock[3] = { NULL };
    char  code[64] = { NULL };
    char  exeFullPath[256] = { NULL };
    CString csDirPath;
    string text;
    string sSystemFullPath;
    string sWritePath;
    ifstream Readfile;
    OGLKview::Market stocks;
    WIN32_FIND_DATA fileData;
    OGLKview::Point Pter = { 0 }, Pt[4] = { 0 };
    OGLKview::Point pt_code = { 0.8f,1.189f };
}

char* MyOglDrawDlg::GetFirstData()
{
    //使用相对路径读取:
    //F:\dell-pc\Documents\GitHub\MyAutomatic\WinNTKline\Debug\data\SH600747.DAT
    using namespace StockOgl;
    GetModuleFileName(NULL, exeFullPath, sizeof(exeFullPath));
    //CString csDirPath((LPCSTR)exeFullPath);
    csDirPath.Format("%s", exeFullPath);
    csDirPath = csDirPath.Left(csDirPath.ReverseFind(_T('\\'))) + "\\data";
    string sSystemPath = csDirPath.GetBuffer(csDirPath.GetLength());
    csDirPath += "\\*.DAT";
    FindFirstFile(csDirPath.GetBuffer(), &fileData);
    sSystemFullPath = sSystemPath + (string)_T("\\") + fileData.cFileName;
    return m_Ogl.file = const_cast<char*>(sSystemFullPath.c_str());
}

int MyOglDrawDlg::GetMarkDatatoDraw(int sign, int len)
{
    InitializeCriticalSection(&mutex);
    static int line = 0;
    if (m_Ogl.file == nullptr)
        return -1;
    using namespace StockOgl;
    std::vector<char*> tradedata;
    tradedata.reserve(1);
    Readfile.open(m_Ogl.file, ios::in);
    if (Readfile.fail())
    {
        if (failmsg == 0 && m_hWnd != NULL)
            ::PostMessage(m_hWnd, WM_MSG_OGL, 0, (LPARAM)m_Ogl.conv.AllocBuffer("Reading failure!"));
        failmsg = -1;
        return failmsg;
    } else {
        while (getline(Readfile, text))
        {
            EnterCriticalSection(&mutex);
            memcpy(buff, const_cast<char*>(text.c_str()), text.size() + 1);
            token = strtok_s(buff, "/\t", &cotx);
            while (token != nullptr && &token != nullptr)
            {
                try {
                    tradedata.push_back(token);
                } catch (exception) {
                    break;
                }
                token = strtok_s(nullptr, "/\t", &cotx);
            }
            token = nullptr;
            if ((tradedata.size() < 3) && (tradedata.size() > 0))
            {
                m_Ogl.coding = true;
                memcpy(buff, static_cast<char*>(tradedata.at(0)), strlen(tradedata.at(0)) + 1);
                title.Format("%s", buff);
                SetWindowText(title);
                idx = 0;
                token = strtok_s(buff, " ", (char**)&cotx);
                while (token != nullptr)
                {
                    div_stock[idx] = token;
                    token = strtok_s(nullptr, " ", (char**)&cotx);
                    idx++;
                }
                m_Ogl.SwitchViewport(0);
                sprintf(code, "%s(%s)", div_stock[1], div_stock[0]);
                m_Ogl.DrawKtext(code, pt_code, 20, { 1,1,0 }, "Terminal", false);
                sprintf(code, _T("%s(%s)<%s>"), div_stock[1], div_stock[0], div_stock[2]);
                pt_code = { -1.22f,pt_code.y };
                m_Ogl.DrawKtext(code, pt_code, 12, { 1,1,0 }, "宋体");
                pt_code = { 0.8f,1.189f };
                *div_stock = nullptr;
                tradedata.clear();
            } else if (tradedata.size() > 8)
            {
                if (line >= sign && sign >= 0) {
                    if (sign != 0 && line > sign + len)
                        break;
                    m_Ogl.lastmarket = stocks;
                    stocks.time.tm_year = atoi(tradedata.at(0));
                    stocks.time.tm_mon = atoi(tradedata.at(1));
                    stocks.time.tm_mday = atoi(tradedata.at(2));
                    stocks.open = (float)atof(tradedata.at(3));
                    stocks.high = (float)atof(tradedata.at(4));
                    stocks.low = (float)atof(tradedata.at(5));
                    stocks.close = (float)atof(tradedata.at(6));
                    stocks.amount = atoi(tradedata.at(7));
                    stocks.price = (float)atof(tradedata.at(8));
                    if (line < m_Ogl.tinkep.move + 12)
                    {
                        if (line > 0)
                            if (li > 3)
                            {
                                li = 0;
                                isNext = true;
                            }
                        Pt[li].x = (22 - m_Ogl.tinkep.multi * (3 - line * (m_Ogl.tinkep.multi + 9) * .01f + 3.f + m_Ogl.tinkep.move * .1f * insert) * 2) * .04f - 0.8f;
                        Pt[li].y = (float)atof(tradedata.at(6)) * .8f + 2.5f;
                        if (line <= 3)
                        {
                            m_Ogl.dlginfo.line = 1;
                            Pter = Pt[li];
                        } else
                            m_Ogl.dlginfo.line = line - 2;
                        if (line > 2) {
                            if (0 != m_Ogl.DrawPoly(Pter, Pt[li], { 0.f,1.f,0.f }))
                                m_Ogl.SwitchViewport(0);
                        }
                        m_Ogl.DrawKline(stocks, m_Ogl.tinkep);
                        Pter = Pt[li];
                        li++;
                    }
                }
                tradedata.clear();
            }
            line++;
            LeaveCriticalSection(&mutex);
        }
        if (m_Ogl.dlginfo.drawstaff > 0)
            m_Ogl.DrawTrade();
        m_szData = line;
        line = 0;
    }
    vector<char*>().swap(tradedata);
    margin = abs(m_Ogl.getPrices().first - m_Ogl.getPrices().second);
    Readfile.close();
    DeleteCriticalSection(&mutex);
    return line;
}

BOOL MyOglDrawDlg::OnInitDialog()
{
    HWND m_hWnd = this->GetSafeHwnd();
    m_hDC = ::GetDC(m_hWnd/*m_tab.GetSafeHwnd()*/);
    CTPclient* m_ctp = new CTPclient();
    m_Ogl.SetWindowPixelFormat(m_hDC, m_hWnd);
    OnPaint();
    SetCtrl();
    m_Ogl.AdjustDraw(W, H);
    GetFirstData();
#ifdef GLTEST
    test.LoadGLTexture();
    //test.Load__QDU(W, H);
#else
    SetTimer(1, 30, NULL);
#endif
    SetWindowPos(FromHandle(GetSafeHwnd()), 0, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN), SWP_NOZORDER);
    m_hAcc = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(ID_ACCLER));
    CloseHandle((HANDLE)_beginthreadex(NULL, 0, m_ctp->SimpleClient, (void*)this, 0, NULL));
    delete m_ctp;
    return CDialog::OnInitDialog();
}

void MyOglDrawDlg::OnPaint()
{
    CRect Recto;
    CPaintDC dc(this);
    GetClientRect(&Recto);
    dc.SetViewportOrg(W = Recto.Width(), H = Recto.Height());
}

HCURSOR MyOglDrawDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}
#ifdef _UNICODE
void MyOglDrawDlg::OnTimer(UINT_PTR nIDEvent)
#else
void MyOglDrawDlg::OnTimer(UINT nIDEvent)
#endif
{
    switch (nIDEvent)
    {
    case 1://调用绘图函数
        DrawFunc(m_hDC);
        break;
    default:
        break;
    }
    CDialog::OnTimer(nIDEvent);
}

NOTIFYICONDATA MyOglDrawDlg::GetNotiIcon(HWND O_hWnd)
{
    m_icon.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
    m_icon.hWnd = O_hWnd;
    m_icon.uID = IDR_MAINFRAME;
    m_icon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
    m_icon.uCallbackMessage = WM_MSG_SHOW;
    m_icon.uTimeout = 3000;
    m_icon.dwInfoFlags = NIIF_INFO;
    _tcscpy(m_icon.szTip, "隐藏的对话框");
    _tcscpy(m_icon.szInfoTitle, "已最小化到托盘区");
    _tcscpy(m_icon.szInfo, "点击图标恢复，右键弹出菜单。");
    m_icon.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
    return m_icon;
}

void MyOglDrawDlg::SetCurrentPosition(float winx, float winy)
{
}

void MyOglDrawDlg::FloatDrift(char* text)
{
    CFont font;
    CWindowDC wdc(FromHandle(::GetDesktopWindow()));
    wdc.SetBkMode(BKMODE_LAST);
    wdc.SetTextColor(RGB(20, 20, 20));
    //wdc.BitBlt(0,0,
    wdc.LineTo(0, 0);
    font.CreateFont(20, 0, 0, 0,
        FW_MEDIUM,
        false/*斜体*/, false, false,
        OEM_CHARSET/*ANSI_CHARSET*/, OUT_CHARACTER_PRECIS,
        CLIP_STROKE_PRECIS, PROOF_QUALITY,
        FF_SCRIPT, " ");
    wdc.SelectObject(&font);
    wdc.TextOut(400, 400, text, strlen(text));
    ::Sleep(300);
    if (text)
        text = NULL;
}

void _stdcall MyOglDrawDlg::DrawFunc(HDC m_hDC)
{
    m_Ogl.InitGraph();
    m_Ogl.DrawCoord(m_Ogl.dlginfo.mouX, m_Ogl.dlginfo.mouY);
#if !defined(GLTEST)
#ifdef _DEBUG || _UTILAPIS_
    this->GetMarkDatatoDraw(kPos, 100);
#else
    m_Ogl.GetMarkDatatoDraw();
#endif
    depth->FillChart(m_Ogl.unfurl);
    depth->DrawItem(depth->item);
#else
    //test.Load__QDU(W, H);
    test.Model(m_Ogl.dlginfo.width, m_Ogl.dlginfo.height, offsetX, offsetY);
    test.House(m_Ogl.dlginfo.width, m_Ogl.dlginfo.height);
    //test.glTEST(true);
#endif
    m_Ogl.AdjustDraw(m_Ogl.dlginfo.width, m_Ogl.dlginfo.height);
    m_Ogl.SetViewBkg(m_Ogl.dlginfo.bkg);
    SwapBuffers(m_hDC);
}

void MyOglDrawDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
}

// MyOglDrawDlg

void MyOglDrawDlg::OnSysCommand(UINT nID, LPARAM lparam)
{
    if (SC_MINIMIZE == nID)
    {
#ifndef _WIN64
        NOTIFYICONDATA noti = GetNotiIcon(m_hWnd);
        Shell_NotifyIcon(NIM_ADD, &noti);
        ShowWindow(SW_HIDE);
#endif
    } else
        CDialog::OnSysCommand(nID, lparam);
}

LRESULT MyOglDrawDlg::GetOglCmd(WPARAM wparam, LPARAM lparam)
{
    CString cmd;
    char* pMsg = (char*)lparam;
    cmd.Format(_T("%s"), pMsg);
    if (MessageBox(cmd) == IDOK)
        OnClose();
    if (pMsg)
        pMsg = NULL;
    return LRESULT();
}

LRESULT MyOglDrawDlg::ShowMsgOnly(WPARAM wparam, LPARAM lparam)
{
    CString cmd;
    char* pMsg = (char*)lparam;
    if (pMsg != NULL)
    {
        cmd.Format(_T("%s"), pMsg);
        AfxMessageBox(cmd);
        pMsg = NULL;
    }
    return LRESULT();
}

BOOL MyOglDrawDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    if (nHitTest != HTCAPTION)
    {
        if (m_Ogl.dlginfo.drawstaff)
        {
            if ((m_Ogl.dlginfo.mouY >= 20) && (m_Ogl.dlginfo.mouX < 950) && (m_Ogl.dlginfo.mouY < 380))
                SetCursor(NULL);
        } else if ((m_Ogl.dlginfo.mouY == 380) && (m_Ogl.dlginfo.mouX < 950))
            SetCursor(LoadCursor(NULL, IDC_SIZENS));
    }
    return 0;
}

void MyOglDrawDlg::OnSize(UINT nType, int cx, int cy)
{
    char warn[] = "不支持该屏占比";
    m_Ogl.dlginfo.width = cx;
    m_Ogl.dlginfo.height = cy;
    OGLKview::Point t = { -1,1 };
    if (!cx || !cy)
        return;
    if (cy / cx < 744 / 1290 || cx / cy > 1.74731f)
    {
        glClearColor(0, 0, 0, 1);
        m_Ogl.DrawKtext(warn, t, 20, { 1,1,0 });
    }
    CDialog::OnSize(nType, cx, cy);
}

LRESULT MyOglDrawDlg::OnTaskShow(WPARAM wparam, LPARAM lparam)
{
    CMenu cmenu;
    CBitmap bitmap;
    if (lparam != IDR_MAINFRAME)
        switch (lparam)
        {
        case WM_RBUTTONUP:
        {
            LPPOINT lpoint = new tagPOINT;
            ::GetCursorPos(lpoint);
            cmenu.CreatePopupMenu();
            cmenu.AppendMenu(MF_STRING, /*ID_APP_EXIT*/WM_MSG_QUIT, _T("Exit"));
            SetForegroundWindow();
            bitmap.LoadBitmap(IDB_BITMAP);
            cmenu.SetMenuItemBitmaps(0, MF_BYPOSITION, &bitmap, &bitmap);
            cmenu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x, lpoint->y, this);
            cmenu.Detach();
            cmenu.DestroyMenu();
            delete lpoint;
        }
        break;
        case WM_LBUTTONDOWN:
            if (IsWindowVisible())
                ShowWindow(SW_HIDE);
            else
            {
                ShowWindow(SW_SHOW);
                Shell_NotifyIcon(NIM_DELETE, &m_icon);
            }
            break;
        default:
            break;
        }
    return LRESULT();
}

LRESULT CALLBACK MyOglDrawDlg::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_DESTROY)
    {
        CWnd::OnDestroy();
    }
    return CDialog::WindowProc(uMsg, wParam, lParam);
}

BOOL MyOglDrawDlg::PreTranslateMessage(tagMSG* pMsg)
{
    switch (pMsg->message)
    {
    case WM_KEYDOWN:
    {
        if (m_hAcc && ::TranslateAccelerator(m_hWnd, m_hAcc, pMsg))
        {
            Dos.RedirectConsole();
            break;
        } else switch (pMsg->wParam)
        {
        case 'F':
        case 'f':
        {
            SetDeepDeg();
        }
        break;
        case 'M':
        {
            depth->item.time.hour = depth->item.time.min = 9;
            depth->item.pc_ = 99;
            depth->item.mode = 1;
        }
        break;
        case 'V':
        {
            CTPclient* m_ctp = new CTPclient();
            CloseHandle((HANDLE)_beginthreadex(NULL, 0, m_ctp->TradeMarket, (void*)this, 0, NULL));
            delete m_ctp;
        }
        break;
        case VK_OEM_COMMA:
            kPos--;
            break;
        case VK_OEM_PERIOD:
            kPos++;
            break;
        case VK_RIGHT:
        {
            m_Ogl.tinkep.move++;
            if (m_Ogl.tinkep.move > m_szData + 30)
                m_Ogl.tinkep.move = m_szData + 30;
        }
        break;
        case VK_LEFT:
        {
            m_Ogl.tinkep.move--;
            if (m_Ogl.tinkep.move < 70)
                m_Ogl.tinkep.move = 70;
        }
        break;
        }
    };
    };
    return CDialog::PreTranslateMessage(pMsg);
}

BOOL MyOglDrawDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
#ifdef _DEBUG
    if (nCode < 0 && nCode != static_cast<int>(0x8000))
        TRACE1("Implementation Warning: control notification = $%X.\n", nCode);
#endif
    if (nCode == CN_COMMAND)
    {
        //for (UINT i = IDR_BKG + 1; i <= IDR_BKG + 5; i++)
        //    if (nID == i)
        //    {
        //        itoa(i - IDR_BKG, buffer, 10);
        //    }
        switch (nID)
        {
        case ID_BKG:
            SetClientBkg();
            break;
        case ID_LEVEL:
            SetDeepDeg();
            break;
        default:
            break;
        }
    } else
    {
        //if (_afxThreadState->m_hLockoutNotifyWindow == m_hWnd)
        //    return TRUE;        // locked out - ignore control notification
        // zero IDs for normal commands are not allowed
        if (nID == 0)
            return FALSE;
    }
    return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void MyOglDrawDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
    {
        HINSTANCE hInstance = (HINSTANCE)GetWindowLong(pWnd->m_hWnd, GWL_HINSTANCE);
        HMENU hMenuTray = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU));
        HMENU hPopupMenu = GetSubMenu(hMenuTray, 0);
        TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, 0, pWnd->m_hWnd, NULL);
    }
    {
        CMenu o_mPop;
        o_mPop.LoadMenu(IDR_MENU);
        o_mPop.GetSubMenu(0);
        ClientToScreen(&point);
        o_mPop.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x - 8, point.y - 60, pWnd);
        o_mPop.Detach();
    }
}

LRESULT MyOglDrawDlg::SetDlgTitle(WPARAM wparam, LPARAM lparam)
{
    SetWindowText((char*)lparam);
    return LRESULT();
}

void MyOglDrawDlg::OnClose()
{
    net_exit = true;
    KillTimer(1);
    //DestroyMenu(m_hTop);
    CWnd::OnClose();
    return PostNcDestroy();
}

void MyOglDrawDlg::PostNcDestroy()
{
    Shell_NotifyIcon(NIM_DELETE, &m_icon);
}

void MyOglDrawDlg::SetCtrl()
{
#ifdef OWNCTL_H
    CMyMenu
#else
    CMenu
#endif
        m_uTop;
    m_uTop.LoadMenu(IDR_TOP);// AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_TOP));
    ASSERT(this);
    CBrush m_brBkgd;
    m_brBkgd.DeleteObject();
    m_brBkgd.CreateSolidBrush(RGB(0, 0, 0));
    MENUINFO m_info = { 0 };
    m_info.cbSize = sizeof(MENUINFO);
    m_info.fMask = /*MIM_APPLYTOSUBMENUS |*/ MIM_BACKGROUND;
    //m_info.dwStyle = MNS_AUTODISMISS;
    m_info.hbrBack = m_brBkgd;
    try {
        if (IsMenu(m_uTop.m_hMenu))
            //::SetMenuInfo(m_uTop.m_hMenu, &m_info);
            Invalidate();
    } catch (...) { return; }
    for (int i = 0; i < m_uTop.GetMenuItemCount(); i++)
    {
        BOOL bModi = m_uTop.ModifyMenu(ID_BTN800 + i, MF_BYCOMMAND | MF_OWNERDRAW, ID_BTN800 + i);
        if (!bModi)
        {
            //TRACK("ModifyMenu fail!");
        }
    }
    SetMenu(/*GetSafeHwnd(),*/ &m_uTop);
    if (m_tool.Create(this) && m_tool.LoadToolBar(IDR_TOOL))
    {
        //m_tool.EnableDocking(CBRS_ALIGN_ANY);
        m_tool.ModifyStyle(0, TBSTYLE_FLAT);
        CRect rcItem;
        //RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rcNew);
        CPoint ptOffset(0, 0);
        CWnd* pwndChild = GetWindow(GW_CHILD);
        while (pwndChild)
        {
            pwndChild->GetWindowRect(rcItem);
            ScreenToClient(rcItem);
            rcItem.OffsetRect(ptOffset);
            pwndChild->MoveWindow(rcItem, FALSE);
            pwndChild = pwndChild->GetNextWindow();
        }
        RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
    }
}

void MyOglDrawDlg::OnDropFiles(HDROP hDropInfo)
{
    //char lpszFileName[MAX_PATH + 1] = { 0 };
    //char g____lpstrFilename[2048];
    //memset(g____lpstrFilename, 0, 512 * sizeof(TCHAR));
    if (DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0) == 1)
    {
        //DragQueryFile(hDropInfo, 0, g____lpstrFilename, 512);
        //myopenfile( g____lpstrFilename, 1 );
        DragQueryFile(hDropInfo, 0, (LPTSTR)m_Ogl.file, _MAX_PATH);
        //m_edit.SetWindowText(g____lpstrFilename);
        //::DragFinish(hDropInfo); //释放内存 i="0;" delete []lpszFileName; //
    };
    CDialog::OnDropFiles(hDropInfo);
}

int MyOglDrawDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;
    DragAcceptFiles(TRUE);
    return 0;
}

auto MyOglDrawDlg::GetPrivMem()
{
    auto f = *((bool*)&m_Ogl);
    if (!f)
        MessageBox(_T("私有成员"));
    return f;
}

HBRUSH MyOglDrawDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CreateSolidBrush(RGB(0, 0, 0));
    if (pWnd->GetDlgCtrlID() == IDR_TOP)
    {
        pDC->SetTextColor(RGB(20, 20, 20));
        pDC->SetBkMode(TRANSPARENT);
    }
    return hbr;
}

void MyOglDrawDlg::CallShellScript(CString Path, CString fbat, CString param)
{
    USES_CONVERSION;
    SHELLEXECUTEINFO ShExecInfo = { 0 };
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpFile = fbat;
    ShExecInfo.lpParameters = param;
    ShExecInfo.lpDirectory = Path;
    ShExecInfo.nShow = SW_HIDE;
    ShExecInfo.hInstApp = NULL;
    ShellExecuteEx(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
}

void MyOglDrawDlg::SetClientBkg()
{
    if (m_Ogl.dlginfo.bkg)
        m_Ogl.dlginfo.bkg = false;
    else
        m_Ogl.dlginfo.bkg = true;
}

void MyOglDrawDlg::SetDeepDeg()
{
    if (m_Ogl.unfurl)
        m_Ogl.unfurl = false;
    else
        m_Ogl.unfurl = true;
}

void MyOglDrawDlg::OnSetBkg()
{
    SetClientBkg();
}

void MyOglDrawDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    m_Ogl.dlginfo.mouX = point.x;
    m_Ogl.dlginfo.mouY = point.y;
    if (point.x > 980 || point.y < 22)
        m_Ogl.dlginfo.drawstaff = 0;
    if ((m_Ogl.dlginfo.leftdown) && (point.x < m_Ogl.dlginfo.width) && (point.y < m_Ogl.dlginfo.height))
    {
        offsetX = point.x - lastX;
        offsetY = point.y - lastY;
        float Delta = sqrt(offsetX * offsetX + offsetY * offsetY);
        SetCursor(LoadCursor(NULL, IDC_SIZEWE));
        if (Delta >= 6)
        {
            if (lastX < point.x)
            {
                m_Ogl.tinkep.move--;
                if (m_Ogl.tinkep.move < 70)
                    m_Ogl.tinkep.move = 70;
            } else if (lastX > point.x)
            {
                m_Ogl.tinkep.move++;
                if (m_Ogl.tinkep.move > m_szData + 30)
                    m_Ogl.tinkep.move = m_szData + 30;
            } else 1;
            lastX = (float)point.x;
            lastY = (float)point.y;
        }
    } else if ((point.x >= m_Ogl.dlginfo.width) || (point.y >= m_Ogl.dlginfo.height))
        m_Ogl.dlginfo.leftdown = false;
}

BOOL MyOglDrawDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint ot_)
{
    if (zDelta == WHEEL_DELTA)
    {
        m_Ogl.tinkep.ratio *= 2;
        m_Ogl.tinkep.multi++;
    }
    if (zDelta == -WHEEL_DELTA)
    {
        m_Ogl.tinkep.ratio /= 2;
        if (m_Ogl.tinkep.ratio < (1.0f / 32))
            m_Ogl.tinkep.ratio = (1.0f / 32);
        m_Ogl.tinkep.multi--;
        if (m_Ogl.tinkep.multi < 1)
            m_Ogl.tinkep.multi = 1;
    }
    InvalidateRect(NULL, FALSE);
    return CDialog::OnMouseWheel(nFlags, zDelta, ot_);
}

void MyOglDrawDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_Ogl.dlginfo.drawstaff++;
    m_Ogl.dlginfo.leftdown = true;
    if (m_Ogl.dlginfo.drawstaff >= 2)
        m_Ogl.dlginfo.drawstaff = 0;
    CDialog::OnLButtonDown(nFlags, point);
}

void MyOglDrawDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_Ogl.dlginfo.leftdown = false;
    CDialog::OnLButtonUp(nFlags, point);
}

void MyOglDrawDlg::OnScrn2Bmp()
{
    ScreenSharing ss;
    ss.ScrnUdpClient();
}

MyOglDrawDlg::~MyOglDrawDlg()
{
}
