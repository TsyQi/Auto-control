// IMhideWndDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCKline.h"
#include "IMhideWndDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//����ģʽ
#define HM_NONE     0   //������
#define HM_TOP      1   //�ö�����
#define HM_BOTTOM   2   //�õ�����
#define HM_LEFT     3   //��������
#define HM_RIGHT    4   //��������

#define CM_ELAPSE   200 //�������Ƿ��뿪���ڵ�ʱ����
#define HS_ELAPSE   5   //���ػ���ʾ����ÿ����ʱ����
#define HS_STEPS    10  //���ػ���ʾ���̷ֳɶ��ٲ�

#define INTERVAL    20  //����ճ��ʱ�������Ļ�߽����С���,��λΪ����
#define INFALTE     10  //��������ʱ����봰�ڱ߽����С���,��λΪ����
#define MINCX       200 //������С���
#define MINCY       400 //������С�߶�

IMPLEMENT_DYNAMIC(CIMhideWndDlg, CDialogEx)

CIMhideWndDlg::CIMhideWndDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_IMHIDEWND, pParent)
{
	//{{AFX_DATA_INIT(CIMhideWndDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	m_isSizeChanged = FALSE;
	m_isSetTimer = FALSE;
	m_hsFinished = TRUE;
	m_hiding = FALSE;

	m_oldWndHeight = MINCY;
	m_taskBarHeight = 30;
	m_edgeHeight = 0;
	m_edgeWidth = 0;
	m_hideMode = HM_NONE;
}

CIMhideWndDlg::~CIMhideWndDlg()
{
}

void CIMhideWndDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TIMER, m_timeStatus);
}


BEGIN_MESSAGE_MAP(CIMhideWndDlg, CDialogEx)
	//{{AFX_MSG_MAP(CIMhideWndDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_NCHITTEST()
	ON_WM_TIMER()
	ON_WM_SIZING()
	ON_WM_CREATE()
	ON_WM_MOVING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void get_Rtn(void* lp) {
	int feedback;
	char rcv[128];
	char info[256];
	struct LPR* lpr = (struct LPR*)lp;
	CIMhideWndDlg *dlg = (CIMhideWndDlg*)lpr->p;
	do {
		feedback = recv(lpr->sock, rcv, 256, 0);
		if (!(feedback == 256)) {
			Sleep(100);
			dlg->m_timeStatus.SetWindowText("connection lost\n");
			exit(0);
		};
		EnterCriticalSection(&lpr->wrcon);
		switch (rcv[1])
		{
		case 10:
			//sprintf(info, "join to gruop %s successfully\n", (lastgroup + 8));
			break;
		case 11:
			//sprintf(info, "join to gruop %s rejected :%s\n", (lastgroup + 8), (cmd + 32));
			break;
		case 12:
			//sprintf(info, "create gruop %s successfully\n", (lastgroup + 8));
			break;
		case 13:
			//sprintf(info, "create gruop %s rejected\n", (lastgroup + 8));
			break;
		case 14:
			//sprintf(info, "leave gruop %s successfully\n", (lastgroup + 8));
			break;
		case 20:
			sprintf(info, "%s\n", (rcv + 8));
			break;
		case 21:
			sprintf(info, "%s\n", (rcv + 32));
			break;
		case 22:
			sprintf(info, "info of %s\n %s \n", (rcv + 8), (rcv + 32));
			break;
		case 30:
			//sprintf(info, "you said to %s : %s\n", (lastmsg + 8), (lastmsg + 32));
			break;
		case 31:
			sprintf(info, "received from %s:%s\n", (rcv + 8), (rcv + 32));
			break;
		case 32:
			//sprintf(info, "talk to %s failed\n", (lastmsg + 8));
			break;
		case 122:
			sprintf(info, "other info %d\n", rcv[1]);
			break;
		case 123:
			sprintf(info, "password changed successfully\n");
			break;
		default:
			sprintf(info, "other info %d\n", rcv[1]);
			break;
		}
		dlg->m_timeStatus.SetWindowText(info);
		LeaveCriticalSection(&lpr->wrcon);
	} while (1);
};

int CIMhideWndDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO: Add your specialized creation code here

	//����������߶�
	CWnd* p = this->FindWindow("Shell_TrayWnd", NULL);
	if (p != NULL)
	{
		CRect tRect;
		p->GetWindowRect(tRect);
		m_taskBarHeight = tRect.Height();
	}

	//�޸ķ��ʹ����������������ʾ
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	//ȥ���رհ���(����뻭3�������Ļ�)
	//ModifyStyle(WS_SYSMENU,NULL);
	//��ñ�Ե�߶ȺͿ��
	m_edgeHeight = GetSystemMetrics(SM_CYEDGE);
	m_edgeWidth = GetSystemMetrics(SM_CXFRAME);

	//��������
	int err = InitChat("127.0.0.1");
	StartChat(err, get_Rtn);
	CloseChat();
	return 0;
}

BOOL CIMhideWndDlg::SetWindowPos(const CWnd* pWndInsertAfter, LPCRECT pCRect, UINT nFlags)
{
	return CDialog::SetWindowPos(pWndInsertAfter, pCRect->left, pCRect->top,
		pCRect->right - pCRect->left, pCRect->bottom - pCRect->top, nFlags);
}

LRESULT CIMhideWndDlg::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CString str;
	str.Format("Mouse (%d,%d)", point.x, point.y);
	//GetDlgItem(IDC_TIMER)->SetWindowText(str);
	if (m_hideMode != HM_NONE && !m_isSetTimer &&
		//��ֹ��곬����Ļ�ұ�ʱ���ұ����������˸
		point.x < GetSystemMetrics(SM_CXSCREEN) + INFALTE)
	{   //������ʱ,����Ǵ�����״̬����ʾ״̬����Timer
		SetTimer(1, CM_ELAPSE, NULL);
		m_isSetTimer = TRUE;

		m_hsFinished = FALSE;
		m_hiding = FALSE;
		SetTimer(2, HS_ELAPSE, NULL); //������ʾ����
	}
	return CDialog::OnNcHitTest(point);
}

void CIMhideWndDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1)
	{
		POINT curPos;
		GetCursorPos(&curPos);

		CString str;
		str.Format("Timer On\n\r(%d,%d)", curPos.x, curPos.y);
		//GetDlgItem(IDC_TIMER)->SetWindowText(str);

		CRect tRect;
		//��ȡ��ʱ���ڴ�С
		GetWindowRect(tRect);
		//����tRect,�Դﵽ����뿪���ڱ���һ������Ŵ����¼�
		tRect.InflateRect(INFALTE, INFALTE);

		if (!tRect.PtInRect(curPos)) //�������뿪���������
		{
			KillTimer(1); //�رռ�����Timer
			m_isSetTimer = FALSE;
			//GetDlgItem(IDC_TIMER)->SetWindowText("Timer Off");

			m_hsFinished = FALSE;
			m_hiding = TRUE;
			SetTimer(2, HS_ELAPSE, NULL); //������������
		}
	}

	if (nIDEvent == 2)
	{
		if (m_hsFinished) //�����������ʾ���������ر�Timer	  
			KillTimer(2);
		else
			m_hiding ? DoHide() : DoShow();
	}

	CDialog::OnTimer(nIDEvent);
}

void CIMhideWndDlg::OnSysCommand(UINT nID, LPARAM lparam)
{
	if (nID == SC_CLOSE)
	{
		DestroyWindow();
		//AfxDump << "OnSysCommand\n";
	}
	CDialog::OnSysCommand(nID, lparam);
}

void CIMhideWndDlg::OnMoving(UINT fwSide, LPRECT pRect)
{
	FixMoving(fwSide, pRect); //����pRect
	CDialog::OnMoving(fwSide, pRect);
}

void CIMhideWndDlg::DoHide()
{
	if (m_hideMode == HM_NONE)
		return;

	CRect tRect;
	GetWindowRect(tRect);

	INT height = tRect.Height();
	INT width = tRect.Width();

	INT steps = 0;

	switch (m_hideMode)
	{
	case HM_TOP:
		steps = height / HS_STEPS;
		tRect.bottom -= steps;
		if (tRect.bottom <= m_edgeWidth)
		{   //����԰�����һ���滻����� ...+=|-=steps �ﵽȡ������Ч��
			//���õİ취����Ӹ�BOOLֵ������,����caseͬ��.
			tRect.bottom = m_edgeWidth;
			m_hsFinished = TRUE;  //������ع���
		}
		tRect.top = tRect.bottom - height;
		break;
	case HM_BOTTOM:
		steps = height / HS_STEPS;
		tRect.top += steps;
		if (tRect.top >= (GetSystemMetrics(SM_CYSCREEN) - m_edgeWidth))
		{
			tRect.top = GetSystemMetrics(SM_CYSCREEN) - m_edgeWidth;
			m_hsFinished = TRUE;
		}
		tRect.bottom = tRect.top + height;
		break;
	case HM_LEFT:
		steps = width / HS_STEPS;
		tRect.right -= steps;
		if (tRect.right <= m_edgeWidth)
		{
			tRect.right = m_edgeWidth;
			m_hsFinished = TRUE;
		}
		tRect.left = tRect.right - width;
		tRect.top = -m_edgeHeight;
		tRect.bottom = GetSystemMetrics(SM_CYSCREEN) - m_taskBarHeight;
		break;
	case HM_RIGHT:
		steps = width / HS_STEPS;
		tRect.left += steps;
		if (tRect.left >= (GetSystemMetrics(SM_CXSCREEN) - m_edgeWidth))
		{
			tRect.left = GetSystemMetrics(SM_CXSCREEN) - m_edgeWidth;
			m_hsFinished = TRUE;
		}
		tRect.right = tRect.left + width;
		tRect.top = -m_edgeHeight;
		tRect.bottom = GetSystemMetrics(SM_CYSCREEN) - m_taskBarHeight;
		break;
	default:
		break;
	}

	SetWindowPos(&wndTopMost, tRect);
}

void CIMhideWndDlg::DoShow()
{
	if (m_hideMode == HM_NONE)
		return;

	CRect tRect;
	GetWindowRect(tRect);
	INT height = tRect.Height();
	INT width = tRect.Width();

	INT steps = 0;

	switch (m_hideMode)
	{
	case HM_TOP:
		steps = height / HS_STEPS;
		tRect.top += steps;
		if (tRect.top >= -m_edgeHeight)
		{   //���԰�����һ���滻����� ...+=|-=steps �ﵽȡ������Ч��
			//���õİ취����Ӹ�BOOLֵ������,����caseͬ��.
			tRect.top = -m_edgeHeight;
			m_hsFinished = TRUE;  //�����ʾ����
		}
		tRect.bottom = tRect.top + height;
		break;
	case HM_BOTTOM:
		steps = height / HS_STEPS;
		tRect.top -= steps;
		if (tRect.top <= (GetSystemMetrics(SM_CYSCREEN) - height))
		{
			tRect.top = GetSystemMetrics(SM_CYSCREEN) - height;
			m_hsFinished = TRUE;
		}
		tRect.bottom = tRect.top + height;
		break;
	case HM_LEFT:
		steps = width / HS_STEPS;
		tRect.right += steps;
		if (tRect.right >= width)
		{
			tRect.right = width;
			m_hsFinished = TRUE;
		}
		tRect.left = tRect.right - width;
		tRect.top = -m_edgeHeight;
		tRect.bottom = GetSystemMetrics(SM_CYSCREEN) - m_taskBarHeight;
		break;
	case HM_RIGHT:
		steps = width / HS_STEPS;
		tRect.left -= steps;
		if (tRect.left <= (GetSystemMetrics(SM_CXSCREEN) - width))
		{
			tRect.left = GetSystemMetrics(SM_CXSCREEN) - width;
			m_hsFinished = TRUE;
		}
		tRect.right = tRect.left + width;
		tRect.top = -m_edgeHeight;
		tRect.bottom = GetSystemMetrics(SM_CYSCREEN) - m_taskBarHeight;
		break;
	default:
		break;
	}

	SetWindowPos(&wndTopMost, tRect);
}

void CIMhideWndDlg::FixMoving(UINT fwSide, LPRECT pRect)
{
	POINT curPos;
	GetCursorPos(&curPos);
	INT screenHeight = GetSystemMetrics(SM_CYSCREEN);
	INT screenWidth = GetSystemMetrics(SM_CXSCREEN);
	INT height = pRect->bottom - pRect->top;
	INT width = pRect->right - pRect->left;

	if (curPos.y <= INTERVAL)
	{   //ճ�����±�
		pRect->bottom = height - m_edgeHeight;
		pRect->top = -m_edgeHeight;
		m_hideMode = HM_BOTTOM;
	}
	else if (curPos.y >= (screenHeight - INTERVAL - m_taskBarHeight))
	{   //ճ�����ϱ�
		pRect->top = screenHeight - m_taskBarHeight - height;
		pRect->bottom = screenHeight - m_taskBarHeight;
		m_hideMode = HM_TOP;
	}
	else if (curPos.x < INTERVAL)
	{	//ճ�������	
		if (!m_isSizeChanged)
		{
			CRect tRect;
			GetWindowRect(tRect);
			m_oldWndHeight = tRect.Height();
		}
		pRect->right = width;
		pRect->left = 0;
		pRect->top = -m_edgeHeight;
		pRect->bottom = screenHeight - m_taskBarHeight;
		m_isSizeChanged = TRUE;
		m_hideMode = HM_LEFT;
	}
	else if (curPos.x >= (screenWidth - INTERVAL))
	{   //ճ�����ұ�
		if (!m_isSizeChanged)
		{
			CRect tRect;
			GetWindowRect(tRect);
			m_oldWndHeight = tRect.Height();
		}
		pRect->left = screenWidth - width;
		pRect->right = screenWidth;
		pRect->top = -m_edgeHeight;
		pRect->bottom = screenHeight - m_taskBarHeight;
		m_isSizeChanged = TRUE;
		m_hideMode = HM_RIGHT;
	}
	else
	{   //��ճ��
		if (m_isSizeChanged)
		{   //�������������,���ϳ��������ԭ����С
			//��"�϶�����ʾ����������"ֻ�й�դ���ԭ����С
			pRect->bottom = pRect->top + m_oldWndHeight;
			m_isSizeChanged = FALSE;
		}
		if (m_isSetTimer)
		{   //���Timer������,��ر�֮
			if (KillTimer(1) == 1)
				m_isSetTimer = FALSE;
		}
		m_hideMode = HM_NONE;
		GetDlgItem(IDC_TIMER)->SetWindowText("Timer off");
	}
}
