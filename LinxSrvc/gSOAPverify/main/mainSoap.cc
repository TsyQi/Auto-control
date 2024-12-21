//
#include"../../include/String_-inl.h"
#include"mainSoap.h"
#include"../sql/sqlDbReq.h"
#include"../sys/status.h"
#ifdef NS_DBG
#define SOAP_DEBUG
#endif
#include"../soap/soapStub.h"
#include"../soap/myweb.nsmap"

pthread_mutex_t queue_lock;      // 队列锁
pthread_cond_t  queue_cond;      // 条件变量
SOAP_SOCKET     queue[MAX_QUEUE];// 数组队列
int head = 0, tail = 0;          // 队列头队列尾初始化
void* process_queue(void*);      // 线程入口函数
int enqueue(SOAP_SOCKET, unsigned long ip); // 入队列函数
SOAP_SOCKET dequeue(void);       // 出队列函数
void dequeue(unsigned int&);
static unsigned long ips[MAX_QUEUE];

void* process_queue(void* soap)
{
    if (soap == nullptr)
        return nullptr;
    struct soap* serv = (struct soap*)soap;
    for (;;) {
        serv->socket = dequeue();
        dequeue(serv->ip);
        if (!soap_valid_socket(serv->socket)) {
            fprintf(stderr, "Thread %d terminating\n", (int)(long)serv->user);
            break;
        }
        soap_serve(serv);
        soap_destroy(serv);
        soap_end(serv);
    }
    return NULL;
}

int enqueue(SOAP_SOCKET sock, unsigned long ip)
{
    int status = SOAP_OK;
    int next;
    pthread_mutex_lock(&queue_lock);
    next = (tail + 1) % MAX_QUEUE;
    if (next >= MAX_QUEUE)
        next = 0;
    // 队列满
    if (next == head)
        status = SOAP_EOM;
    else {
        queue[tail] = sock;
        ips[tail] = ip;
        tail = next;
        pthread_cond_signal(&queue_cond);
    }
    pthread_mutex_unlock(&queue_lock);
    return status;
}

SOAP_SOCKET dequeue()
{
    SOAP_SOCKET sock;
    pthread_mutex_lock(&queue_lock);
    while (head == tail) {
        pthread_cond_wait(&queue_cond, &queue_lock);
    }
    sock = queue[head++];
    if (head >= MAX_QUEUE) {
        head = 0;
    }
    pthread_mutex_unlock(&queue_lock);
    return sock;
}

void dequeue(unsigned int& ip)
{
    int j = 0;
    if (head == 0)
        j = MAX_QUEUE - 1;
    else
        j = head - 1;
    ip = ips[j];
}

int http_get(struct soap* soap)
#ifdef NS_HTTPPOST
{
    soap_response(soap, SOAP_HTML);
    soap_send(soap, "<html>Hello I'm WebService.</html>");
    soap_end_send(soap);
    return SOAP_OK;
}
int http_post(struct soap* soap, const char* endpoint, const char* host, int port, const char* path, const char* action, size_t count)
#endif
{
    FILE* stream = 0;
#ifdef NS_HTTPPOST
    // 请求WSDL时，传送相应文件
    // 获取请求的wsdl文件
    std::string filePath(soap->path);
    size_t pos = filePath.rfind("/");
    std::string fileName(filePath, pos + 1);
    // 将?替换为.
    size_t dotPos = fileName.rfind("?");
    if (dotPos == std::string::npos)
        return 404;
    fileName.replace(dotPos, 1, ".");
    // 打开WSDL文件准备拷贝
    stream = fopen(fileName.c_str(), "rb");
#else
    char* s = strchr(soap->path, '?');
    if (!s || strcmp(s, "?wsdl"))
        return SOAP_GET_METHOD;
    stream = fopen("myweb.wsdl", "rb");
#endif // NS_HTTPPOST
    if (!stream) {
        // HTTP not found error
        return 404;
    }
    // HTTP header with text/xml content
    soap->http_content = "text/xml";
    soap_response(soap, SOAP_FILE);
    for (;;) {
        // 从stream中读取数据
        size_t r = fread(soap->tmpbuf, 1, sizeof(soap->tmpbuf), stream);
        if (!r)break;
        if (soap_send_raw(soap, soap->tmpbuf, r)) {
            fprintf(stderr, "can't send raw data of tmpbuf.\n");
            break;
        }
    }
    fclose(stream);
    soap_end_send(soap);
    return
#ifdef NS_HTTPPOST
        http_get(soap);
#else
        SOAP_OK;
#endif
}

int main_server(int argc, char** argv)
{
    if (argc > 1 && argv[1] == nullptr) {
        std::cout << "Please type an argument as port eg. '\033[45m" << argv[0] << " 8080\033[0m'" << std::endl;
        kill(getppid(), SIGALRM);
        return -1;
    }
    struct soap Soap;
    // 初始化运行时环境
    soap_init(&Soap);
#ifdef NS_HTTPPOST
    Soap.fpost = http_post;
#else
    Soap.fget = http_get;
#endif // !NS_HTTPPOST
    // 设置UTF-8编码
    soap_set_mode(&Soap, SOAP_C_UTFSTRING);
    soap_set_namespaces(&Soap, namespaces);
    struct timespec ts = { 0, 50000 };
    // 没有参数，作为CGI程序处理
    if (argc < 2) {
        // CGI 风格服务请求，单线程
        soap_serve(&Soap);
        // 清除序列化类实例
        soap_destroy(&Soap);
        // 清除序列化数据
        soap_end(&Soap);
    } else {
        struct soap* soap_thr[MAX_THR];
        pthread_t tid[MAX_THR];
        // 锁和条件变量初始化
        pthread_mutex_init(&queue_lock, NULL);
        pthread_cond_init(&queue_cond, NULL);
        // 绑定服务端口
        int port = atoi(argv[1]);
        SOAP_SOCKET m = soap_bind(&Soap, NULL, port, BACKLOG);
        int valid = 0;
        // 循环绑定直至服务套接字合法
        while (!soap_valid_socket(m)) {
            if (valid == 0) {
                fprintf(stderr, "Bind PORT(%d) \033[31merror\033[0m! \n", port);
                exit(1);
            }
            m = soap_bind(&Soap, NULL, port, BACKLOG);
            valid++;
        }
        fprintf(stdout, "======== Socket Server Port: %d ========\n", port);
        // 生成服务线程
        for (int i = 0; i < MAX_THR; i++) {
            soap_thr[i] = soap_copy(&Soap);
            fprintf(stderr, " ++++\tthread %d.\n", i);
            pthread_create(&tid[i], NULL, (void* (*)(void*))process_queue, (void*)soap_thr[i]);
            nanosleep(&ts, NULL);
        }
        int j = 0;
        static int no = 0;
        for (;;) {
            // 接受客户端连接
            SOAP_SOCKET sock = soap_accept(&Soap);
            if (!soap_valid_socket(sock)) {
                if (Soap.errnum) {
                    soap_print_fault(&Soap, stderr);
                    continue;
                } else {
                    fprintf(stderr, "Server timed out \n");
                    break;
                }
            }
            no++;
            // 客户端IP地址
            fprintf(stdout, "\033[32mAccepted\033[0m \033[1mREMOTE\033[0m connection. IP = \033[33m%d.%d.%d.%d\033[0m, socket = %d, log(%d) \n", \
                (int)(((Soap.ip) >> 24) & 0xFF), (int)(((Soap.ip) >> 16) & 0xFF), (int)(((Soap.ip) >> 8) & 0xFF), \
                (int)((Soap.ip) & 0xFF), (int)(Soap.socket), no);
            // 套接字入队，如果队列已满则循环等待
            while (enqueue(sock, ips[j]) == SOAP_EOM) {
                ts.tv_nsec = 100000;
                nanosleep(&ts, NULL);
            }
            j++;
            if (j >= MAX_THR)
                j = 0;
        }
        // 清理服务
        for (int i = 0; i < MAX_THR; i++) {
            while (enqueue(SOAP_INVALID_SOCKET, ips[i]) == SOAP_EOM) {
                ts.tv_nsec = 100000;
                nanosleep(&ts, NULL);
            }
        }
        for (int i = 0; i < MAX_THR; i++) {
            fprintf(stderr, "Waiting for thread %d to terminate ..\n", i);
            pthread_join((pthread_t)tid[i], NULL);
            fprintf(stderr, "terminated \n");
            soap_done(soap_thr[i]);
            free(soap_thr[i]);
        }
        pthread_mutex_destroy(&queue_lock);
        pthread_cond_destroy(&queue_cond);
    }
    // 分离运行时环境
    soap_done(&Soap);
    return 0;
}

int api__trans(struct soap* soap, char* msg, char* rtn[])
{
    String_ str;
    struct PARAM {
        char key[16];
        char value[16];
    };
    static int j = 0;
    struct PARAM params[8];
    char* text[8] = { msg };
    int neq = str.char_count_(*text, '=');
    if (neq < 0 || msg == NULL) {
        memcpy(text[0], "request uri error!", 19);
        return -1;
    }
    char* token = strtok(msg, "@&");
    printf("GET:[%s][%d]\n", msg, neq);
    for (int i = 0; i < 8; i++) {
        text[i] = (char*)malloc(64);
        memset(text[0], 0, 64);
    }
    if (token != NULL && (memcmp(token, "trans", 6) != 0)) {
        memcpy(text[0], "illegal command!", 17);
        return -2;
    }
    char* tmp[4] = { NULL };
    while (token != NULL) {
        tmp[j] = token;
        if (strstr(tmp[j], "=") != NULL) {
            str.strcut_((unsigned char*)tmp[j], '=', params[j].key, params[j].value);
            j++;
        }
        token = strtok(NULL, "&");
    }
    for (int k = 0; k < j; k++) {
        sprintf(text[k], "Param(%d): %s[%s]", k, params[k].key, params[k].value);
        cout << text[k] << endl;
    }
    j = 0;
    token = NULL;
    *rtn = *text;
    return 0;
}

int api__get_server_status(struct soap* soap, xsd_string req, xsd_string& rsp)
{
    if (req != NULL && memcmp(req, "1000", 5) == 0) {
        st_sys ss = { 0 };
        char gt[8];
        get_mem_stat(const_cast<char*>("localhost"), &ss);
        rsp = gcvt(100.f * ss.mem_free / ss.mem_all, 5, gt);
        cout << req << ": " << rsp << endl;
    }
    return 0;
}

int api__login_by_key(struct soap*, char* usr, char* psw, struct api__ArrayOfEmp2& sch)
{
    sch.rslt.flag = -3;
    if (usr != nullptr && psw != nullptr) {
        struct queryParam param;
        param.user.acc = usr;
        param.user.psw = psw;
        if (sqlQuery(param) != 0) {
            param.msg.flag = false;
            sch.rslt.flag = -2;
            printf("[OUT]:\tqueryParam.rslt is null.\n");
        }
        if (param.msg.flag) {
            sch.rslt.email = param.msg.email;
            sch.rslt.tell = param.msg.tell;
            sch.rslt.flag = 200;
            printf("[OUT]:\temail:%s\t", sch.rslt.email);
            if (sch.rslt.tell[0] != '\0')
                cout << "tell:" << sch.rslt.tell;
            cout << endl;
        }
    }
    return sch.rslt.flag;
}

int main(int argc, char* argv[])
{
    if (fork() == 0) {
        main_server(argc, argv);
    }
    return 0;
}
