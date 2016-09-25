//
#include<iostream>
#include<winsock2.h>
#include<windows.h> 
#include"sql\sqldb.h"
#define HAVE_STRUCT_TIMESPEC
//#define MY_WSDL
#include"pthreads.2\pthread.h"
#include"soap\web.nsmap"
#include"thdpool\pthread_pool.h"
#pragma comment(lib, "pthreads.2/lib/x64pthreadVC2.lib") 
#pragma comment(lib, "WS2_32.lib")

//����ȫ�ֱ����Ķ���
#define  BACKLOG (64)  
#define  MAX_THR (8)   
#define  MAX_QUEUE (1024)

pthread_mutex_t queue_lock;//������
pthread_cond_t  queue_noti;//��������
SOAP_SOCKET     queue[MAX_QUEUE];//�������
int head = 0, tail = 0;          //����ͷ����β��ʼ��         
void *process_queue(void *);     //�߳���ں���
int enqueue(SOAP_SOCKET, unsigned long ip); //����к���
unsigned long dequeue_ip();
SOAP_SOCKET dequeue(void); //�����к���
static unsigned long ips[MAX_QUEUE];
int http_get(struct soap *soap);
int http_post(struct soap *soap, const char *endpoint, const char *host, int port, const char *path, const char *action, size_t count);

int main(int argc, char** argv)
{
#ifdef MY_DEBUG
	argc = 3; argv[1] = "88";
#endif // MY_DEBUG
	struct soap Soap;
	//��ʼ������ʱ����
	soap_init(&Soap);
	Soap.fget = http_get;
#ifndef MY_WSDL
	Soap.fpost = http_post;
#endif // !MY_WSDL
	//����UTF-8����
	soap_set_mode(&Soap, SOAP_C_UTFSTRING);
	soap_set_namespaces(&Soap, namespaces);
	//���û�в���������CGI������
	if (argc <2)
	{
		//CGI ���������󣬵��߳�
		soap_serve(&Soap);
		//������л������ʵ��
		soap_destroy(&Soap);
		//������л�������
		soap_end(&Soap);
	}
	else
	{
		struct soap * soap_thr[MAX_THR];
		pthread_t tid[MAX_THR];
		int i, port = atoi(argv[1]);
		SOAP_SOCKET m, cs;
		//��������������ʼ��
		pthread_mutex_init(&queue_lock, NULL);
		pthread_cond_init(&queue_noti, NULL);
		//�󶨷���˿�
		m = soap_bind(&Soap, NULL, port, BACKLOG);
		//ѭ��ֱ�������׽��ֺϷ�
		int vilid = 0;
		while (!soap_valid_socket(m))
		{
			if (vilid == 0)
				fprintf(stderr, "Bind port error! \n");
			m = soap_bind(&Soap, NULL, port, BACKLOG);
			vilid++;
		}
		fprintf(stderr, "Socket�˿ں�:%s \n", argv[1]);

		//���ɷ����߳�
		for (i = 0; i <MAX_THR; i++)
		{
			soap_thr[i] = soap_copy(&Soap);
			fprintf(stderr, "\tthread %d.\n", i);
			pthread_create(&tid[i], NULL, (void*(*)(void*))process_queue, (void*)soap_thr[i]);
		}
		int j = 0;
		for (;;)
		{
			//���ܿͻ��˵�����
			cs = soap_accept(&Soap);
			if (!soap_valid_socket(cs))
			{
				if (Soap.errnum)
				{
					soap_print_fault(&Soap, stderr);
					continue;
				}
				else
				{
					fprintf(stderr, "Server timed out \n");
					break;
				}
			}
			//�ͻ��˵�IP��ַ
			fprintf(stderr, "Accepted connection from IP= %d.%d.%d.%d socket = %d \n",
				(int)(((Soap.ip) >> 24) && 0xFF), (int)(((Soap.ip) >> 16) & 0xFF), (int)(((Soap.ip) >> 8) & 0xFF), (int)((Soap.ip) & 0xFF), (int)(Soap.socket));
			//������׽��ֽ�����У��������������ѭ���ȴ�
			while (enqueue(cs, ips[j]) == SOAP_EOM)
				Sleep(1000);
			j++;
			if (j >= MAX_THR)
				j = 0;
		}
		//����������������
		for (i = 0; i < MAX_THR; i++)
		{
			while (enqueue(SOAP_INVALID_SOCKET, ips[i]) == SOAP_EOM)
			{
				Sleep(1000);
			}
		}
		for (i = 0; i< MAX_THR; i++)
		{
			fprintf(stderr, "Waiting for thread %d to terminate ..\n", i);
			pthread_join(tid[i], NULL);
			fprintf(stderr, "terminated \n");
			soap_done(soap_thr[i]);
			free(soap_thr[i]);
		}
		pthread_mutex_destroy(&queue_lock);
		pthread_cond_destroy(&queue_noti);
	}
	//��������ʱ�Ļ���
	soap_done(&Soap);
	return 0;
}

void * process_queue(void * soap)
{
	struct soap * tsoap = (struct soap *)soap;
	for (;;)
	{
		tsoap->socket = dequeue();
		tsoap->ip = dequeue_ip();
		if (!soap_valid_socket(tsoap->socket))
		{
#ifdef DEBUG  
			fprintf(stderr, "Thread %d terminating\n", (int)(long)tsoap->user);
#endif
			break;
		}
		soap_serve(tsoap);
		soap_destroy(tsoap);
		soap_end(tsoap);
	}
	return NULL;
}

//����в���
int enqueue(SOAP_SOCKET sock, unsigned long ip)
{
	int status = SOAP_OK;
	int next;
	pthread_mutex_lock(&queue_lock);
	next = (tail + 1) % MAX_QUEUE;
	if (next >= MAX_QUEUE)
		next = 0;
	if (next == head)
		status = SOAP_EOM;//������
	else
	{
		queue[tail] = sock;
		ips[tail] = ip;
		tail = next;
		pthread_cond_signal(&queue_noti);
	}
	pthread_mutex_unlock(&queue_lock);
	return status;
}

//�����в���
SOAP_SOCKET dequeue()
{
	SOAP_SOCKET sock;
	pthread_mutex_lock(&queue_lock);
	while (head == tail)
	{
		pthread_cond_wait(&queue_noti, &queue_lock);
	}
	sock = queue[head++];
	if (head >= MAX_QUEUE)
	{
		head = 0;
	}
	pthread_mutex_unlock(&queue_lock);
	return sock;
}

unsigned long dequeue_ip()
{
	unsigned long ip;
	int num = 0;
	if (head == 0)
		num = MAX_QUEUE - 1;
	else
		num = head - 1;
	ip = ips[num];
	return ip;
}
#ifdef MY_WSDL
int http_get(struct soap *soap)
#else
int http_post(struct soap *soap, const char *endpoint, const char *host, int port, const char *path, const char *action, size_t count)
#endif
{
	FILE* fd = 0;
#ifndef MY_WSDL
	// ����WSDLʱ��������Ӧ�ļ�
	// ��ȡ�����wsdl�ļ���
	std::string fielPath(soap->path);
	size_t pos = fielPath.rfind("/");
	std::string fileName(fielPath, pos + 1);

	// ��?�滻Ϊ.
	size_t dotPos = fileName.rfind("?");
	if (dotPos == -1)
	{
		return 404;
	}
	fileName.replace(dotPos, 1, ".");
	// ��WSDL�ļ�׼������
	fopen_s(&fd, fileName.c_str(), "rb");
#else
	char* s = strchr(soap->path, '?');
	if (!s || strcmp(s, "?wsdl"))
		return SOAP_GET_METHOD;
	fopen_s(&fd, "web.wsdl", "rb");
#endif // MY_WSDL
	if (!fd)
		// HTTP not found error
		return 404;
	// HTTP header with text/xml content
	soap->http_content = "text/xml";
	soap_response(soap, SOAP_FILE);
	for (;;)
	{
		// ��fd�ж�ȡ����
		size_t r = fread(soap->tmpbuf, 1, sizeof(soap->tmpbuf), fd);
		if (!r)
		{
			break;
		}
		if (soap_send_raw(soap, soap->tmpbuf, r))
		{
			// can't send, but little we can do about that
			break;
		}
	}
	fclose(fd);
	soap_end_send(soap);
	return
#ifdef MY_WSDL
		SOAP_OK;
#else
		http_get(soap);
}

int http_get(struct soap *soap)
{
	soap_response(soap, SOAP_HTML);
	soap_send(soap, "<html>Hello I'm WebService.</html>");
	soap_end_send(soap);
	return SOAP_OK;
#endif
}

int api__login_by_key(struct soap *soap, xsd_string usr, xsd_string psw, struct api__result &flag)
{
	int key = 0;
	flag.email = (char*)soap_malloc(soap, 32);
	if (!(strcmp(usr, "0") || strcmp(psw, "0")))
	{
		flag.email = "OK";
		key = 1;
	}
	printf(flag.email);
	return key;
}

int api__encrypt(struct soap *soap, char* input, char** output)
{
	sqldb();
	return 0;
}