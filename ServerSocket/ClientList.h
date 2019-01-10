#ifndef CLIENTLIST_H
#define CLIENTLIST_H

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include "define.h"

using namespace std;

class ClientList
{
public:
	ClientList();
	~ClientList();
	// 添加新客户端
	void addNewClient(SOCKET id, sockaddr_in addr);
	// 打印客户端信息
	void printList();
	// 生成客户端列表字符数组
	char* clientListToChars();
	// 改变客户端状态
	void changeClientState(SOCKET id, int state);
	// 判断编号为id的客户端是否存在，存在返回索引
	int clientExists(SOCKET id);
	// 判断索引位置的客户端是否在线（已存在的客户端）
	bool clientExistsAndOn(int idx);
	// 获取指定位置套接字编号
	SOCKET getSocket(int idx);
	// 客户端数目
	int getClientsNum();
private:
	vector<Client> clientList;		// 客户端列表
};

#endif // !CLIENTLIST_H

