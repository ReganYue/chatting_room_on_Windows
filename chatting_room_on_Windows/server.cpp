#include <WinSock2.h>//导入用于socket通信的winsock2头文件
#include <stdio.h>
#include <stdlib.h>
#include <windows.h> //用于提供多线程服务
#pragma comment(lib, "Ws2_32.lib")
//设置占用的端口
#define PORT 12345
//设置客户端的最大数量
#define CLIENT_MAX_NUM 128
//buff的大小
#define BUFF_LEN 80

SOCKET socketServer; //服务器套接字
HANDLE mul_thread[CLIENT_MAX_NUM];	//管理线程
int clent_num = 0;			//客户端套接字计数
int socketClents[CLIENT_MAX_NUM];	//客户端套接字
//HANDLE g_hEvent; //对象句柄
DWORD WINAPI ThreadFun(LPVOID threadFunPara);	//线程执行函数
void send_msg(char* msg, int len);			//消息发送函数

BOOL WINAPI cls(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		//释放所有句柄
		for (u_int i = 0; i < CLIENT_MAX_NUM; i++)
		{
			closesocket(socketClents[i]);	
		}
		closesocket(socketServer);
		WSACleanup();//清理网络库
		
	}

	return TRUE;
}

int main() {
	SetConsoleCtrlHandler(cls, TRUE);
	//1、打开网络库
	WORD wdVersion = MAKEWORD(2, 2);
	WSADATA wdScoket;

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

	// 3、创建SOCKET
	socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

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

	sockaddr_in remoteAddr;
	int remoteAddrLen = sizeof(remoteAddr); //
	DWORD dwThreadId;	//线程ID
	SOCKET socketClent;//单个客户端套接字

	while (TRUE)
	{
		printf("等待新用户连接...\n");
		//SOCKET WSAAPI accept(
		//SOCKET   s,
		//	sockaddr* addr,
		//	int* addrlen
		//	);
		//
		//参数1：服务器SOCKET句柄，该句柄要先处于监听状态，客户端的连接都由这个服务器SOCKET句柄管理。
		//参数2：sockaddr类型的结构体的传址调用，获取客户端的地址、端口信息。
		//参数3：int类型的传址调用，传入参数2的大小。
		socketClent = accept(socketServer, (SOCKADDR*)&remoteAddr, &remoteAddrLen);
		if (INVALID_SOCKET == socketClent)
		{
			int err = WSAGetLastError();//取错误码
			printf("获取客户端句柄失败错误码为：%d\n", err);
			closesocket(socketServer);//释放
			WSACleanup();//清理网络库

			return 0;
		}
		//DWORD WaitForSingleObject(
		//  HANDLE hHandle,
		//	DWORD dwMilliseconds
		//	);
		//参数一：对象句柄
		//参数二：超时间隔，以毫秒为单位。
		//如果dwMilliseconds为零，则该函数会测试对象的状态并立即返回。如果dwMilliseconds是 INFINITE，则函数的超时间隔永远不会过去。
		//int err = WaitForSingleObject(g_hEvent, INFINITE);
		/*if (err == WAIT_FAILED) {
			printf("可能出现以下情况一个以上：系统内存不足。两个线程正在等待同一个中断事件。当前线程正在被终止。hHandle所代表的底层对象已被删除。句柄无效。");
		}
		else if (err == WAIT_OBJECT_0) {
			printf("指定对象的状态已发出信号。");
		}
		else if (err == WAIT_TIMEOUT) {
			printf("超时间隔已过，并且对象的状态为无信号。");
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
			第一个参数 lpThreadAttributes 表示线程内核对象的安全属性，一般传入NULL表示使用默认设置。
			第二个参数 dwStackSize 表示线程栈空间大小。传入0表示使用默认大小（1MB）。
			第三个参数 lpStartAddress 表示新线程所执行的线程函数地址，多个线程可以使用同一个函数地址。
			第四个参数 threadFunParaeter 是传给线程函数的参数。
			第五个参数 dwCreationFlags 指定额外的标志来控制线程的创建，为0表示线程创建之后立即就可以进行调度，如果为CREATE_SUSPENDED则表示线程创建后暂停运行，这样它就无法调度，直到调用ResumeThread()。
			第六个参数 lpThreadId 将返回线程的ID号，传入NULL表示不需要返回该线程ID号。
		*/
		mul_thread[clent_num] = CreateThread(
			NULL,		// 默认安全属性
			0,		// 线程栈空间大小,默认大小（1MB）
			ThreadFun,	// 线程入口地址（执行线程的函数）,多个线程可以使用同一个函数地址
			(void*)&socketClent,		// 传给函数的参数
			0,		// 线程创建之后立即就可以进行调度
			&dwThreadId);	// 返回线程的ID号
		socketClents[clent_num++] = socketClent;
		//SetEvent(g_hEvent);				/*设置受信*/

		printf(" %s连接成功,该线程ID为%d。\r\n", inet_ntoa(remoteAddr.sin_addr), dwThreadId);
	}
	
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
	WaitForMultipleObjects(clent_num, mul_thread, FALSE, INFINITE);
	//关闭每一个线程句柄
	for (int i = 0; i < clent_num; i++)
	{
		CloseHandle(mul_thread[i]);
	}
	// 关闭监听套节字
	closesocket(socketServer);

	for (u_int i = 0; i < CLIENT_MAX_NUM; i++)
	{
		closesocket(socketClents[i]);
	}
	// 释放WS2_32库
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
	参数1：客户端句柄，有多个客户端句柄的时候，根据句柄来接收消息。
	参数2：客户端消息的存储空间（字符数组），一般为1500字节，这个大小和最大传输单元（MTU）限制有关，MTU是链路层中的网络对数据帧的一个限制，以以太网为例，MTU是1500个字节。
	参数3：想要读取的字节个数，一般是参数2的字节数-1，把\0字符串结尾留出来。
	参数4：数据的读取方式。默认是0即可。正常情况下recv根据参数3读取数据缓冲区指定长度的数据后（指定长度大于数据长度则全部读取），
	数据缓冲区中被读取的数据会清除，把空间留出来给别的消息进来（不清理的话时间长了内存会溢出，数据缓冲区数据结构相当于队列）。
	*/
	while ((str_len = recv(socketClent, msg, sizeof(msg), 0)) != -1)
	{
		send_msg(msg, str_len);
		printf("群发送成功\n");
	}
	printf("客户端退出:%d\n", GetCurrentThreadId());
	//WaitForSingleObject(g_hEvent, INFINITE);
	//将关闭的客户端套接字删除。
	for (i = 0; i < clent_num; i++)
	{
		if (socketClent == socketClents[i])
		{
			while (i++ < clent_num - 1)
				socketClents[i] = socketClents[i + 1];
			break;
		}
	}
	//客户端套接字数量减一
	clent_num--;
	//将指定的事件对象设置为信号状态。
	/*
	BOOL SetEvent(
		[in] HANDLE hEvent
	);
	参数是事件对象的句柄。
	*/
	//SetEvent(g_hEvent);
	
	// 关闭同客户端的连接
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
	//参数一：对象句柄
	//参数二：超时间隔，以毫秒为单位。
	//如果dwMilliseconds为零，则该函数会测试对象的状态并立即返回。如果dwMilliseconds是 INFINITE，则函数的超时间隔永远不会过去。
	//WaitForSingleObject(g_hEvent, INFINITE);
	//给每个客户端套接字发送消息
	for (i = 0; i < clent_num; i++) {
		send(socketClents[i], msg, len, 0);
	}
	//将指定的事件对象设置为信号状态。
	/*
	BOOL SetEvent(
		[in] HANDLE hEvent
	);
	参数是事件对象的句柄。
	*/
	//SetEvent(g_hEvent);	
}

