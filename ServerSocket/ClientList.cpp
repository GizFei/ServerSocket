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
- ����¿ͻ���
- id���׽��ֱ�ʶ��
- addr����ַ��Ϣ
- :return����ַ��Ϣ
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
		// IP��ַ
		newClient.ipAddr = new char[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &addr.sin_addr, newClient.ipAddr, INET_ADDRSTRLEN);
		// �˿ں�
		newClient.port = htons(addr.sin_port);
		// ������
		newClient.state = STATE_TYPE::STATE_ON;
		this->clientList.push_back(newClient);
	}
}

/* --------------------------------
- ��ӡ�ͻ����б�
- :return��VOID
-------------------------------- */
void ClientList::printList()
{
	cout << "--------------------------------" << endl << "> �ͻ����б�" << endl;
	for (int i = 0; i < this->clientList.size(); i++)
	{
		cout << "| " << clientList[i].id << " "
			<< clientList[i].ipAddr << ":" << clientList[i].port << " "
			<< (clientList[i].state ? "OFF" : "ON") << endl;
	}
	cout << "--------------------------------" << endl << endl;
}

/* --------------------------------
- ���ɿͻ����б��ַ�����
- :return���ͻ����б��ַ����飨������β��ǣ�
-------------------------------- */
char * ClientList::clientListToChars()
{
	char *c = new char[1024];
	int idx = 0;
	for (int i = 0; i < this->clientList.size(); ++i)
	{
		char *tempC = clientToChars(clientList[i]);
		int len = CharsWithDeliLen(tempC);		// ȥ����β���
		memcpy(c + idx, tempC, len);
		idx += len;
		c[idx++] = CLIENT_DELIMITER;			// �ͻ��������
		free(tempC);
	}
	c[idx] = DELIMITER;
	/*
	cout << endl << "�ͻ����б��ַ����飺" << CharsWithDeliLen(c) << endl;
	for (int i = 0; i < CharsWithDeliLen(c); ++i) {
		cout << c[i] << "-";
	}
	cout << endl;
	//*/
	return c;
}

/* --------------------------------
- �ı�ͻ���״̬
- id���ͻ��˱��
- state��״̬
- :return��VOID
-------------------------------- */
void ClientList::changeClientState(SOCKET id, int state)
{
	int idx = this->clientExists(id);
	if (idx != -1) {
		this->clientList[idx].state = state;
	}
}

/* --------------------------------
- �жϱ��Ϊid�Ŀͻ����Ƿ���ڣ����ڷ�������
- id���ͻ��˱��
- :return�����б��е������������ڷ���-1
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
- �ж�����λ�õĿͻ����Ƿ����ߣ��Ѵ��ڵĿͻ��ˣ�
- idx������λ��
- :return���Ƿ�����
-------------------------------- */
bool ClientList::clientExistsAndOn(int idx)
{
	if(idx < 0 || idx >= this->clientList.size())
		return false;
	return (clientList[idx].state == STATE_ON);
}

/* --------------------------------
- ��ȡָ��λ���׽��ֱ��
- idx������λ��
- :return���׽��ֱ��
-------------------------------- */
SOCKET ClientList::getSocket(int idx)
{
	return this->clientList[idx].id;
}

int ClientList::getClientsNum()
{
	return this->clientList.size();
}
