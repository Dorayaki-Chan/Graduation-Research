/*
内容:
    URGplusURG8+socket4
情報:2022/10/13 N.Hirota

アドレス:C:\Users\tombow\Documents\GitHub\ARILab\urg_library\vs2017\cpp\x64\Debug\URGPDR.exe
なんぞや:

LRFのやつと合体する下準備、とりまESPと通信して、描画することを目指す
注意事項:txt開くときANSIで開いてくれ

TODO:



*/
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 2
#define DEFAULT_PORT "5000"
#define ESPIPADDR  "192.168.11.18"

int  recvCount = 0;
int walkCount = 0;
int disDegree = 0;

char iRecvData[10];
char iRecvDataA[2];
char iRecvDataB[2];
char iRecvDataC[2];
char iRecvDataD[2];
char iRecvDataE[2];

using namespace std;
#include "Urg_driver.h"
#include <C:\Users\tombow\Documents\GitHub\ARILab\urg_library\samples\cpp\Connection_information_me.h>
#include "math_utilities.h"
#include <iostream>


int MyPaint(HWND hwnd);
int Mytest(HWND hwnd);
int ESPSocket(HWND hwnd);

int Mytest(HWND hwnd) {
    HDC hdc;
    PAINTSTRUCT ps;

    hdc = BeginPaint(hwnd, &ps);
    for (int iCount = 0; iCount < 1024; iCount++)
        SetPixel(hdc, 10 + iCount, 10, 0xFF);//(???,x,y,色)
    EndPaint(hwnd, &ps);

    return 0;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        MyPaint(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, int nCmdShow) {
    HWND hwnd;
    MSG msg;
    WNDCLASS winc;

    winc.style = CS_HREDRAW | CS_VREDRAW;
    winc.lpfnWndProc = WndProc;
    winc.cbClsExtra = winc.cbWndExtra = 0;
    winc.hInstance = hInstance;
    winc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    winc.hCursor = LoadCursor(NULL, IDC_ARROW);
    winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    winc.lpszMenuName = NULL;
    winc.lpszClassName = TEXT("KITTY");

    if (!RegisterClass(&winc)) return -1;

    hwnd = CreateWindow(
        TEXT("KITTY"), TEXT("Kitty on your lap"),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return -1;

    while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
    return msg.wParam;
}

int MyPaint(HWND hwnd) {
    int iCount;
    ESPSocket(hwnd);

    return 0;
}

int ESPSocket(HWND hwnd) {

    HDC hdc;
    PAINTSTRUCT ps;
    hdc = BeginPaint(hwnd, &ps);
    int frameSize = 500;
    for (int iCount = 0; iCount < frameSize; iCount++) {
        SetPixel(hdc, iCount, 0, 0x00);
        SetPixel(hdc, iCount, frameSize, 0x00);
        SetPixel(hdc, 0, iCount, 0x00);
        SetPixel(hdc, frameSize, iCount, 0x00);
    }
    EndPaint(hwnd, &ps);
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    const char* sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    //入力が二つ（.exeとipadress）じゃなければ
    /*if (argc != 2) {
        printf("usage: %s please write IPadress \n", argv[0]);
        return 1;
    }*/

    // Initialize Winsock winsockの初期化

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        MessageBox(NULL, TEXT("WSAStartup failed with error"), TEXT("ESPSocket"), MB_OK);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    // Resolve the server address and port 鯖のアドレスかポートのエラー
    iResult = getaddrinfo(ESPIPADDR, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        MessageBox(NULL, TEXT("getaddrinfo failed with error"), TEXT("ESPSocket"), MB_OK);
        WSACleanup();
        return 1;
    }
    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            MessageBox(NULL, TEXT("socket failed with error"), TEXT("ESPSocket"), MB_OK);
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        MessageBox(NULL, TEXT("Unable to connect to server"), TEXT("ESPSocket"), MB_OK);
        WSACleanup();
        return 1;
    }

    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    MessageBox(NULL, TEXT("わぁ"), TEXT("Test"), MB_OK);
    // Receive until the peer closes the connection
    do {
        MessageBox(NULL, TEXT("わぁ"), TEXT("Test"), MB_OK);
        //繰り返す処理
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        printf("bytes:%3d, %3s\n", iResult, recvbuf);
        if (iResult > 0) {
            if (recvCount == 0) {
                memcpy(iRecvDataA, recvbuf, sizeof(recvbuf));
                recvCount++;
            }
            else if (recvCount == 1) {
                memcpy(iRecvDataB, recvbuf, sizeof(recvbuf));
                recvCount++;
            }
            else if (recvCount == 2) {
                memcpy(iRecvDataC, recvbuf, sizeof(recvbuf));
                recvCount++;
            }
            else if (recvCount == 3) {
                memcpy(iRecvDataD, recvbuf, sizeof(recvbuf));
                recvCount++;
            }
            else {
                memcpy(iRecvDataE, recvbuf, sizeof(recvbuf));
                sprintf(iRecvData, "%s%s%s%s%s\0", iRecvDataA, iRecvDataB, iRecvDataC, iRecvDataD, iRecvDataE);
                printf("%10s\n", iRecvData);
                string str(iRecvData, sizeof(iRecvData));
                //頭はマイナス符号が入っている可能性がある,
                int walkX = int(str[1] - '0') * 1000 + int(str[2] - '0') * 100 + int(str[3] - '0') * 10 + int(str[4] - '0');
                if (str[0] == '-') walkX *= -1;
                int walkY = int(str[6] - '0') * 1000 + int(str[7] - '0') * 100 + int(str[8] - '0') * 10 + int(str[9] - '0');
                if (str[5] == '-') walkY *= -1;
                printf("walkX:%4d, walkY:%4d\n", walkX, walkY);

                recvCount = 0;
            }


        }
        else if (iResult == 0) {
            printf("Connection closed\n");
            MessageBox(NULL, TEXT("Connection closed"), TEXT("ESPSocket"), MB_OK);

        }
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            MessageBox(NULL, TEXT("recv failed with error:"), TEXT("ESPSocket"), MB_OK);
        }

    } while (iResult > 0);//ペアが接続を閉じるまで
    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}