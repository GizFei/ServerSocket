#include "pch.h"
#include <iostream>
#include "ClientList.h"

ClientList::ClientList()
{
}


ClientList::~ClientList()
{
}

/* --------------------------------
- 添加新客户端
- id：套接字标识符
- addr：地址信息
- :return：地址信息
-------------------------------- */
void ClientList::addNewClient(SOCKET id, sockaddr_in addr)
{
	int idx = this->clientExists(id);
	if (idx != -1) 
	{
		this->clientList[idx].ipAddr = new char[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &addr.sin_addr, this->clientList[idx].ipAddr, INET_ADDRSTRLEN);
		this->clientList[idx].port = htons(addr.sin_port);
		this->clientList[idx].state = STATE_ON;
	}
	else
	{
		Client newClient;
		newClient.id = id;
		// IP地址
		newClient.ipAddr = new char[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &addr.sin_addr, newClient.ipAddr, INET_ADDRSTRLEN);
		// 端口号
		newClient.port = htons(addr.sin_port);
		// 已连接
		newClient.state = STATE_TYPE::STATE_ON;
		this->clientList.push_back(newClient);
	}
}

/* --------------------------------
- 打印客户端列表
- :return：VOID
-------------------------------- */
void ClientList::printList()
{
	cout << "--------------------------------" << endl << "> 客户端列表：" << endl;
	for (int i = 0; i < this->clientList.size(); i++)
	{
		cout << "| " << clientList[i].id << " "
			<< clientList[i].ipAddr << ":" << clientList[i].port << " "
			<< (clientList[i].state ? "OFF" : "ON") << endl;
	}
	cout << "--------------------------------" << endl << endl;
}

/* --------------------------------
- 生成客户端列表字符数组
- :return：客户端列表字符数组（包括结尾标记）
-------------------------------- */
char * ClientList::clientListToChars()
{
	char *c = new char[1024];
	int idx = 0;
	for (int i = 0; i < this->clientList.size(); ++i)
	{
		char *tempC = clientToChars(clientList[i]);
		int len = CharsWithDeliLen(tempC);		// 去掉结尾标记
		memcpy(c + idx, tempC, len);
		idx += len;
		c[idx++] = CLIENT_DELIMITER;			// 客户端相隔符
		free(tempC);
	}
	c[idx] = DELIMITER;
	/*
	cout << endl << "客户端列表字符数组：" << CharsWithDeliLen(c) << endl;
	for (int i = 0; i < CharsWithDeliLen(c); ++i) {
		cout << c[i] << "-";
	}
	cout << endl;
	//*/
	return c;
}

/* --------------------------------
- 改变客户端状态
- id：客户端编号
- state：状态
- :return：VOID
-------------------------------- */
void ClientList::changeClientState(SOCKET id, int state)
{
	int idx = this->clientExists(id);
	if (idx != -1) {
		this->clientList[idx].state = state;
	}
}

/* --------------------------------
- 判断编号为id的客户端是否存在，存在返回索引
- id：客户端编号
- :return：在列表中的索引，不存在返回-1
-------------------------------- */
int ClientList::clientExists(SOCKET id)
{
	for (int i = 0; i < this->clientList.size(); ++i)
	{
		if (clientList[i].id == id) {
			return i;
		}
	}
	return -1;
}

/* --------------------------------
- 判断索引位置的客户端是否在线（已存在的客户端）
- idx：索引位置
- :return：是否在线
-------------------------------- */
bool ClientList::clientExistsAndOn(int idx)
{
	if(idx < 0 || idx >= this->clientList.size())
		return false;
	return (clientList[idx].state == STATE_ON);
}

/* --------------------------------
- 获取指定位置套接字编号
- idx：索引位置
- :return：套接字编号
-------------------------------- */
SOCKET ClientList::getSocket(int idx)
{
	return this->clientList[idx].id;
}

int ClientList::getClientsNum()
{
	return this->clientList.size();
}
