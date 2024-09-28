// ZeroMQService.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "ZeromqTrans.h"

using namespace std;

int main()
{
    ZeromqTrans zeromqtrans;
    zeromqtrans.startService("127.0.0.1", 1900, 1902);
    
    thread ServerPub([&]() {
        cout << "ServerPub begin " << endl;
        this_thread::sleep_for(std::chrono::milliseconds(1000));
        while (true) {
            char pData[1024];
            zeromqtrans.sendData(pData, 1024);
            this_thread::sleep_for(std::chrono::milliseconds(500));
        }

    });

    thread ServerSub([&]() {
        cout << "ServerSub begin " << endl;
        this_thread::sleep_for(std::chrono::milliseconds(100));
        while (true) {
            char pData[512];
            zeromqtrans.recvData(pData, 512);
            this_thread::sleep_for(std::chrono::milliseconds(1500));
        }
    });
    ServerPub.join();
    ServerSub.join();
    system("pause");
    return 0;
    
    void* test = zmq_ctx_new();
    if (test) {
        cout << "success " << endl;
    }

    void* pp = nullptr;
    pp = zmq_socket(test, ZMQ_REP);
    int ret = zmq_bind(pp, "tcp://127.0.0.1:1900");

    char pData[1024] = { 0 };
    thread t1([&]() {
        cout << "begin recv" << endl;
        ret = zmq_recv(pp, pData, 1024, 0);
        cout << "end recv" << endl;
        string strIP; //server sub port
        for (int i = 0; i < ret; i++)	strIP += pData[i];
        cout << "-----" << strIP << "------" << endl;
    });
    //ret = zmq_recv(pp, pData, 1024, 0);
    //cout << "recv ret: " << pData[0] << endl;

    //ret = zmq_send(pp, pData, 1024, 0);
    //cout << "sned ret: " << pData[0] << endl;
    t1.join();
    cout << "ret: " << ret;
    
  
    return 0;
    this_thread::sleep_for(std::chrono::seconds(5));
    
 

    //ret = zmq_recv(pp, pData, 1024, 0);
    //cout << "recv ret: " << ret << endl;
    ret = -1;
    int i = 0;
    while (true) {
        if (i >= 10) i = 0;

        pData[0] = i + '0';
        ret = zmq_send(pp, pData, 1024, 0);
        cout << "sned ret: " << pData[0] << endl;

        i++;
        this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    

    system("pause");
    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
