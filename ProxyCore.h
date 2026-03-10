#pragma once
#include <map>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "TcpClient.h"

class ProxyCore {
private:
	struct MsgData
	{
		MsgType type;
		std::vector<uint8_t> data;
	};
	struct MsgResult
	{
		short code;
		MsgData data;
	};
	struct Client {
		std::vector<MsgData> msgBuf;
		std::condition_variable cv;
	};
	std::mutex mtx;
	std::map<short, Client> clients;
	short nextId = 0;
	TcpClient tcp;
	bool tcpRunning = false;
public:
	void start(); 
	void stop();
	void addClient(short);
	short sendConnect(short, char*, long, long, short);
	short clientConnect(short, char*, long, long, short);
	short clientDisconnect(short);
	short sendMessage(short, char*, short, short, short);
	short readMessage(short, char*, short, short);
	short sendCommand(short, short, char*, short);
	void readVersion(char*, char*, char*, char*);
	short getErrorMsg(short, char*);
	short getHardwareStatus(short, char*, short, short);
	short getLastErrorMsg(short, short, int*, char*);
	short readDetailedVersion(short, char*, char*, char*);
	short ioctl(short, long, void*, void*);
	MsgResult getMessageData(short, MsgType);
	void injectBuffer(short, MsgType, std::vector<uint8_t>);
	HMODULE hModule;
	std::string lastError;
};
