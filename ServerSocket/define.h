#ifndef DEFINE_H
#define DEFINE_H

#include <WinSock2.h>
#pragma warning (disable : 4996)

#define DELIMITER (char)255
#define CLIENT_DELIMITER	 (char)254

// ��������
enum REQ_TYPE
{
	REQ_TIME,		// ��ȡʱ��
	REQ_NAME,	// ��ȡ����
	REQ_LIST,		// ��ȡ�ͻ����б�
	REQ_MSG		// ������Ϣ
};

// ���ݰ�����
enum RES_TYPE
{
	RES_REQUEST,		// �ͻ��˷�������
	RES_REPLY,		// �������Ӧ����
	RES_INSTRUCT		// ����˷���ָʾ
};

// ����״̬
enum STATE_TYPE
{
	STATE_ON,			// ������
	STATE_OFF		// δ����
};

// ������Ϣ������Ϣ
enum MSG_ERROR
{
	ERROR_NO_EXISTS,		// ������
	ERROR_NO_CONNECTED	// δ����
};

// ���ݰ��ṹ
struct Packet
{
	int req_type;		// ��������
	int res_type;		// ��Ӧ����
	int length;			// ���ݰ��ĳ���
	const char* data;	// ����
};

// �ͻ�����Ϣ�ṹ
struct Client
{
	SOCKET id;		// ��� 
	int port;			// �˿ں�
	int state;			// ����״̬
	char* ipAddr;		// IP��ַ
};

// 32λ��Intת��char[4]
char* intToBytes(int n);

// char[4]ת��Ϊ32λ��Int
int bytesToInt(char *bytes);

// �������ݰ��ַ����鳤��
int CharsWithDeliLen(char *s);

// ����Packet��Ҫ���ַ����鳤��
int calcPacketLen(Packet &p);

// ���ݰ��ṹ��ת��Ϊ�ַ�����
char* packetToChars(Packet& p);

// �ַ�����ת��Ϊ���ݰ��ṹ��
Packet charsToPacket(char* s);

// ����Client���ַ����鳤�ȣ�IPAddr���Ȳ�ͬ��
int calcClientLen(Client &client);

// Client�ṹ��ת��Ϊ�ַ�����
char* clientToChars(Client &client);

// �ַ�����ת��ΪClient�ṹ��
Client charsToClient(char *s);

// �����ַ������ӡ�ͻ����б�
void printClientList(char *s);

// ��װ��Ϣ����
char* formMessage(int socket, const char* msg);

// ������Ϣ���ݵĳ���
int calcMessageLen(char* msg);

// ����Ϣ�����л�ȡ�ͻ��˱��
int getIdFromMessage(const char* msg);

// ����Ϣ�����л�ȡ��Ϣ
char* getMsgFromMessage(const char* msg);

// ��װ������Ϣ
char *formErrorMessage(int error, const char* msg);

// �Ӵ�����Ϣ�л�ȡ�������
int getErrorCode(const char* em);

// �Ӵ�����Ϣ�л�ȡ���������Ϣ
char* getErrorInfo(const char *em);

void packetToCharsTest();
void clientToCharsTest();
void charsToPacketTest();
void charsToClientTest();

#endif // !DEFINE_H