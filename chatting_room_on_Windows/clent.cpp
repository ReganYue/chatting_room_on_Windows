//��������socketͨ�ŵ�winsock2ͷ�ļ�
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
//����ռ�õĶ˿�
#define PORT 12345
//���ÿͻ��˵��������
#define CLIENT_MAX_NUM 128
#define BUFF_LEN 80

DWORD WINAPI send_msg(LPVOID lpParam);
DWORD WINAPI recv_msg(LPVOID lpParam);
void error_handling(const char* msg);

char name[CLIENT_MAX_NUM] = "[DEFAULT]";
char msg[BUFF_LEN];

int main()
{
	HANDLE hThread[2];
	DWORD dwThreadId;
	// ��ʼ��WS2_32.dll
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	WSAStartup(sockVersion, &wsaData);

	/*���õ�¼�û���*/

	printf("Input your Chat Name:");
	scanf("%s", name);
	getchar();	//���ջ��з�
	// �����׽���
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
		error_handling("Failed socket()");

	// ��дԶ�̵�ַ��Ϣ
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(PORT);
	// �����ļ����û��������ֱ��ʹ�ñ��ص�ַ127.0.0.1
	servAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	if (connect(sock, (sockaddr*)&servAddr, sizeof(servAddr)) == -1)
		error_handling("Failed connect()");
	printf("connect success\n");
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
	WaitForMultipleObjects(2, hThread, true, INFINITE);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	printf(" Thread Over,Enter anything to over.\n");
	getchar();
	// �ر��׽���
	closesocket(sock);
	// �ͷ�WS2_32��
	WSACleanup();
	return 0;
}

DWORD WINAPI send_msg(LPVOID lpParam)
{
	int sock = *((int*)lpParam);
	char name_msg[CLIENT_MAX_NUM + BUFF_LEN];
	while (1)
	{
		fgets(msg, BUFF_LEN, stdin);
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			closesocket(sock);
			exit(0);
		}
		sprintf(name_msg, "[%s]: %s", name, msg);
		int nRecv = send(sock, name_msg, strlen(name_msg), 0);
	}
	return NULL;
}

DWORD WINAPI recv_msg(LPVOID lpParam)
{
	int sock = *((int*)lpParam);
	char name_msg[CLIENT_MAX_NUM + BUFF_LEN];
	int str_len;
	while (1)
	{
		str_len = recv(sock, name_msg, CLIENT_MAX_NUM + BUFF_LEN - 1, 0);
		if (str_len == -1)
			return -1;
		name_msg[str_len] = 0;
		fputs(name_msg, stdout);
	}
	return NULL;
}

void error_handling(const char* msg)
{
	printf("%s\n", msg);
	WSACleanup();
	exit(1);
}