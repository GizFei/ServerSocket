#include "pch.h"
#include <iostream>
#include "define.h"

using namespace std;

/* --------------------------------
- 32位的Int转换char[4]
- n：32-bit整数
- :return：char[4]数组
-------------------------------- */
char* intToBytes(int n) {
	// 使用new分配空间，可以返回正确结果
	char *bytes = new char[4];
	bytes[0] = (n >> 24) & 0xFF;
	bytes[1] = (n >> 16) & 0xFF;
	bytes[2] = (n >> 8) & 0xFF;
	bytes[3] = n & 0xFF;

	return bytes;
}

/* --------------------------------
- char[4]转换为32位的Int
- bytes：char[4]数组
- :return：32-bit整数
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
- 计算带边界符的字符数组长度
- s：数据包字符数组
- :return：长度（不包括结尾标记）
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
- 计算Packet需要的字符数组长度
- p：Packet结构体引用
- :return：长度
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
- 数据包结构体转换为字符数组
- p：数据包
- :return：字符数组
- NOTE：结尾标记为char(255)，该数组的长度请使用<CharsWithDeliLen>计算长度
-------------------------------- */
char* packetToChars(Packet& p)
{
	char *c = new char[p.length + 1];

	itoa(p.req_type, &c[0], 10);
	itoa(p.res_type, &c[1], 10);
	// 长度 
	char *l = intToBytes(p.length);
	for (int i = 0; i < 4; ++i) {
		c[i + 2] = l[i];
	}
	// 数据 
	for (int i = 6; i < p.length; ++i) {
		c[i] = p.data[i - 6];
	}
	// 结尾标记 
	c[p.length] = DELIMITER;
	return c;
}

/* --------------------------------
- 字符数组转换为数据包结构体
- s：字符数组（包括结尾标记）
- :return：数据包结构体
-------------------------------- */
Packet charsToPacket(char* s)
{
	Packet p;
	char req = s[0];
	char res = s[1];
	p.req_type = atoi(&req);
	p.res_type = atoi(&res);
	// 长度
	char *bytes = new char[4]();
	for (int i = 0; i < 4; ++i) {
		bytes[i] = s[i + 2];
	}
	p.length = bytesToInt(bytes);

	int len = p.length - 6;
	char *d = new char[len + 1]();	// 长度+1，并且加()，初始化所有字符为'\0'
	for (int i = 0; i < len; i++) {
		d[i] = s[i + 6];
	}
	p.data = d;

	return p;
}

/* --------------------------------
- 计算Client的字符数组长度（IPAddr长度不同）
- client：Client结构体引用
- :return：长度
-------------------------------- */
int calcClientLen(Client &client)
{
	return (9 + strlen(client.ipAddr));
}

/* --------------------------------
- Client结构体转换为字符数组
- client：Client结构体引用
- :return：字符数组（包括结尾标记）
- NOTE：结尾标记为char(255)，该数组的长度请使用<packetCharsLen>计算长度
-------------------------------- */
char* clientToChars(Client &client) {
	int len = calcClientLen(client);
	char *c = new char[len + 1];
	// 客户端编号
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
- 字符数组转换为Client结构体
- s：字符数组（包括结尾标记）
- :return：Client结构体
-------------------------------- */
Client charsToClient(char *s)
{
	Client client;
	// 编号
	char *idBytes = new char[4];
	for (int i = 0; i < 4; ++i) {
		idBytes[i] = s[i];
	}
	client.id = bytesToInt(idBytes);
	// 端口号
	char *portBytes = new char[4];
	for (int i = 0; i < 4; ++i) {
		portBytes[i] = s[i + 4];
	}
	client.port = bytesToInt(portBytes);
	// 连接状态
	char state = s[8];
	client.state = atoi(&state);
	// IP地址
	int ipLen = CharsWithDeliLen(s) - 9;
	char *ia = new char[ipLen + 1]();	// 长度+1，并且加()，初始化所有字符为'\0'
	for (int i = 0; i < ipLen; i++) {
		ia[i] = s[i + 9];
	}
	client.ipAddr = ia;

	return client;
}

/* --------------------------------
- 根据字符数组打印客户端列表
- s：字符数组（包括结尾标记）
- :return：VOID
-------------------------------- */
void printClientList(char * s)
{
	cout << endl << "客户端列表为：" << endl;
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
			// 清空
			memset(client, 0, len);
			len = 0;
		}
	}
	cout << endl;
}

/* --------------------------------
- 组装消息数据
- socket：套接字编号
- msg：信息
- :return：消息数据
- NOTE：该数据的长度请用<calcMessageLen>计算
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
- 计算消息数据的长度
- msg：消息数据
- :return：长度
-------------------------------- */
int calcMessageLen(char* msg)
{
	char *m = new char[1024];
	strcpy(m, msg + 4);
	return 4 + strlen(m);
}

/* --------------------------------
- 从消息数据中获取客户端编号
- msg：消息数据
- :return：客户端编号
-------------------------------- */
int getIdFromMessage(const char * msg)
{
	// 编号
	char *idBytes = new char[4];
	for (int i = 0; i < 4; ++i) {
		idBytes[i] = msg[i];
	}
	int id = bytesToInt(idBytes);
	free(idBytes);

	return id;
}

/* --------------------------------
- 从消息数据中获取消息
- msg：消息数据
- :return：消息字符数组
-------------------------------- */
char * getMsgFromMessage(const char * msg)
{
	char *m = new char[1024];
	strcpy(m, msg + 4);
	return m;
}

/* --------------------------------
- 组装错误消息数据
- error：错误代码
- msg：错误信息
- :return：消息数据
-------------------------------- */
char * formErrorMessage(int error, const char * msg)
{
	char *data = new char[64]();
	// 错误码
	//char errorCode;
	itoa(error, data, 10);
	//data[0] = errorCode;
	// 错误信息
	strcpy(data + 1, msg);

	return data;
}

/* --------------------------------
- 从错误信息中获取错误代码
- em：错误信息
- :return：错误代码
-------------------------------- */
int getErrorCode(const char * em)
{
	return atoi(&em[0]);
}

/* --------------------------------
- 从错误信息中获取具体错误信息
- em：错误
- :return：错误信息
-------------------------------- */
char * getErrorInfo(const char * em)
{
	char *msg = new char[64]();
	strcpy(msg, em + 1);
	return msg;
}

void packetToCharsTest()
{
	/* Packet -> Chars测试 */
	Packet p;
	p.req_type = REQ_LIST;
	p.res_type = RES_REPLY;
	p.data = "test";
	p.length = 6 + strlen(p.data);

	char *s = packetToChars(p);
	int len = CharsWithDeliLen(s);
	std::cout << "长度为：" << len << std::endl;
	for (int i = 0; i < len; ++i) {
		cout << (int)s[i] << " ";
	}
	cout << endl;
}
void clientToCharsTest()
{
	/* Client -> Chars测试 */
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
	p.data = "中文测试";
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