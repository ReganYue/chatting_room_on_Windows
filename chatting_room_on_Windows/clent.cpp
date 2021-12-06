//导入用于socket通信的winsock2头文件
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>//用于提供多线程服务
#pragma comment(lib, "Ws2_32.lib")
//设置占用的端口
#define PORT 12345
//设置客户端的最大数量
#define CLIENT_MAX_NUM 128
//buff的大小
#define BUFF_LEN 80
//发送消息的函数
DWORD WINAPI send_msg(LPVOID threadFunPara);
//接收消息的函数
DWORD WINAPI recv_msg(LPVOID threadFunPara);
//给名字设置默认值：[爱国青年]
char name[CLIENT_MAX_NUM] = "[爱国青年]";
//
char msg[BUFF_LEN];

int main()
{
	//线程句柄数组
	HANDLE hThread[2];
	//线程ID
	DWORD dwThreadId;
	
	WORD wdVersion = MAKEWORD(2, 2);
	WSADATA wdScoket;
	//1、打开网络库
	int nRes = WSAStartup(wdVersion, &wdScoket);
	//处理错误
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

	//输入昵称
	printf("请输入你的昵称:");
	scanf("%s", name);
	getchar();	//接收换行符

	// 3、创建SOCKET
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == sock)
	{
		int err = WSAGetLastError();
		printf("创建SOCKET失败错误码为：%d\n", err);

		//清理网络库，不关闭句柄
		WSACleanup();
		return 0;
	}

	struct sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(PORT);//用htons宏将整型转为端口号的无符号整型

	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	/*
	int WSAAPI connect(
	  SOCKET         s,
	  const sockaddr *name,
	  int            namelen
	);
	参数1 服务器的socket
	参数2 指向应建立连接的sockaddr结构的指针
	参数3 name参数所指向的sockaddr结构的长度（以字节为单位）
	
	*/
	int err = connect(sock, (sockaddr*)&si, sizeof(si));
	if (err == SOCKET_ERROR) {
		printf("connect错误，错误码：%d\n", WSAGetLastError()); //检验错误原因
		closesocket(sock);
		WSACleanup();	//不成功需要关闭网络库
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
		第一个参数 lpThreadAttributes 表示线程内核对象的安全属性，一般传入NULL表示使用默认设置。
		第二个参数 dwStackSize 表示线程栈空间大小。传入0表示使用默认大小（1MB）。
		第三个参数 lpStartAddress 表示新线程所执行的线程函数地址，多个线程可以使用同一个函数地址。
		第四个参数 threadFunParaeter 是传给线程函数的参数。
		第五个参数 dwCreationFlags 指定额外的标志来控制线程的创建，为0表示线程创建之后立即就可以进行调度，如果为CREATE_SUSPENDED则表示线程创建后暂停运行，这样它就无法调度，直到调用ResumeThread()。
		第六个参数 lpThreadId 将返回线程的ID号，传入NULL表示不需要返回该线程ID号。
	*/
	hThread[0] = CreateThread(
		NULL,		// 默认安全属性
		NULL,		// 默认堆栈大小
		send_msg,	// 线程入口地址（执行线程的函数）
		&sock,		// 传给函数的参数
		0,		// 指定线程立即运行
		&dwThreadId);	// 返回线程的ID号
	hThread[1] = CreateThread(
		NULL,		// 默认安全属性
		NULL,		// 默认堆栈大小
		recv_msg,	// 线程入口地址（执行线程的函数）
		&sock,		// 传给函数的参数
		0,		// 指定线程立即运行
		&dwThreadId);	// 返回线程的ID号

	//等待线程运行结束
	/*
	DWORD WaitForMultipleObjects(
		DWORD nCount,
		CONST HANDLE* lpHandles,
		BOOL fWaitAll,
		DWORD dwMilliseconds
	);
	第一个参数nCount 指定lpHandles指向的数组中的对象句柄数。对象句柄的最大数量为 MAXIMUM_WAIT_OBJECTS。
	第二个参数lpHandles 指向对象句柄数组的指针。
	第三个参数fWaitAll 指定等待类型。此参数必须设置为 FALSE
	第四个参数dwMilliseconds 以毫秒为单位指定超时间隔。只要经过时间间隔，即使不满足 bWaitAll参数指定的条件，该函数也会返回。
	如果dwMilliseconds为零，则该函数测试指定对象的状态并立即返回。如果dwMilliseconds是 INFINITE，则函数的超时间隔永远不会过去。
	*/
	WaitForMultipleObjects(2, hThread, true, INFINITE);
	//关闭线程句柄
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	// 关闭套节字
	closesocket(sock);
	// 关闭网络库
	WSACleanup();
	return 0;
}

DWORD WINAPI send_msg(LPVOID threadFunPara)
{
	int sock = *((int*)threadFunPara);
	//发送消息的长度为名字加上消息的长度
	char name_msg[CLIENT_MAX_NUM + BUFF_LEN];
	while (1)
	{
		//获取消息
		fgets(msg, BUFF_LEN, stdin);
		//如果消息是q或者Q,退出。
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			closesocket(sock);
			exit(0);
		}
		//如果不是就将消息本地输出
		sprintf(name_msg, "[%s]: %s", name, msg);
		//然后发送。
		int nRecv = send(sock, name_msg, strlen(name_msg), 0);
	}
	return NULL;
}

DWORD WINAPI recv_msg(LPVOID threadFunPara)
{
	int sock = *((int*)threadFunPara);
	//接收消息的长度为名字加上消息的长度
	char name_msg[CLIENT_MAX_NUM + BUFF_LEN];
	int str_len;
	while (1)
	{
		//接收消息。
		str_len = recv(sock, name_msg, CLIENT_MAX_NUM + BUFF_LEN - 1, 0);
		if (str_len == -1)
			return -1;
		//然后将字符数组置0。
		name_msg[str_len] = 0;
		//输出。
		fputs(name_msg, stdout);
	}
	return NULL;
}
