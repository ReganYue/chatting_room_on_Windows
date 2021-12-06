//导入用于socket通信的winsock2头文件
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")
//设置占用的端口
#define PORT 12345
//设置客户端的最大数量
#define CLIENT_MAX_NUM 128

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

	// 3、创建SOCKET
	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == socketServer)
	{
		int err = WSAGetLastError();
		printf("服务器创建SOCKET失败错误码为：%d\n", err);

		//清理网络库，不关闭句柄
		WSACleanup();
		return 0;
	}
	//printf("服务器创建SOCKET成功！\n");

	struct sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(PORT);//用htons宏将整型转为端口号的无符号整型

	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");


	//4、绑定地址与端口
	if (SOCKET_ERROR == bind(socketServer, (const struct sockaddr*)&si, sizeof(si)))
	{
		int err = WSAGetLastError();//取错误码
		printf("服务器bind失败错误码为：%d\n", err);
		closesocket(socketServer);//释放
		WSACleanup();//清理网络库

		return 0;
	}
	//printf("服务器端bind成功！\n");

		//5、开始监听
	if (SOCKET_ERROR == listen(socketServer, CLIENT_MAX_NUM))
	{
		int err = WSAGetLastError();//取错误码
		printf("服务器监听失败错误码为：%d\n", err);
		closesocket(socketServer);//释放
		WSACleanup();//清理网络库

		return 0;
	}

	//printf("服务器端监听成功！\n");
}