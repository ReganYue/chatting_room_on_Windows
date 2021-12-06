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

int main() {
	//1���������
	WORD wdVersion = MAKEWORD(2, 2);
	WSADATA wdScoket;

	int nRes = WSAStartup(wdVersion, &wdScoket);

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
	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

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
}