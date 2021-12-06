//导入用于socket通信的winsock2头文件
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
//设置占用的端口
#define PORT 12345

int main() {
	//1、打开网络库
	WORD wdVersion = MAKEWORD(2, 2);
	WSADATA wdScoket;

	int nRes = WSAStartup(wdVersion, &wdScoket);

	if (0 != nRes)
	{
		switch (nRes)
		{
		case WSASYSNOTREADY:
			printf("解决方案：重启。。。");
			break;
		case WSAVERNOTSUPPORTED:
			printf("解决方案：更新网络库");
			break;
		case WSAEINPROGRESS:
			printf("解决方案：重启。。。");
			break;
		case WSAEPROCLIM:
			printf("解决方案：网络连接达到上限或阻塞，关闭不必要软件");
			break;
		case WSAEFAULT:
			printf("解决方案：程序有误");
			break;
		}
		return 0;

	}
	//printf("打开网络库成功！\n");

	//2、校验版本号
	int a = *((char*)&wdVersion);
	int b = *((char*)&wdVersion + 1);

	if (2 != HIBYTE(wdScoket.wVersion) || 2 != LOBYTE(wdScoket.wVersion))
	{
		printf("版本有问题！\n");
		WSACleanup();
		return 0;
	}
	//printf("版本校验成功！\n");
}