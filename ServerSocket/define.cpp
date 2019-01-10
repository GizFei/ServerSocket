#include "pch.h"
#include <iostream>
#include "define.h"

using namespace std;

/* --------------------------------
- 32λ��Intת��char[4]
- n��32-bit����
- :return��char[4]����
-------------------------------- */
char* intToBytes(int n) {
	// ʹ��new����ռ䣬���Է�����ȷ���
	char *bytes = new char[4];
	bytes[0] = (n >> 24) & 0xFF;
	bytes[1] = (n >> 16) & 0xFF;
	bytes[2] = (n >> 8) & 0xFF;
	bytes[3] = n & 0xFF;

	return bytes;
}

/* --------------------------------
- char[4]ת��Ϊ32λ��Int
- bytes��char[4]����
- :return��32-bit����
-------------------------------- */
int bytesToInt(char *bytes) {
	unsigned int nOffset = *reinterpret_cast<unsigned*>(bytes);
	nOffset = (nOffset >> 24) |
		((nOffset << 8) & 0x00FF0000) |
		((nOffset >> 8) & 0x0000FF00) |
		(nOffset << 24);

	return nOffset;
}

/* --------------------------------
- ������߽�����ַ����鳤��
- s�����ݰ��ַ�����
- :return�����ȣ���������β��ǣ�
-------------------------------- */
int CharsWithDeliLen(char *s)
{
	int i = 0, len = 0;
	while (s[i] != DELIMITER) {
		++len;
		++i;
	}
	return len;
}

/* --------------------------------
- ����Packet��Ҫ���ַ����鳤��
- p��Packet�ṹ������
- :return������
-------------------------------- */
int calcPacketLen(Packet & p)
{
	if (p.req_type == REQ_MSG)
		return 6 + calcMessageLen((char*)p.data);
	else if (p.req_type == REQ_LIST)
		return 6 + CharsWithDeliLen((char*)p.data);
	else
		return 6 + strlen(p.data);
}

/* --------------------------------
- ���ݰ��ṹ��ת��Ϊ�ַ�����
- p�����ݰ�
- :return���ַ�����
- NOTE����β���Ϊchar(255)��������ĳ�����ʹ��<CharsWithDeliLen>���㳤��
-------------------------------- */
char* packetToChars(Packet& p)
{
	char *c = new char[p.length + 1];

	itoa(p.req_type, &c[0], 10);
	itoa(p.res_type, &c[1], 10);
	// ���� 
	char *l = intToBytes(p.length);
	for (int i = 0; i < 4; ++i) {
		c[i + 2] = l[i];
	}
	// ���� 
	for (int i = 6; i < p.length; ++i) {
		c[i] = p.data[i - 6];
	}
	// ��β��� 
	c[p.length] = DELIMITER;
	return c;
}

/* --------------------------------
- �ַ�����ת��Ϊ���ݰ��ṹ��
- s���ַ����飨������β��ǣ�
- :return�����ݰ��ṹ��
-------------------------------- */
Packet charsToPacket(char* s)
{
	Packet p;
	char req = s[0];
	char res = s[1];
	p.req_type = atoi(&req);
	p.res_type = atoi(&res);
	// ����
	char *bytes = new char[4]();
	for (int i = 0; i < 4; ++i) {
		bytes[i] = s[i + 2];
	}
	p.length = bytesToInt(bytes);

	int len = p.length - 6;
	char *d = new char[len + 1]();	// ����+1�����Ҽ�()����ʼ�������ַ�Ϊ'\0'
	for (int i = 0; i < len; i++) {
		d[i] = s[i + 6];
	}
	p.data = d;

	return p;
}

/* --------------------------------
- ����Client���ַ����鳤�ȣ�IPAddr���Ȳ�ͬ��
- client��Client�ṹ������
- :return������
-------------------------------- */
int calcClientLen(Client &client)
{
	return (9 + strlen(client.ipAddr));
}

/* --------------------------------
- Client�ṹ��ת��Ϊ�ַ�����
- client��Client�ṹ������
- :return���ַ����飨������β��ǣ�
- NOTE����β���Ϊchar(255)��������ĳ�����ʹ��<packetCharsLen>���㳤��
-------------------------------- */
char* clientToChars(Client &client) {
	int len = calcClientLen(client);
	char *c = new char[len + 1];
	// �ͻ��˱��
	char *idBytes = intToBytes(client.id);
	for (int i = 0; i < 4; i++) {
		c[i] = idBytes[i];
	}
	char *portBytes = intToBytes(client.port);
	for (int i = 0; i < 4; i++) {
		c[i + 4] = portBytes[i];
	}
	itoa(client.state, c + 8, 10);
	strcpy(c + 9, client.ipAddr);

	c[len] = DELIMITER;
	return c;
}

/* --------------------------------
- �ַ�����ת��ΪClient�ṹ��
- s���ַ����飨������β��ǣ�
- :return��Client�ṹ��
-------------------------------- */
Client charsToClient(char *s)
{
	Client client;
	// ���
	char *idBytes = new char[4];
	for (int i = 0; i < 4; ++i) {
		idBytes[i] = s[i];
	}
	client.id = bytesToInt(idBytes);
	// �˿ں�
	char *portBytes = new char[4];
	for (int i = 0; i < 4; ++i) {
		portBytes[i] = s[i + 4];
	}
	client.port = bytesToInt(portBytes);
	// ����״̬
	char state = s[8];
	client.state = atoi(&state);
	// IP��ַ
	int ipLen = CharsWithDeliLen(s) - 9;
	char *ia = new char[ipLen + 1]();	// ����+1�����Ҽ�()����ʼ�������ַ�Ϊ'\0'
	for (int i = 0; i < ipLen; i++) {
		ia[i] = s[i + 9];
	}
	client.ipAddr = ia;

	return client;
}

/* --------------------------------
- �����ַ������ӡ�ͻ����б�
- s���ַ����飨������β��ǣ�
- :return��VOID
-------------------------------- */
void printClientList(char * s)
{
	cout << endl << "�ͻ����б�Ϊ��" << endl;
	char* client = new char[64];
	int len = 0;
	for (int i = 0; i < CharsWithDeliLen(s); i++)
	{
		client[len++] = s[i];
		if (s[i] == CLIENT_DELIMITER)
		{
			client[len - 1] = DELIMITER;
			Client c = charsToClient(client);
			cout << c.id << " "
				<< c.ipAddr << ":" << c.port << " ";
			if (c.state == STATE_ON)
				cout << "ON" << endl;
			else
				cout << "OFF" << endl;
			// ���
			memset(client, 0, len);
			len = 0;
		}
	}
	cout << endl;
}

/* --------------------------------
- ��װ��Ϣ����
- socket���׽��ֱ��
- msg����Ϣ
- :return����Ϣ����
- NOTE�������ݵĳ�������<calcMessageLen>����
-------------------------------- */
char * formMessage(int socket, const char * msg)
{
	char *data = new char[1024];
	char *idBytes = intToBytes(socket);
	for (int i = 0; i < 4; i++) {
		data[i] = idBytes[i];
	}
	strcpy(data + 4, msg);

	return data;
}

/* --------------------------------
- ������Ϣ���ݵĳ���
- msg����Ϣ����
- :return������
-------------------------------- */
int calcMessageLen(char* msg)
{
	char *m = new char[1024];
	strcpy(m, msg + 4);
	return 4 + strlen(m);
}

/* --------------------------------
- ����Ϣ�����л�ȡ�ͻ��˱��
- msg����Ϣ����
- :return���ͻ��˱��
-------------------------------- */
int getIdFromMessage(const char * msg)
{
	// ���
	char *idBytes = new char[4];
	for (int i = 0; i < 4; ++i) {
		idBytes[i] = msg[i];
	}
	int id = bytesToInt(idBytes);
	free(idBytes);

	return id;
}

/* --------------------------------
- ����Ϣ�����л�ȡ��Ϣ
- msg����Ϣ����
- :return����Ϣ�ַ�����
-------------------------------- */
char * getMsgFromMessage(const char * msg)
{
	char *m = new char[1024];
	strcpy(m, msg + 4);
	return m;
}

/* --------------------------------
- ��װ������Ϣ����
- error���������
- msg��������Ϣ
- :return����Ϣ����
-------------------------------- */
char * formErrorMessage(int error, const char * msg)
{
	char *data = new char[64]();
	// ������
	//char errorCode;
	itoa(error, data, 10);
	//data[0] = errorCode;
	// ������Ϣ
	strcpy(data + 1, msg);

	return data;
}

/* --------------------------------
- �Ӵ�����Ϣ�л�ȡ�������
- em��������Ϣ
- :return���������
-------------------------------- */
int getErrorCode(const char * em)
{
	return atoi(&em[0]);
}

/* --------------------------------
- �Ӵ�����Ϣ�л�ȡ���������Ϣ
- em������
- :return��������Ϣ
-------------------------------- */
char * getErrorInfo(const char * em)
{
	char *msg = new char[64]();
	strcpy(msg, em + 1);
	return msg;
}

void packetToCharsTest()
{
	/* Packet -> Chars���� */
	Packet p;
	p.req_type = REQ_LIST;
	p.res_type = RES_REPLY;
	p.data = "test";
	p.length = 6 + strlen(p.data);

	char *s = packetToChars(p);
	int len = CharsWithDeliLen(s);
	std::cout << "����Ϊ��" << len << std::endl;
	for (int i = 0; i < len; ++i) {
		cout << (int)s[i] << " ";
	}
	cout << endl;
}
void clientToCharsTest()
{
	/* Client -> Chars���� */
	Client c;
	c.id = 100;
	c.port = 4221;
	c.state = STATE_ON;
	c.ipAddr = new char[9];
	strcpy(c.ipAddr, "127.0.0.1");

	char *cc = clientToChars(c);
	std::cout << CharsWithDeliLen(cc) << std::endl;
}
void charsToPacketTest()
{
	Packet p;
	p.req_type = REQ_LIST;
	p.res_type = RES_REPLY;
	p.data = "���Ĳ���";
	p.length = 6 + strlen(p.data);

	char *s = packetToChars(p);
	Packet p2 = charsToPacket(s);

	cout << p2.req_type << " " << p2.res_type << " " << p2.length << " " << p2.data << endl;
}
void charsToClientTest()
{
	Client c;
	c.id = 100;
	c.port = 4221;
	c.state = STATE_ON;
	c.ipAddr = new char[9];
	strcpy(c.ipAddr, "127.0.0.1");

	char *cc = clientToChars(c);
	Client c2 = charsToClient(cc);

	cout << c2.id << " " << c2.ipAddr << ":" << c2.port << " " << c2.state << endl;
}