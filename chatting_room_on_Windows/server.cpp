//��������socketͨ�ŵ�winsock2ͷ�ļ�
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
//����ռ�õĶ˿�
#define PORT 12345

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
}