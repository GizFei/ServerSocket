#ifndef DEFINE_H
#define DEFINE_H

#include <WinSock2.h>
#pragma warning (disable : 4996)

#define DELIMITER (char)255
#define CLIENT_DELIMITER	 (char)254

// 请求类型
enum REQ_TYPE
{
	REQ_TIME,		// 获取时间
	REQ_NAME,	// 获取名字
	REQ_LIST,		// 获取客户端列表
	REQ_MSG		// 发送消息
};

// 数据包类型
enum RES_TYPE
{
	RES_REQUEST,		// 客户端发送请求
	RES_REPLY,		// 服务端响应请求
	RES_INSTRUCT		// 服务端发送指示
};

// 连接状态
enum STATE_TYPE
{
	STATE_ON,			// 连接中
	STATE_OFF		// 未连接
};

// 发送消息错误信息
enum MSG_ERROR
{
	ERROR_NO_EXISTS,		// 不存在
	ERROR_NO_CONNECTED	// 未在线
};

// 数据包结构
struct Packet
{
	int req_type;		// 请求类型
	int res_type;		// 响应类型
	int length;			// 数据包的长度
	const char* data;	// 数据
};

// 客户端信息结构
struct Client
{
	SOCKET id;		// 编号 
	int port;			// 端口号
	int state;			// 连接状态
	char* ipAddr;		// IP地址
};

// 32位的Int转换char[4]
char* intToBytes(int n);

// char[4]转换为32位的Int
int bytesToInt(char *bytes);

// 计算数据包字符数组长度
int CharsWithDeliLen(char *s);

// 计算Packet需要的字符数组长度
int calcPacketLen(Packet &p);

// 数据包结构体转换为字符数组
char* packetToChars(Packet& p);

// 字符数组转换为数据包结构体
Packet charsToPacket(char* s);

// 计算Client的字符数组长度（IPAddr长度不同）
int calcClientLen(Client &client);

// Client结构体转换为字符数组
char* clientToChars(Client &client);

// 字符数组转换为Client结构体
Client charsToClient(char *s);

// 根据字符数组打印客户端列表
void printClientList(char *s);

// 组装消息数据
char* formMessage(int socket, const char* msg);

// 计算消息数据的长度
int calcMessageLen(char* msg);

// 从消息数据中获取客户端编号
int getIdFromMessage(const char* msg);

// 从消息数据中获取消息
char* getMsgFromMessage(const char* msg);

// 组装错误信息
char *formErrorMessage(int error, const char* msg);

// 从错误信息中获取错误代码
int getErrorCode(const char* em);

// 从错误信息中获取具体错误信息
char* getErrorInfo(const char *em);

void packetToCharsTest();
void clientToCharsTest();
void charsToPacketTest();
void charsToClientTest();

#endif // !DEFINE_H