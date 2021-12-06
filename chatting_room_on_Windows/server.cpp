#include <WinSock2.h>//��������socketͨ�ŵ�winsock2ͷ�ļ�
#include <stdio.h>
#include <stdlib.h>
#include <windows.h> //�����ṩ���̷߳���
#pragma comment(lib, "Ws2_32.lib")
//����ռ�õĶ˿�
#define PORT 12345
//���ÿͻ��˵��������
#define CLIENT_MAX_NUM 128
//buff�Ĵ�С
#define BUFF_LEN 80

SOCKET socketServer; //�������׽���
HANDLE mul_thread[CLIENT_MAX_NUM];	//�����߳�
int clent_num = 0;			//�ͻ����׽��ּ���
int socketClents[CLIENT_MAX_NUM];	//�ͻ����׽���
//HANDLE g_hEvent; //������
DWORD WINAPI ThreadFun(LPVOID threadFunPara);	//�߳�ִ�к���
void send_msg(char* msg, int len);			//��Ϣ���ͺ���

BOOL WINAPI cls(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		//�ͷ����о��
		for (u_int i = 0; i < CLIENT_MAX_NUM; i++)
		{
			closesocket(socketClents[i]);	
		}
		closesocket(socketServer);
		WSACleanup();//���������
		
	}

	return TRUE;
}

int main() {
	SetConsoleCtrlHandler(cls, TRUE);
	//1���������
	WORD wdVersion = MAKEWORD(2, 2);
	WSADATA wdScoket;

	int nRes = WSAStartup(wdVersion, &wdScoket);
	//�������
	if (0 != nRes)
	{
		switch (nRes)
		{
		case WSASYSNOTREADY:
			printf("�������������������");
			break;
		case WSAVERNOTSUPPORTED:
			printf("������������������");
			break;
		case WSAEINPROGRESS:
			printf("�������������������");
			break;
		case WSAEPROCLIM:
			printf("����������������Ӵﵽ���޻��������رղ���Ҫ���");
			break;
		case WSAEFAULT:
			printf("�����������������");
			break;
		}
		return 0;

	}
	//printf("�������ɹ���\n");

	//2��У��汾��
	int a = *((char*)&wdVersion);
	int b = *((char*)&wdVersion + 1);

	if (2 != HIBYTE(wdScoket.wVersion) || 2 != LOBYTE(wdScoket.wVersion))
	{
		printf("�汾�����⣡\n");
		WSACleanup();
		return 0;
	}
	//printf("�汾У��ɹ���\n");

	// 3������SOCKET
	socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == socketServer)
	{
		int err = WSAGetLastError();
		printf("����������SOCKETʧ�ܴ�����Ϊ��%d\n", err);

		//��������⣬���رվ��
		WSACleanup();
		return 0;
	}
	//printf("����������SOCKET�ɹ���\n");

	struct sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(PORT);//��htons�꽫����תΪ�˿ںŵ��޷�������

	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");


	//4���󶨵�ַ��˿�
	if (SOCKET_ERROR == bind(socketServer, (const struct sockaddr*)&si, sizeof(si)))
	{
		int err = WSAGetLastError();//ȡ������
		printf("������bindʧ�ܴ�����Ϊ��%d\n", err);
		closesocket(socketServer);//�ͷ�
		WSACleanup();//���������

		return 0;
	}
	//printf("��������bind�ɹ���\n");

		//5����ʼ����
	if (SOCKET_ERROR == listen(socketServer, CLIENT_MAX_NUM))
	{
		int err = WSAGetLastError();//ȡ������
		printf("����������ʧ�ܴ�����Ϊ��%d\n", err);
		closesocket(socketServer);//�ͷ�
		WSACleanup();//���������

		return 0;
	}

	//printf("�������˼����ɹ���\n");

	sockaddr_in remoteAddr;
	int remoteAddrLen = sizeof(remoteAddr); //
	DWORD dwThreadId;	//�߳�ID
	SOCKET socketClent;//�����ͻ����׽���

	while (TRUE)
	{
		printf("�ȴ����û�����...\n");
		//SOCKET WSAAPI accept(
		//SOCKET   s,
		//	sockaddr* addr,
		//	int* addrlen
		//	);
		//
		//����1��������SOCKET������þ��Ҫ�ȴ��ڼ���״̬���ͻ��˵����Ӷ������������SOCKET�������
		//����2��sockaddr���͵Ľṹ��Ĵ�ַ���ã���ȡ�ͻ��˵ĵ�ַ���˿���Ϣ��
		//����3��int���͵Ĵ�ַ���ã��������2�Ĵ�С��
		socketClent = accept(socketServer, (SOCKADDR*)&remoteAddr, &remoteAddrLen);
		if (INVALID_SOCKET == socketClent)
		{
			int err = WSAGetLastError();//ȡ������
			printf("��ȡ�ͻ��˾��ʧ�ܴ�����Ϊ��%d\n", err);
			closesocket(socketServer);//�ͷ�
			WSACleanup();//���������

			return 0;
		}
		//DWORD WaitForSingleObject(
		//  HANDLE hHandle,
		//	DWORD dwMilliseconds
		//	);
		//����һ��������
		//����������ʱ������Ժ���Ϊ��λ��
		//���dwMillisecondsΪ�㣬��ú�������Զ����״̬���������ء����dwMilliseconds�� INFINITE�������ĳ�ʱ�����Զ�����ȥ��
		//int err = WaitForSingleObject(g_hEvent, INFINITE);
		/*if (err == WAIT_FAILED) {
			printf("���ܳ����������һ�����ϣ�ϵͳ�ڴ治�㡣�����߳����ڵȴ�ͬһ���ж��¼�����ǰ�߳����ڱ���ֹ��hHandle������ĵײ�����ѱ�ɾ���������Ч��");
		}
		else if (err == WAIT_OBJECT_0) {
			printf("ָ�������״̬�ѷ����źš�");
		}
		else if (err == WAIT_TIMEOUT) {
			printf("��ʱ����ѹ������Ҷ����״̬Ϊ���źš�");
		}*/
		/*
		* 	HANDLE WINAPI CreateThread(
			  _In_opt_  LPSECURITY_ATTRIBUTES  lpThreadAttributes,   
			  _In_      SIZE_T                 dwStackSize,
			  _In_      LPTHREAD_START_ROUTINE lpStartAddress,
			  _In_opt_  LPVOID                 threadFunParaeter,
			  _In_      DWORD                  dwCreationFlags,
			  _Out_opt_ LPDWORD                lpThreadId
			);
			��һ������ lpThreadAttributes ��ʾ�߳��ں˶���İ�ȫ���ԣ�һ�㴫��NULL��ʾʹ��Ĭ�����á�
			�ڶ������� dwStackSize ��ʾ�߳�ջ�ռ��С������0��ʾʹ��Ĭ�ϴ�С��1MB����
			���������� lpStartAddress ��ʾ���߳���ִ�е��̺߳�����ַ������߳̿���ʹ��ͬһ��������ַ��
			���ĸ����� threadFunParaeter �Ǵ����̺߳����Ĳ�����
			��������� dwCreationFlags ָ������ı�־�������̵߳Ĵ�����Ϊ0��ʾ�̴߳���֮�������Ϳ��Խ��е��ȣ����ΪCREATE_SUSPENDED���ʾ�̴߳�������ͣ���У����������޷����ȣ�ֱ������ResumeThread()��
			���������� lpThreadId �������̵߳�ID�ţ�����NULL��ʾ����Ҫ���ظ��߳�ID�š�
		*/
		mul_thread[clent_num] = CreateThread(
			NULL,		// Ĭ�ϰ�ȫ����
			0,		// �߳�ջ�ռ��С,Ĭ�ϴ�С��1MB��
			ThreadFun,	// �߳���ڵ�ַ��ִ���̵߳ĺ�����,����߳̿���ʹ��ͬһ��������ַ
			(void*)&socketClent,		// ���������Ĳ���
			0,		// �̴߳���֮�������Ϳ��Խ��е���
			&dwThreadId);	// �����̵߳�ID��
		socketClents[clent_num++] = socketClent;
		//SetEvent(g_hEvent);				/*��������*/

		printf(" %s���ӳɹ�,���߳�IDΪ%d��\r\n", inet_ntoa(remoteAddr.sin_addr), dwThreadId);
	}
	
	/*
	DWORD WaitForMultipleObjects(
		DWORD nCount,
		CONST HANDLE* lpHandles,
		BOOL fWaitAll,
		DWORD dwMilliseconds
	);
	��һ������nCount ָ��lpHandlesָ��������еĶ������������������������Ϊ MAXIMUM_WAIT_OBJECTS��
	�ڶ�������lpHandles ָ������������ָ�롣
	����������fWaitAll ָ���ȴ����͡��˲�����������Ϊ FALSE
	���ĸ�����dwMilliseconds �Ժ���Ϊ��λָ����ʱ�����ֻҪ����ʱ��������ʹ������ bWaitAll����ָ�����������ú���Ҳ�᷵�ء�
	���dwMillisecondsΪ�㣬��ú�������ָ�������״̬���������ء����dwMilliseconds�� INFINITE�������ĳ�ʱ�����Զ�����ȥ��
	*/
	WaitForMultipleObjects(clent_num, mul_thread, FALSE, INFINITE);
	//�ر�ÿһ���߳̾��
	for (int i = 0; i < clent_num; i++)
	{
		CloseHandle(mul_thread[i]);
	}
	// �رռ����׽���
	closesocket(socketServer);

	for (u_int i = 0; i < CLIENT_MAX_NUM; i++)
	{
		closesocket(socketClents[i]);
	}
	// �ͷ�WS2_32��
	WSACleanup();
	return 0;
}

DWORD WINAPI ThreadFun(LPVOID threadFunPara)
{
	int socketClent = *((int*)threadFunPara);
	int str_len = 0, i;
	char msg[BUFF_LEN];
	//int WSAAPI recv(
	//	SOCKET s,
	//	char* buf,
	//	int    len,
	//	int    flags
	//	);
	/*
	����1���ͻ��˾�����ж���ͻ��˾����ʱ�򣬸��ݾ����������Ϣ��
	����2���ͻ�����Ϣ�Ĵ洢�ռ䣨�ַ����飩��һ��Ϊ1500�ֽڣ������С������䵥Ԫ��MTU�������йأ�MTU����·���е����������֡��һ�����ƣ�����̫��Ϊ����MTU��1500���ֽڡ�
	����3����Ҫ��ȡ���ֽڸ�����һ���ǲ���2���ֽ���-1����\0�ַ�����β��������
	����4�����ݵĶ�ȡ��ʽ��Ĭ����0���ɡ����������recv���ݲ���3��ȡ���ݻ�����ָ�����ȵ����ݺ�ָ�����ȴ������ݳ�����ȫ����ȡ����
	���ݻ������б���ȡ�����ݻ�������ѿռ��������������Ϣ������������Ļ�ʱ�䳤���ڴ����������ݻ��������ݽṹ�൱�ڶ��У���
	*/
	while ((str_len = recv(socketClent, msg, sizeof(msg), 0)) != -1)
	{
		send_msg(msg, str_len);
		printf("Ⱥ���ͳɹ�\n");
	}
	printf("�ͻ����˳�:%d\n", GetCurrentThreadId());
	//WaitForSingleObject(g_hEvent, INFINITE);
	//���رյĿͻ����׽���ɾ����
	for (i = 0; i < clent_num; i++)
	{
		if (socketClent == socketClents[i])
		{
			while (i++ < clent_num - 1)
				socketClents[i] = socketClents[i + 1];
			break;
		}
	}
	//�ͻ����׽���������һ
	clent_num--;
	//��ָ�����¼���������Ϊ�ź�״̬��
	/*
	BOOL SetEvent(
		[in] HANDLE hEvent
	);
	�������¼�����ľ����
	*/
	//SetEvent(g_hEvent);
	
	// �ر�ͬ�ͻ��˵�����
	closesocket(socketClent);
	return NULL;
}

void send_msg(char* msg, int len)
{
	int i;
	//DWORD WaitForSingleObject(
	//  HANDLE hHandle,
	//	DWORD dwMilliseconds
	//	);
	//����һ��������
	//����������ʱ������Ժ���Ϊ��λ��
	//���dwMillisecondsΪ�㣬��ú�������Զ����״̬���������ء����dwMilliseconds�� INFINITE�������ĳ�ʱ�����Զ�����ȥ��
	//WaitForSingleObject(g_hEvent, INFINITE);
	//��ÿ���ͻ����׽��ַ�����Ϣ
	for (i = 0; i < clent_num; i++) {
		send(socketClents[i], msg, len, 0);
	}
	//��ָ�����¼���������Ϊ�ź�״̬��
	/*
	BOOL SetEvent(
		[in] HANDLE hEvent
	);
	�������¼�����ľ����
	*/
	//SetEvent(g_hEvent);	
}

