#include "CTPclient.h"
#include "ChannelCollector.h"

extern bool net_exit;
struct st_TThostFtdc STFTDC; 
CThostFtdcTraderApi *TRDAPI;

CTPclient::CTPclient() {}

CTPclient::~CTPclient() {}

unsigned int __stdcall CTPclient::SimpleClient(void* P)
{
	//�߳�Ҫ���õĺ���
	int err;
	int j = 0;
	SOCKET clientSock;
	WSADATA wsaData;//WSAata�洢ϵͳ���صĹ���WinSocket����Ϣ
	CTPclient *m_ctp = new CTPclient;
	MyOglDrawDlg* m_hWnd = (MyOglDrawDlg*)P;
	if (m_hWnd->ctpIP == NULL)return NULL;
	OGLKview::Point pnt = { 0,0 };
	int loo[8];
	CString Str;
	CString Left;
	char Buf[50];
	char buffer[10];
	WCHAR LPCT[50];
	SOCKADDR_IN addrSrv;
	WORD wVersionRequested;
	//���������޷��Ų���
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		::PostMessage(m_hWnd->m_hWnd, WM_MSG_BOX, 0,
			(LPARAM)m_ctp->Ogl.index.AllocBuffer("����������δ������"));
		return -1;
	}
	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return -2;
	}
	clientSock = socket(AF_INET, SOCK_STREAM, 0);//AF_INET ��ʾTCP ����
	addrSrv.sin_addr.S_un.S_addr = inet_addr((const char*)m_hWnd->ctpIP);//������ַ,�������ڱ�������
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6001);//���ö˿ں�
	connect(clientSock, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));//���ӷ�����
	while (1)
	{
		if (net_exit) { m_hWnd->MessageBox("exit"); return -1; };
		if (recv(clientSock, Buf, 50, 0) == SOCKET_ERROR)
		{
			if (m_hWnd->m_hWnd != NULL)
				::PostMessage(m_hWnd->m_hWnd, WM_MSG_BOX, 0,
				(LPARAM)m_ctp->Ogl.index.AllocBuffer("����ʧ�ܣ�"));
			return -3;
		}//�������ݲ���䵽�б�
		MultiByteToWideChar(CP_ACP, 0, Buf, strlen(Buf) + 1, LPCT, sizeof(LPCT) / sizeof(LPCT[0]));
		m_ctp->Ogl.DrawKtext(Buf, pnt, 20, { 1,1,1 });
		loo[0] = Str.Find(_T("."));//���ҵ�һ��"."λ��
		Left = Str.Left(loo[0]);	//��","��ߵ�ֵȡ��
		Str.Format(_T("%s"), LPCT);
		_ultoa_s(GetCurrentThreadId(), buffer, 10);//��ǰ�߳�id
		send(clientSock, buffer, strlen(buffer) + 1, 0);//��������
		j++;
	}
	if (m_ctp != nullptr)
		delete m_ctp;
	closesocket(clientSock);
	WSACleanup();
	return 0;
}

unsigned int __stdcall CTPclient::TradeMarket(void* P)
{
	MyOglDrawDlg* m_hWnd = (MyOglDrawDlg*)P;
	TradeChannel trade;
	CString str;
	if (AllocConsole())
		freopen("CONOUT$", "w", stderr);
	TRDAPI = CThostFtdcTraderApi::CreateFtdcTraderApi(STFTDC.FLOW_PATH);
	TradeChannel *tdChnl = new TradeChannel();
	TRDAPI->RegisterSpi((CThostFtdcTraderSpi*)tdChnl);
	TRDAPI->RegisterFront(STFTDC.FRONT_ADDR);
	TRDAPI->SubscribePrivateTopic(THOST_TERT_QUICK);
	TRDAPI->SubscribePublicTopic(THOST_TERT_QUICK);
	TRDAPI->Init();
	TRDAPI->Join();
	str.Format(_T("%d"), tdChnl->getRtn());
	CTPclient *m_ctp = new CTPclient;
	if (m_hWnd->m_hWnd != NULL)
		::PostMessage(m_hWnd->m_hWnd, WM_MSG_BOX, 0,
		(LPARAM)m_ctp->Ogl.index.AllocBuffer(str));
	delete tdChnl, m_ctp;
	return 0;
}

