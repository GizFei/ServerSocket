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
	// ����¿ͻ���
	void addNewClient(SOCKET id, sockaddr_in addr);
	// ��ӡ�ͻ�����Ϣ
	void printList();
	// ���ɿͻ����б��ַ�����
	char* clientListToChars();
	// �ı�ͻ���״̬
	void changeClientState(SOCKET id, int state);
	// �жϱ��Ϊid�Ŀͻ����Ƿ���ڣ����ڷ�������
	int clientExists(SOCKET id);
	// �ж�����λ�õĿͻ����Ƿ����ߣ��Ѵ��ڵĿͻ��ˣ�
	bool clientExistsAndOn(int idx);
	// ��ȡָ��λ���׽��ֱ��
	SOCKET getSocket(int idx);
	// �ͻ�����Ŀ
	int getClientsNum();
private:
	vector<Client> clientList;		// �ͻ����б�
};

#endif // !CLIENTLIST_H

