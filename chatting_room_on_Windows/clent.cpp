//��������socketͨ�ŵ�winsock2ͷ�ļ�
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>//�����ṩ���̷߳���
#pragma comment(lib, "Ws2_32.lib")
//����ռ�õĶ˿�
#define PORT 12345
//���ÿͻ��˵��������
#define CLIENT_MAX_NUM 128
//buff�Ĵ�С
#define BUFF_LEN 80
//������Ϣ�ĺ���
DWORD WINAPI send_msg(LPVOID threadFunPara);
//������Ϣ�ĺ���
DWORD WINAPI recv_msg(LPVOID threadFunPara);
//����������Ĭ��ֵ��[��������]
char name[CLIENT_MAX_NUM] = "[��������]";
//
char msg[BUFF_LEN];

int main()
{
	//�߳̾������
	HANDLE hThread[2];
	//�߳�ID
	DWORD dwThreadId;
	
	WORD wdVersion = MAKEWORD(2, 2);
	WSADATA wdScoket;
	//1���������
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

	//�����ǳ�
	printf("����������ǳ�:");
	scanf("%s", name);
	getchar();	//���ջ��з�

	// 3������SOCKET
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == sock)
	{
		int err = WSAGetLastError();
		printf("����SOCKETʧ�ܴ�����Ϊ��%d\n", err);

		//��������⣬���رվ��
		WSACleanup();
		return 0;
	}

	struct sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(PORT);//��htons�꽫����תΪ�˿ںŵ��޷�������

	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	/*
	int WSAAPI connect(
	  SOCKET         s,
	  const sockaddr *name,
	  int            namelen
	);
	����1 ��������socket
	����2 ָ��Ӧ�������ӵ�sockaddr�ṹ��ָ��
	����3 name������ָ���sockaddr�ṹ�ĳ��ȣ����ֽ�Ϊ��λ��
	
	*/
	int err = connect(sock, (sockaddr*)&si, sizeof(si));
	if (err == SOCKET_ERROR) {
		printf("connect���󣬴����룺%d\n", WSAGetLastError()); //�������ԭ��
		closesocket(sock);
		WSACleanup();	//���ɹ���Ҫ�ر������
		return 0;
	}
	else {
		printf("connect success\n");
	}
	
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
	hThread[0] = CreateThread(
		NULL,		// Ĭ�ϰ�ȫ����
		NULL,		// Ĭ�϶�ջ��С
		send_msg,	// �߳���ڵ�ַ��ִ���̵߳ĺ�����
		&sock,		// ���������Ĳ���
		0,		// ָ���߳���������
		&dwThreadId);	// �����̵߳�ID��
	hThread[1] = CreateThread(
		NULL,		// Ĭ�ϰ�ȫ����
		NULL,		// Ĭ�϶�ջ��С
		recv_msg,	// �߳���ڵ�ַ��ִ���̵߳ĺ�����
		&sock,		// ���������Ĳ���
		0,		// ָ���߳���������
		&dwThreadId);	// �����̵߳�ID��

	//�ȴ��߳����н���
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
	WaitForMultipleObjects(2, hThread, true, INFINITE);
	//�ر��߳̾��
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	// �ر��׽���
	closesocket(sock);
	// �ر������
	WSACleanup();
	return 0;
}

DWORD WINAPI send_msg(LPVOID threadFunPara)
{
	int sock = *((int*)threadFunPara);
	//������Ϣ�ĳ���Ϊ���ּ�����Ϣ�ĳ���
	char name_msg[CLIENT_MAX_NUM + BUFF_LEN];
	while (1)
	{
		//��ȡ��Ϣ
		fgets(msg, BUFF_LEN, stdin);
		//�����Ϣ��q����Q,�˳���
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			closesocket(sock);
			exit(0);
		}
		//������Ǿͽ���Ϣ�������
		sprintf(name_msg, "[%s]: %s", name, msg);
		//Ȼ���͡�
		int nRecv = send(sock, name_msg, strlen(name_msg), 0);
	}
	return NULL;
}

DWORD WINAPI recv_msg(LPVOID threadFunPara)
{
	int sock = *((int*)threadFunPara);
	//������Ϣ�ĳ���Ϊ���ּ�����Ϣ�ĳ���
	char name_msg[CLIENT_MAX_NUM + BUFF_LEN];
	int str_len;
	while (1)
	{
		//������Ϣ��
		str_len = recv(sock, name_msg, CLIENT_MAX_NUM + BUFF_LEN - 1, 0);
		if (str_len == -1)
			return -1;
		//Ȼ���ַ�������0��
		name_msg[str_len] = 0;
		//�����
		fputs(name_msg, stdout);
	}
	return NULL;
}
