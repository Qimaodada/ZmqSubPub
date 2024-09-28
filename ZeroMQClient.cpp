// ZeroMQClient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <zmq.h>
#include <thread>
#include <chrono>
#include "ZeromqTrans.h"
using namespace std;

int main()
{
    ZeromqTrans zeromqtrans;
    zeromqtrans.startClient("127.0.0.1", 1901, 1900, 1902);
    
    thread ClientPub([&]() {
        cout << "ClientPub begin " << endl;
        while (true) {
            char pData[512];
            zeromqtrans.sendData(pData, 512);
            this_thread::sleep_for(std::chrono::milliseconds(1500));
        }

    });

    thread ClientSub([&]() {
        cout << "ClientSub begin " << endl;
        while (true) {
            char pData[1024];
            zeromqtrans.recvData(pData, 1024);
            this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });
    ClientPub.join();
    ClientSub.join();
    

    system("pause");
    return 0;


    void* test = zmq_ctx_new();
    if (test) {
        cout << "success " << endl;
    }

    void* pp = nullptr;
    pp = zmq_socket(test, ZMQ_REQ);

    int bzmqopt = 1;
    int ret = zmq_setsockopt(pp, ZMQ_CONFLATE, &bzmqopt, sizeof(bzmqopt));
    cout << "set ZMQ_CONFLATE, ret: " << ret << endl;
    ret = zmq_connect(pp, "tcp://127.0.0.1:1900");
    
    char pData[] ="tcp://127.0.0.1:1900";
    ret = zmq_send(pp, pData, strlen(pData), 0);
    cout << "send ret: " << ret << endl;

    ret = zmq_recv(pp, pData, 1024, 0);
    cout << "1recv ret: " << pData[0] << endl;

    cout << "ret: " << ret << endl;
    //zmq_setsockopt(pp, ZMQ_SUBSCRIBE, "", 0);
    return 0 ;

    //ret = zmq_send(pp, pData, 1024, 0);
    //cout << "sned ret: " << ret << endl;
    this_thread::sleep_for(std::chrono::milliseconds(1000));
    ret = -1;
    while (true) {
        ret = zmq_recv(pp, pData, 1024, 0);
        cout << "1recv ret: " << pData[0] << endl;
        this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    ret = zmq_recv(pp, pData, 1024, 0);
    cout << "1recv ret: " << pData[0] << endl;



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
