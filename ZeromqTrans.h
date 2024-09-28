#pragma once
#include <iostream>
#include <zmq.h>
#include <string>
#include <thread>
class ZeromqTrans
{
public:
	ZeromqTrans();
	~ZeromqTrans();
	bool startService(std::string address, int port, int checkport);
	bool startClient(std::string address, int selfport, int port, int checkport);
	int sendData(char* pData, int nLen);
	int recvData(char* pData, int nLen);
	void disconnect();

private:
	void checkConnection();
private:
	void* m_pCheckContext = nullptr;
	void* m_pPuberContext = nullptr;
	void* m_pSuberContext = nullptr;

	void* m_pCheckSocket = nullptr;
	void* m_pPuberSocket = nullptr;
	void* m_pSuberSocket = nullptr;

	bool m_isRunning = true;
};

