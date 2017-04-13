#ifndef IM_IMCHAT_H
#define IM_IMCHAT_H
#pragma comment(lib, "WS2_32.lib")

#ifdef _TESTDLG_
// /clrģʽ������ͷ�ļ����ᵼ��
// error LNK2022 : Ԫ���ݲ���ʧ��(8013118D) : �ظ�����(group_filter)�еĲ�����Ϣ��һ�� : (0x0200020b)��
// 2>LINK : fatal error LNK1255 : ����Ԫ���ݴ�������ʧ��
#include <WS2tcpip.h>
#else
#include <winsock2.h>
#endif
#include <process.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <map>
#pragma warning (disable:4477)

typedef struct IMUSR {
	char addr[MAX_PATH];
	char comm[64];
	char psw[32];
	char usr[32];
	int err;
} st_imusr;

struct CHATMSG
{
	char lastuser[256];
	char lastgroup[256];
};

struct LPR
{
	SOCKET sock;
	CRITICAL_SECTION wrcon;
	void* dlg;
	CHATMSG *msg;
};

int InitChat(char argv[] = "127.0.0.1" , int argc = 2);
int StartChat(int err, void(*func)(void*));
void CloseChat();

#endif