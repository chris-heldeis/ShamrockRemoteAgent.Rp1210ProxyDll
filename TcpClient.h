#pragma once
#include <thread>
#include <atomic>
#include "Platform.h"
#include "Protocol.h"

class ProxyCore;
class TcpClient 
{
public:
	void start(ProxyCore*); 
	void stop();
	void sendConnect(short, short, char*, long, long, short);
	void sendDisconnect(short);
	void sendData(short, char*, short, short, short);
	void readData(short, short, short);
	void sendCommand(short, short, char*, short);
	void readVersion();
	void getErrorMsg(short);
	void getHardwareStatus(short, short, short);
	void getLastErrorMsg(short, short);
	void readDetailedVersion(short);
	void ioctl(short, long, void*);
private:
	  void run(); 
	  bool sendPacket(const MsgHeader&, const uint8_t*);
	  ProxyCore* core = nullptr; 
	  std::thread th; 
	  std::atomic<bool> runFlag{false};
	  SOCKET s = INVALID_SOCKET;
};
