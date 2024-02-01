/*
内容:
    URGplusURG8+socket4
情報:2022/10/13 N.Hirota

アドレス:

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
#define ESPIPADDR  "192.168.218.233"
#define BUTTON_ID1 0

int recvCount = 0;
int walkCount = 0;
int disDegree = 0;

char iRecvData[14];
char iRecvDataA[2];
char iRecvDataB[2];
char iRecvDataC[2];
char iRecvDataD[2];
char iRecvDataE[2];
char iRecvDataF[2];

using namespace std;
#include "Urg_driver.h"
#include <C:\Users\tombow\Documents\GitHub\ARILab\urg_library\samples\cpp\Connection_information_me.h>
#include "math_utilities.h"
#include <iostream>


int MyPaint(HWND hwnd);
int Mytest(HWND hwnd);
int ESPSocket(HWND hwnd);

char pBuff[100];
bool txtOutSituation = 0;

int xWalkLog[100];
int yWalkLog[100];
int logCount = 0;

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
        //InvalidateRect(hwnd, NULL, TRUE);
        //MyPaint(hwnd);
        ESPSocket(hwnd);
        return 0;
	case WM_COMMAND:
        switch (LOWORD(wp)) {
        case BUTTON_ID1:
            MessageBox(hwnd, TEXT("STOP"), TEXT("BUTTON1"), MB_OK);
            txtOutSituation++;//1になるはず
            //Mytext(hwnd, LRFData);
        }
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

    while (true) {
        //PM_REMOVEはメッセージ処理後に削除することを表す。基本PM_REMOVEでOK
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            //処理
            ESPSocket(hwnd);
        }
    }

}

int MyPaint(HWND hwnd) {

    return 0;
}
int ESPSocket(HWND hwnd) {
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    const char* sendbuf = "connection start";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

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

   // ここアリにすると謎のフリーズ無限ループに突入する
   iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    
    /*printf("Bytes Sent: %ld\n", iResult);
    sprintf_s(pBuff, "Bytes Sent: %ld\n", iResult);
    cout << pBuff;
    MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);*/
    
    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    // Receive until the peer closes the connection
    do {
        InvalidateRect(hwnd, NULL, TRUE);
        // 
        //繰り返す処理
        HDC hdc;
        PAINTSTRUCT ps;
        hdc = BeginPaint(hwnd, &ps);
        HPEN        hPen, hOldPen;
        hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));       // 幅3ピクセルの青ペン
        hOldPen = (HPEN)SelectObject(hdc, hPen);          // hPen を選択

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        printf("bytes:%3d, %3s\n", iResult, recvbuf);
        if (iResult > 0) {
            if (recvCount == 0) {
                memcpy(iRecvDataA, recvbuf, sizeof(recvbuf));
                //頭がSなら次へ、ちがければもう一回送るようにESPに要求。もういちどここから
                string strA(iRecvDataA, sizeof(iRecvDataA));
                //if (strA[0] != 'S') {
                  //  MessageBox(NULL, TEXT("受信エラー"), TEXT("ERROR"), MB_OK);
                //}
                //else {
                   recvCount++;
                //}
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
            else if (recvCount == 4) {
                memcpy(iRecvDataE, recvbuf, sizeof(recvbuf));
                recvCount++;
            }
            else {

                memcpy(iRecvDataF, recvbuf, sizeof(recvbuf));
                sprintf(iRecvData, "%s%s%s%s%s%s\0", iRecvDataA, iRecvDataB, iRecvDataC, iRecvDataD, iRecvDataE, iRecvDataF);
                printf("%12s\n", iRecvData);
                string str(iRecvData, sizeof(iRecvData));
                MessageBox(hwnd, iRecvData, "Msg title", MB_OK | MB_ICONQUESTION);
                //頭はマイナス符号が入っている可能性がある,
                int walkX = int(str[2]- '0') * 1000 + int(str[3] - '0') * 100 + int(str[4] - '0') * 10 + int(str[5] - '0');
                if (str[1] == '-') walkX *= -1;
                int walkY = int(str[7] - '0') * 1000 + int(str[8] - '0') * 100 + int(str[9] - '0') * 10 + int(str[10] - '0');
                if (str[6] == '-') walkY *= -1;


                printf("walkX:%4d, walkY:%4d\nS", walkX, walkY);
                //string name = "stackoverflow";
                sprintf_s(pBuff, "walkX:  %d,walkY: %d ", walkX, walkY);
                MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
                //表示用に変換
                
                int xPrint, yPrint = 0;
                xPrint = ( walkX + 2000) / 8;//TODO:実はこっちもマイナスがいるのでは？？？
                yPrint = (-1 * walkY + 2000) / 8;//y軸方向違う 10000 40
                int frameSize = 500;
                for (int iCount = 0; iCount < frameSize; iCount++) {
                    SetPixel(hdc, iCount, 0, 0x00);
                    SetPixel(hdc, iCount, frameSize, 0x00);
                    SetPixel(hdc, 0, iCount, 0x00);
                    SetPixel(hdc, frameSize, iCount, 0x00);
                }
                
                txtOutSituation = 0;

                
                //配列に保存
                xWalkLog[logCount] = xPrint;
                yWalkLog[logCount] = yPrint;
                logCount++;

                
                //配列に保存されてる分だけ描画
                for (int i = 0; i < logCount; i++) {
                    //初回なら始点に移動
                    if (i == 0)
                        MoveToEx(hdc, 250, 250, NULL);
                    LineTo(hdc, xWalkLog[i], yWalkLog[i]);
                    MoveToEx(hdc, xWalkLog[i], yWalkLog[i], NULL);
                }
                sprintf_s(pBuff, "%d回分 ", logCount);
                MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
                recvCount = 0;
            }


        }
        else if (iResult == 0) {
            printf("Connection closed\n");
            //MessageBox(NULL, TEXT("Connection closed"), TEXT("ESPSocket"), MB_OK);

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