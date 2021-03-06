// ServerSocket.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <WinSock2.h>
#include <thread>
#include <conio.h>
#include <ctime>
#include <WS2tcpip.h>
#include "define.h"
#include "ClientList.h"

#define RECVLEN 160		// 一次接收数据的长度
#define RECVDATALEN 4096	// 最大接收数据长度

using namespace std;
#pragma comment (lib, "ws2_32.lib")    // 调用WS2静态库
#pragma warning (disable : 4996)

/* UTIL */
// 获得地址信息
sockaddr_in getAddrInfo(const char *host, int port);
// 将地址信息转换成字符串并打印
void printAddrInfo(sockaddr_in addrInfo);
// 获取地址信息中的端口号
int getPort(sockaddr_in addrInfo);
// 组成时间字符数组
char* formTime();
/* --UTIL-- */

// 初始化Socket环境
bool initSocket();
// 初始化服务端
SOCKET initServer();
// 客户端线程处理函数
void newClientThread(SOCKET s);
// 处理数据包
void handleData(SOCKET s, char* data);
// 发送消息到指定客户端
void sendMsgTo(SOCKET from, const char* message);

/* 全局变量 */
// 服务器IP地址
const char* SERVER_HOST = "10.180.169.141";
// 服务器端口号
const char* SERVER_PORT = "4221";
// 客户端列表
ClientList clientList;
// 主线程状态，是否退出，是则子线程退出
bool mainExit = false;
// 退出的子线程数目
int exitThreadNum = 0;

int main()
{
	int iResult;				// 函数执行反馈参数

	if (!initSocket()) {
		return 1;
	}
	// 初始化服务器
	SOCKET server = initServer();
	if (server == INVALID_SOCKET) {
		return 1;
	}

	// 监听
	iResult = listen(server, 3);
	if (iResult == SOCKET_ERROR) {
		cout << "Listen failed with error: " << WSAGetLastError() << endl;
		closesocket(server);
		WSACleanup();
		return 1;
	}
	cout << "等待连接……" << endl << endl;

	// 设置服务端为非阻塞的，持续执行accept，这样可以获得键盘输入
	u_long imode = 1;
	ioctlsocket(server, FIONBIO, &imode);

	while (true)
	{
		if (_kbhit()) {
			char key = _getch();	// 键盘输入字符
			if (key == 'e' || key == 'E' || key == 0x1B)		// 退出
			{
				// closesocket(clientList.getSocket(0));
				mainExit = true;
				break;
			}
			if (key == 'l' || key == 'L')
				clientList.printList();
		}

		sockaddr_in addr;		// 客户端地址
		int len = sizeof(addr);
		SOCKET client = accept(server, (SOCKADDR*)&addr, &len);
		if (client == INVALID_SOCKET) {
			// cout << "连接失败" << endl;
		}
		else
		{
			// 连接
			cout << "连接成功，客户端信息为：" << endl;
			printAddrInfo(addr);
			// 添加新客户端
			if (clientList.clientExists(client) != -1)
				--exitThreadNum;
			clientList.addNewClient(client, addr);
			clientList.printList();
			// printClientList(clientList.clientListToChars());
			// 新建子线程
			thread clientThread(newClientThread, client);
			// 该线程与主函数解耦，while可以继续循环
			clientThread.detach();
		}
	}

	while (true) {
		if (exitThreadNum == clientList.getClientsNum()) {
			// 所有子线程都退出了
			closesocket(server);
			cout << "退出服务器" << endl;
			break;
		}
	}

	return 1;
}

bool initSocket()
{
	WSADATA wsaData;
	int iResult;

	// 初始化WinSock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cout << "WSAStartup failed: " << iResult << endl;
		return false;
	}
	return true;
}

/* --------------------------------
- 初始化服务器并绑定地址
- :return：服务器套接字SOCKET
-------------------------------- */
SOCKET initServer()
{
	int iResult;

	// 服务器地址
	sockaddr_in addr = getAddrInfo(SERVER_HOST, atoi(SERVER_PORT));
	cout << "正在连接到服务器：" << endl;
	printAddrInfo(addr);
	// 创建服务端Socket
	SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// 绑定
	iResult = ::bind(server, (sockaddr*)&addr, sizeof(addr));
	if (iResult == 0) {
		return server;
	}
	else {
		cout << "Error at binding socket: " << WSAGetLastError() << endl;
		closesocket(server);
		WSACleanup();
		return INVALID_SOCKET;
	}
}

/* --------------------------------
- 客户端线程处理函数
- s：客户端SOCKET编号
- :return：地址信息
-------------------------------- */
void newClientThread(SOCKET client)
{
	// 发送消息
	char *msg = new char[64]();
	sprintf(msg, "Hello, your id is %d.", client);
	send(client, msg, strlen(msg), 0);

	char recvBuf[RECVLEN + 1] = { 0 };	// 注意长度加1！
	int totalLen = 0;
	char *recvData = new char[RECVDATALEN + 1]();
	int bufLen;
	while (true)
	{
		bufLen = recv(client, recvBuf, RECVLEN, 0); // 阻塞状态
		if (bufLen > 0)
		{
			cout << "--------------------------------" << endl;
			cout << "> 客户端编号：" << client << endl;
			// cout << "一次接收信息的长度：" << bufLen << endl;
			for (int i = 0; i < bufLen; ++i) {
				recvData[totalLen++] = recvBuf[i];
				if (recvBuf[i] == DELIMITER) {
					// 接收到一个完整数据包了
					int len = CharsWithDeliLen(recvData);
					cout << "# 数据包长度为：" << len << endl;
					// 处理数据包
					char *data = new char[len + 1]();
					memcpy(data, recvData, len + 1);
					handleData(client, data);
					// 清空
					totalLen = 0;
					memset(recvData, 0, len + 1);
				}
			}
		}
		if (bufLen == 0) {
			clientList.changeClientState(client, STATE_OFF);
			cout << "退出客户端<" << client << ">" << "所在的子线程" << endl << endl;
			closesocket(client);	// 关闭Socket
			exitThreadNum += 1;
			return;
		}
		else
		{
			if (mainExit) {
				cout << "退出客户端<" << client << ">" << "所在的子线程" << endl;
				closesocket(client);	// 关闭Socket
				exitThreadNum += 1;
				return;
			}		
		}
	}
}

/* --------------------------------
- 处理数据包
- s：套接字编号
- data：数据包字符数组
- :return：VOID
-------------------------------- */
void handleData(SOCKET s, char* data)
{
	Packet p = charsToPacket(data);
	if(p.req_type != REQ_MSG)
		cout << "> 数据包信息为：" << p.res_type << " " << p.req_type << " " << p.length << " " << p.data << endl;
	else
		cout << "> 数据包信息为：" << p.res_type << " " << p.req_type << " " << p.length << " " << getMsgFromMessage(p.data) << endl;

	if (p.res_type == RES_REQUEST)
	{
		// 客户端发过来的请求
		char *sendData;				// 返回的数据
		p.res_type = RES_REPLY;		// 响应类型
		switch (p.req_type)
		{
		case REQ_TIME:
			// 时间
			p.data = formTime();
			p.length = calcPacketLen(p);
			sendData = packetToChars(p);
			// 发送带时间信息的数据包
			cout << "发送时间信息：" << p.data << endl;
			send(s, sendData, CharsWithDeliLen(sendData) + 1, 0);
			break;
		case REQ_NAME:
			// 请求获取服务器的名字
			p.data = "DESTOP-6DN6G65";
			p.length = calcPacketLen(p);
			sendData = packetToChars(p);
			// 发送带名字信息的数据包
			cout << "发送服务器的名字：" << p.data << endl;
			send(s, sendData, CharsWithDeliLen(sendData) + 1, 0);
			break;
		case REQ_MSG:
			// 发送消息
			sendMsgTo(s, p.data);
			break;
		case REQ_LIST:
			// 客户端列表
			p.data = clientList.clientListToChars();
			p.length = calcPacketLen(p);
			sendData = packetToChars(p);
			// 发送客户端列表字符数组
			cout << "发送客户端列表字符数组……" << endl;
			send(s, sendData, CharsWithDeliLen(sendData) + 1, 0);
			cout << "发送完毕" << endl;
			break;
		default:
			break;
		}
	}
	cout << "--------------------------------" << endl;
}

/* --------------------------------
- 发送消息到指定客户端
- from：源客户端编号
- message：消息数据（未解析的）
- :return：VOID
-------------------------------- */
void sendMsgTo(SOCKET from, const char * message)
{
	int iResult;
	// 解析数据
	int toClient = getIdFromMessage(message);
	char* msg = getMsgFromMessage(message);

	int idx = clientList.clientExists(toClient);
	char *sendData;
	if (idx != -1) {
		if (clientList.clientExistsAndOn(idx)) {
			// 存在并在线，转发
			char *fromInfo = new char[512];
			sprintf(fromInfo, "From Client<%d>: %s", from, msg);
			Packet p;
			p.req_type = REQ_MSG;
			p.res_type = RES_INSTRUCT;
			p.data = fromInfo;
			p.length = calcPacketLen(p);
			sendData = packetToChars(p);

			cout << "# 客户端" << from << " -> " << "客户端" << toClient << endl;
			cout << "> 消息：" << msg << "|END|" << endl;
			iResult = send(toClient, sendData, CharsWithDeliLen(sendData), 0);
			if (iResult == SOCKET_ERROR) {
				int e = WSAGetLastError();
				cout << "发送失败：" << e << endl;
				if (e == 10054) {
					// 判断该客户端的连接已经断开
					clientList.changeClientState(toClient, STATE_OFF);
				}
			}
			else {
				cout << "- 发送成功" << endl;
			}
		}
		else {
			// 不在线，返回错误信息
			Packet p;
			p.req_type = REQ_MSG;
			p.res_type = RES_REPLY;
			p.data = formErrorMessage(ERROR_NO_CONNECTED, "该客户端不在线");
			p.length = calcPacketLen(p);
			sendData = packetToChars(p);
			cout << "发送错误信息：" << p.data << endl;
			iResult = send(from, sendData, CharsWithDeliLen(sendData), 0);
			if (iResult == SOCKET_ERROR) {
				cout << "发送失败：" << WSAGetLastError() << endl;
			}
			else {
				cout << "- 发送错误信息成功" << endl;
			}
		}
	}
	else {
		// 不存在，返回错误信息
		Packet p;
		p.req_type = REQ_MSG;
		p.res_type = RES_REPLY;
		p.data = formErrorMessage(ERROR_NO_EXISTS, "客户端不存在");
		p.length = calcPacketLen(p);
		sendData = packetToChars(p);
		cout << "发送错误信息：" << p.data << endl;
		iResult = send(from, sendData, CharsWithDeliLen(sendData), 0);
		if (iResult == SOCKET_ERROR) {
			cout << "发送失败：" << WSAGetLastError() << endl;
		}
		else {
			cout << "- 发送错误信息成功" << endl;
		}
	}
}

/* --------------------------------
- 获得地址信息
- host：IP地址（如"127.0.0.1")
- port：端口号
- :return：地址信息
-------------------------------- */
sockaddr_in getAddrInfo(const char *host, int port)
{
	sockaddr_in addr;
	addr.sin_addr.S_un.S_addr = inet_addr(host);
	addr.sin_family = AF_INET;		// IPv4
	// htons函数将u_short从主机转换为TCP / IP网络字节顺序（这是big-endian）
	addr.sin_port = htons(port);

	return addr;
}

/* --------------------------------
- 将地址信息转换成字符串并打印
- addrInfo：地址信息
- :return：VOID
-------------------------------- */
void printAddrInfo(sockaddr_in addrInfo)
{
	char ipPtr[INET_ADDRSTRLEN] = "";
	int port;
	// InetPton函数将标准文本表示形式的IPv4或IPv6 Internet网络地址转换为其数字二进制形式。
	// 此函数的ANSI版本为inet_pton。
	inet_ntop(AF_INET, &addrInfo.sin_addr, ipPtr, INET_ADDRSTRLEN);
	// ntohs函数将u_short从TCP / IP网络字节顺序转换为主机字节顺序（在Intel处理器上为little-endian）
	// network byte -> host byte
	port = ntohs(addrInfo.sin_port);
	// 打印
	cout << "地址为：" << ipPtr << ":" << port << endl;
}

/* --------------------------------
 -获取地址信息中的端口号
- addrInfo：地址信息
- :return：端口号
-------------------------------- */
int getPort(sockaddr_in addrInfo)
{
	return ntohs(addrInfo.sin_port);
}

/* --------------------------------
- 组成时间字符数组
- :return：时间信息字符数组
-------------------------------- */
char* formTime()
{
	char *timeStr = new char[64]();
	time_t now = time(0);	// 当前时间

	struct tm* t = localtime(&now);
	strftime(timeStr, 64, "%Y-%m-%d %H:%M:%S", t);

	return timeStr;
}
