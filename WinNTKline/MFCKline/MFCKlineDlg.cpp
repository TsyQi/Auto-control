
// MFCKlineDlg.cpp : ʵ���ļ�
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

// CMFCKlineDlg �Ի���

CMFCKlineDlg::CMFCKlineDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCKLINE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMFCKlineDlg::~CMFCKlineDlg()
{
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
END_MESSAGE_MAP()


// CMFCKlineDlg ��Ϣ�������

BOOL CMFCKlineDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	InitMap();
	SetList();
	SetBottom();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCKlineDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFCKlineDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCKlineDlg::OnBnClickedOgl()
{
	MyOglDrawDlg *pTD = new MyOglDrawDlg();
	pTD->Create(IDD_OGLDLG);
	pTD->ShowWindow(SW_SHOWNORMAL);
}


void CMFCKlineDlg::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	POSITION curpos = m_list.GetFirstSelectedItemPosition();
	char Open[4][8] = { "K-line","Qt","WPF","OK" };
	if(!curpos)
		::SendMessage(m_hBottom, SB_SETTEXT, 0, (LPARAM)TEXT(Open[3]));
	else
		while (curpos)
		{
			int item = m_list.GetNextSelectedItem(curpos);
			::SendMessage(m_hBottom, SB_SETTEXT, 0, (LPARAM)TEXT(Open[item]));
		}
	*pResult = 0;
}

void CMFCKlineDlg::InitMap()
{
	m_Ogl.stockmap.insert(std::pair<int, OGLKview::Strmap>(0, { "SH600747", "�����ع�" }));
	m_Ogl.stockmap.insert(std::pair<int, OGLKview::Strmap>(1, { "1", "File1" }));
}

void CMFCKlineDlg::SetList()
{
	RECT m_rect;
	m_list.GetClientRect(&m_rect);
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, _T("�򿪷�ʽ"), 0, m_rect.right / 6);
	m_list.InsertColumn(1, _T("�ļ�"), 0, m_list.GetColumnWidth(0));
	m_list.InsertColumn(2, _T("����"), 0, m_list.GetColumnWidth(0));
	m_list.InsertColumn(3, _T("..."), 0, m_list.GetColumnWidth(0));
	m_list.InsertItem(0, _T("MFC"));
	m_list.InsertItem(1, _T("QT"));
	m_list.InsertItem(2, _T("WPF"));
	m_list.SetItemText(0, 1, m_Ogl.stockmap[1].info.c_str()); 
	m_list.SetItemText(0, 2, m_Ogl.stockmap[0].info.c_str());
}

void CMFCKlineDlg::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	POSITION curpos = m_list.GetFirstSelectedItemPosition();
	while(curpos)
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
	switch (lParam)
	{
	case 0:
		OpenGL(lParam);
		break;
	case 1:
		OpenQt();
		break;
	case 2:
		OpenWpf();
		break;
	default:
		break;
	}
	return LRESULT();
}

bool CMFCKlineDlg::OpenGL(int item)
{
	for (int i = 0; i <= m_list.GetHeaderCtrl()->GetItemCount(); ++i)
	{
		if (m_list.GetItemText(item, i) == m_Ogl.stockmap[item].info.c_str())
			OnBnClickedOgl();
	}
	return true;
}

void CMFCKlineDlg::OpenWpf()
{
	WpfHost::WpfWindow = gcnew WpfKline::MainWindow();
	WpfHost::WpfWindow->ShowDialog();
}

void CMFCKlineDlg::OpenQt()
{
	m_Mod.FloatDrift("Qt���K��ͼ");
	HWND h_Wnd=::FindWindow(NULL,_T("QtKline"));
	CDialog MessageBox("Qt");
	OpenQtexe();
}

BOOL CALLBACK EnumChildWindowCallBack(HWND hWnd, LPARAM lParam)
{
	DWORD dwPid = 0;
	GetWindowThreadProcessId(hWnd, &dwPid);  
	if (dwPid == lParam)   
	{
		printf("0x%08X    ", hWnd);  
		TCHAR buf[WINDOW_TEXT_LENGTH];
		SendMessage(hWnd, WM_GETTEXT, WINDOW_TEXT_LENGTH, (LPARAM)buf);
		wprintf(L"%s/n", buf);
		EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);
	}
	return TRUE;
}

BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)
{
	ProcessWindow *pProcessWindow = (ProcessWindow *)lParam;

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
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = TRUE;
	TCHAR cmdline[] = TEXT("F:/dell-pc/Documents/Visual Studio 2015/Projects/WinNTKline/QtKline/Win32/Debug/QtKline.exe");
	if (::CreateProcess(
		cmdline,
		"", //Unicode�汾�˲�������Ϊ�����ַ���
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
	}
	else
	{
		int error = GetLastError();
		printf("error code:%d/n", error);
	}
	//while (!hChildWnd)
	//{
	//	hChildWnd = ::FindWindow(/*0 & , */"QtKline", NULL);
	//	if (hChildWnd != NULL)
	//		break;
	//}
	//HWND hQt = ::FindWindow("QtKline", NULL);
	if (hChildWnd = ::FindWindow(/*0 & , */"QtKline", NULL))
	{
		LONG style = GetWindowLong(hChildWnd, GWL_STYLE);
		style &= ~WS_CAPTION;
		style &= ~WS_THICKFRAME;
		style |=WS_CHILD;  
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