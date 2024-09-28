#include "ZeromqTrans.h"

using namespace std;

ZeromqTrans::ZeromqTrans()
{

}

ZeromqTrans::~ZeromqTrans()
{
	m_isRunning = false;
	if (m_pCheckSocket) {
		zmq_close(m_pCheckSocket);
	}
	if (m_pPuberSocket) {
		zmq_close(m_pPuberSocket);
	}
	if (m_pSuberSocket) {
		zmq_close(m_pSuberSocket);
	}
	if (m_pCheckContext) {
		zmq_ctx_destroy(m_pCheckContext);
	}
	if (m_pPuberContext) {
		zmq_ctx_destroy(m_pPuberContext);
	}
	if (m_pSuberContext) {
		zmq_ctx_destroy(m_pSuberContext);
	}
}

bool ZeromqTrans::startService(std::string address, int port, int checkport)
{
	m_pCheckContext = zmq_ctx_new();
	m_pPuberContext = zmq_ctx_new();
	if (!m_pCheckContext) {
		cout << "Context error!" << endl;
		return false;
	}
	if (!m_pPuberContext) {
		cout << "Context error!" << endl;
		return false;
	}

	m_pCheckSocket = zmq_socket(m_pCheckContext, ZMQ_REP);
	m_pPuberSocket = zmq_socket(m_pPuberContext, ZMQ_PUB);
	if (!m_pCheckSocket) {
		cout << "Socket error!" << endl;
		return false;
	}
	if (!m_pPuberSocket) {
		cout << "Socket error!" << endl;
		return false;
	}
	string strIP = string("tcp://*:") + std::to_string(port); //server pub port
	int ret = -1;
	ret = zmq_bind(m_pPuberSocket, strIP.data());
	if (ret == 0) {
		cout << "Socket bind success!" << endl;
	}
	else {
		cout << "Socket bind error!" << endl;
		return false;
	}

	strIP = string("tcp://*:") + std::to_string(checkport); //server check port
	ret = -1;
	ret = zmq_bind(m_pCheckSocket, strIP.data());
	if (ret == 0) {
		cout << "Socket bind success!" << endl;
	}
	else {
		cout << "Socket bind error!" << endl;
		return false;
	}
	thread t(&ZeromqTrans::checkConnection, this);
	t.detach();
	return true;
}

bool ZeromqTrans::startClient(std::string address, int selfport, int port, int checkport)
{
	m_pCheckContext = zmq_ctx_new();
	m_pSuberContext= zmq_ctx_new();
	if (!m_pCheckContext) {
		cout << "Context error!" << endl;
		return false;
	}
	if (!m_pSuberContext) {
		cout << "Context error!" << endl;
		return false;
	}

	m_pCheckSocket = zmq_socket(m_pCheckContext, ZMQ_REQ);
	m_pSuberSocket = zmq_socket(m_pSuberContext, ZMQ_SUB);
	if (!m_pCheckSocket) {
		cout << "Socket error!" << endl;
		return false;
	}
	if (!m_pSuberSocket) {
		cout << "Socket error!" << endl;
		return false;
	}
	int tureValue = 1;
	zmq_setsockopt(m_pSuberSocket, ZMQ_CONFLATE, &tureValue, sizeof(tureValue));

	string strIP = string("tcp://127.0.0.1:") + std::to_string(port); //client sub port
	int ret = -1;
	ret = zmq_connect(m_pSuberSocket, strIP.data());
	if (ret == 0) {
		cout << "Socket connect success!" << endl;
	}
	else {
		cout << "Socket connect error!" << endl;
		return false;
	}
	zmq_setsockopt(m_pSuberSocket, ZMQ_SUBSCRIBE, "", 0);

	strIP = string("tcp://127.0.0.1:") + std::to_string(checkport); //client check port
	ret = -1;
	ret = zmq_connect(m_pCheckSocket, strIP.data());
	if (ret == 0) {
		cout << "Socket connect success!" << endl;
	}
	else {
		cout << "Socket connect error!" << endl;
		return false;
	}

	m_pPuberContext = zmq_ctx_new();
	if (!m_pCheckContext) {
		cout << "Context error!" << endl;
		return false;
	}

	m_pPuberSocket = zmq_socket(m_pPuberContext, ZMQ_PUB);
	if (!m_pPuberSocket) {
		cout << "Socket error!" << endl;
		return false;
	}

	strIP = string("tcp://*:") + std::to_string(selfport);	//client pub port
	ret = -1;
	ret = zmq_bind(m_pPuberSocket, strIP.data());
	if (ret == 0) {
		cout << "Socket bind success!" << endl;
	}
	else {
		cout << "Socket bind error!" << endl;
		return false;
	}

	strIP = string("tcp://") + address + ":" + std::to_string(selfport);
	cout << "client begin checkConnection" << endl;
	ret = zmq_send(m_pCheckSocket, strIP.data(), strIP.size(), 0);
	cout << "send check data, ret = " << ret << endl;
	char pData[1024];
	ret = -1;
	ret = zmq_recv(m_pCheckSocket, pData, 1024, 0);
	cout << "recv check data, ret = " << ret << endl;

	return true;
}
int ZeromqTrans::sendData(char* pData, int nLen)
{
	cout << "zmq sendData Begin" << endl;
	int ret = -1;
	if (!m_pPuberSocket) {
		cout << "zmq Socket error" << endl;
		return ret;
	}

	ret = zmq_send(m_pPuberSocket, pData, nLen, 0);
	if (ret == -1) {
		cout << "zmq send error" << endl;
		return ret;
	}
	
	cout << "zmq send success, ret: " << ret << endl;
	return ret;
}

int ZeromqTrans::recvData(char* pData, int nLen)
{
	cout << "zmq rendData Begin" << endl;
	int ret = -1;
	if (!m_pSuberSocket) {
		cout << "zmq Socket error" << endl;
		return ret;
	}

	ret = zmq_recv(m_pSuberSocket, pData, nLen, 0);
	if (ret == -1) {
		cout << "zmq recv error" << endl;
		return ret;
	}

	cout << "zmq recv success, ret: " << ret << endl;
	return ret;
}

void ZeromqTrans::disconnect()
{
	cout << "zmq disconnect begin" << endl;

	//外部重新调用一次 startClient, Server不用特意重新连接
	if (m_pCheckSocket) {
		zmq_close(m_pCheckSocket);
	}
	if (m_pPuberSocket) {
		zmq_close(m_pPuberSocket);
	}
	if (m_pSuberSocket) {
		zmq_close(m_pSuberSocket);
	}
	if (m_pCheckContext) {
		zmq_ctx_destroy(m_pCheckContext);
	}
	if (m_pPuberContext) {
		zmq_ctx_destroy(m_pPuberContext);
	}
	if (m_pSuberContext) {
		zmq_ctx_destroy(m_pSuberContext);
	}

}

void ZeromqTrans::checkConnection()
{
	char pData[1024];
	int ret = -1;
	//string strIP1 = string("tcp://127.0.0.1:1901"); //server sub port
	
	cout << "Service begin checkConnection" << endl;
	while (m_isRunning)
	{
		ret = zmq_recv(m_pCheckSocket, pData, 1024, 0);
		cout << "recv check data, ret = " << ret << endl;

		string strIP; //server sub port
		for (int i = 0; i < ret; i++)	strIP += pData[i];
		cout << "strIP: " << strIP << endl;

		if (!m_pSuberContext) {
			m_pSuberContext = zmq_ctx_new();
			if (!m_pSuberContext) {
				cout << "Context error!" << endl;
				return;
			}
		}
		if (!m_pSuberSocket) {
			m_pSuberSocket = zmq_socket(m_pSuberContext, ZMQ_SUB);
			if (!m_pSuberSocket) {
				cout << "Socket error!" << endl;
				return;
			}
		}

		ret = -1;
		ret = zmq_connect(m_pSuberSocket, strIP.c_str());
		if (ret == 0) {
			cout << "Socket connect success!" << endl;
		}
		else {
			cout << "Socket connect error!" << endl;
			return;
		}
		zmq_setsockopt(m_pSuberSocket, ZMQ_SUBSCRIBE, "", 0);

		ret = -1;
		ret = zmq_send(m_pCheckSocket, pData, 1024, 0);
		cout << "send check data, ret = " << ret << endl;


	}
}