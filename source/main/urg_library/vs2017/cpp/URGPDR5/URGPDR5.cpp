/*
内容:
    URGplusURG8+socket4
情報:2022/10/13 N.Hirota

アドレス:

なんぞや:
交差したところを検知
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
#include "Urg_driver.h"
#include <D:\Programming\SotsuKen\渡すプログラム\urg_library\samples\cpp\Connection_information_me.h>
#include "math_utilities.h"
#include <iostream>
#include <stdio.h>
#include <tchar.h>
#include <math.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 2
#define DEFAULT_PORT "5000"
#define ESPIPADDR  "10.84.233.91"//"192.168.11.18"//192.168.208.233, 
#define BUTTON_ID1 0
#define FRAMESIZE  500
#define STARTX 250 //歩行開始地点 250がLRFがある場所
#define STARTY 250
#define LRFX 250 //LRF地点 250がLRFがある場所
#define LRFY 250



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


#define BUTTON_ID1 0

int MyPaint(HWND hwnd);
int MyLRF(HWND hwnd);
int Mytest(HWND hwnd);
int ESPSocket(HWND hwnd);
int get_deistance_me(HWND hwnd);
int myABS(int n);
int myTXTout();

char pBuff[100];
bool txtOutSituation = 0;

int xWalkLog[100]={0};
int yWalkLog[100]={0};
int walkLogCount = 0;

int xObjectLog[100];//始終始終始終
int yObjectLog[100];
int  objPosNum = 0;//物体何個あったか

//表示用に変換
int xLRFLog[1081] = { 0 };
int yLRFLog[1081] = { 0 };
int xPrintLRF[1081], yPrintLRF[1081];
int  LRFNum = 0;//物体何個あったか

TCHAR out[64];

using namespace qrk;
using namespace std;//動的配列クラスvectorの名前空間指定

namespace
{
	void print_data(HWND hwnd, const Urg_driver& urg,
		const vector<long>& data, long time_stamp) {

		long min_distance = urg.min_distance();
		long max_distance = urg.max_distance();
		size_t data_n = data.size();
        LRFNum = data_n;


		long oldl = data[0]; //比較用
		bool objectFlag = true;
        objPosNum = 0;
        for (size_t i = 0; i < data_n; ++i) {
            long l = data[i];

            if ((l <= min_distance) || (l >= max_distance)) {
                continue;
            }

            double radian = urg.index2rad(i);
            long x = (long)(l * sin(radian));
            long y = (long)(l * cos(radian));

            xLRFLog[i] = x;
            yLRFLog[i] = y;
        }
	}
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        MyLRF(hwnd);
        MyPaint(hwnd);
        return 0;
	case WM_COMMAND:
        switch (LOWORD(wp)) {
        case BUTTON_ID1:
            MessageBox(NULL, TEXT("LOGOUT!!"), NULL, MB_OK);
            myTXTout();
            txtOutSituation=true;//1になるはず
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
	winc.lpszClassName = TEXT("LRFPrint");

	if (!RegisterClass(&winc)) return -1;

	hwnd = CreateWindow(
		TEXT("LRFPrint"), TEXT("LRFPrint on your lap"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL
	);
	CreateWindow(
		TEXT("BUTTON"), TEXT("STOP"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		550, 0, 100, 50,
		hwnd, (HMENU)BUTTON_ID1, hInstance, NULL
	);


	while (1) { /* メインループ */
        //PM_REMOVEはメッセージ処理後に削除することを表す。基本PM_REMOVEでOK
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {

			if (!GetMessage(&msg, NULL, 0, 0)) /* メッセージ処理 */
				return msg.wParam;

			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else {
            ESPSocket(hwnd);
		}
	}
}

int MyLRF(HWND hwnd) {
	if (get_deistance_me(hwnd))
		return 1;
	return 0;
}

int myTXTout() {
    HANDLE hFile;
    DWORD dwWriteSize;

    hFile = CreateFile(TEXT("test1115-1.txt"), GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBox(NULL, TEXT("ファイルが開けません"), NULL, MB_OK);
    }
    SetFilePointer(hFile, 0, NULL, FILE_END);

    wsprintf(out, TEXT("*****************LOG******************************** \r\n"));
    WriteFile(hFile, out, DWORD(lstrlen(out)), &dwWriteSize, NULL);

    for (int i = 0; i < walkLogCount; i++) {//LRF
        wsprintf(out, TEXT("N:%d X:%d ,Y:%d"), i,xWalkLog[i], yWalkLog[i]);
        WriteFile(hFile, out, DWORD(lstrlen(out)), &dwWriteSize, NULL);
    }

    WriteFile(hFile, TEXT("finish. \n"), 20, &dwWriteSize, NULL);
    CloseHandle(hFile);
    txtOutSituation = 0;
    return 0;
}



int MyPaint(HWND hwnd) {//描画は全部ここ
    HDC hdc;
    PAINTSTRUCT ps;

    hdc = BeginPaint(hwnd, &ps);
    
    HANDLE hFile;
    DWORD dwWriteSize;

    hFile = CreateFile(TEXT("shiken1115-1.txt"), GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        //MessageBox(NULL, TEXT("ファイルが開けません"), NULL, MB_OK);
    }
    SetFilePointer(hFile, 0, NULL, FILE_END);
    //枠
    for (int iCount = 0; iCount < FRAMESIZE; iCount++) {
        SetPixel(hdc, iCount, 0, 0x00);
        SetPixel(hdc, iCount, FRAMESIZE, 0x00);
        SetPixel(hdc, 0, iCount, 0x00);
        SetPixel(hdc, FRAMESIZE, iCount, 0x00);
    }

    for (int i = 0; i < LRFNum; i++) {//LRF
        xPrintLRF[i] = (-1 * xLRFLog[i] + 10000) / 40;//TODO:実はこっちもマイナスがいるのでは？？？
        //if (xPrintLRF[i] < 1|| xPrintLRF[i] > FRAMESIZE) xPrintLRF[i] = LRFX;//エラー処理、無限遠に行ってる奴はLRF
        yPrintLRF[i] = (-1 * yLRFLog[i] + 10000) / 40;//y軸方向違う
        //if (yPrintLRF[i] < 1|| yPrintLRF[i] > FRAMESIZE) yPrintLRF[i] = LRFX;//エラー処理、無限遠に行ってる奴はLRF
        SetPixel(hdc, xPrintLRF[i], yPrintLRF[i], 0x00);
        wsprintf(out, TEXT("LRF N: %d X: %d, Y : %d \r\n"), i, xLRFLog[i], yLRFLog[i]);
        WriteFile(hFile, out, DWORD(lstrlen(out)), &dwWriteSize, NULL);
    }
    
    //歩行
    HPEN        hPen, hOldPen;
    hPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 255));       // 幅3ピクセルの青ペン
    hOldPen = (HPEN)SelectObject(hdc, hPen);          // hPen を選択
    
    int xPrintAdd = 0, yPrintAdd = 0;
    for (int i = 0; i < walkLogCount; i++) {
        //初回なら始点に移動
        if (i == 0)
            MoveToEx(hdc, STARTX, STARTY, NULL);
        //表示用に変換    
        int xPrint, yPrint = 0;
        xPrint =      xWalkLog[i] / 4;//TODO:実はこっちもマイナスがいるのでは？？？
        yPrint = -1 * yWalkLog[i] / 4;//y軸方向違う 10000 40
        xPrintAdd += xPrint;
        yPrintAdd += yPrint;
        LineTo(hdc, xPrintAdd+ STARTX, yPrintAdd + STARTY);


        wsprintf(out, TEXT("Walk N:%d X:%d ,Y:%d \r\n"), i, xWalkLog[i], yWalkLog[i]);
        WriteFile(hFile, out, DWORD(lstrlen(out)), &dwWriteSize, NULL);

    }

    //足跡と交差してるかどうか
    int xWalkAdd= 0, yWalkAdd = 0;
    for (int i = 0; i < walkLogCount; i++) {
        //足跡の辺に対して始点A,終端B,すべてのLRFで取得した物体Pに対して傾きを調べる、AP PBの傾きが同じなら交差しているとする
        /*
            範囲を調べる、どっちが大きいか
        　　第一象限の話になるようにする
          　アークタンジェントで角度得る
           +0.25°の-0.25の誤差は許容する
        */
        if (xWalkLog[i] == 0 && yWalkLog[i] == 0) {
            continue;//動いてないなら交差しない
        }
        else {
            int xWalkBigSide = 0, yWalkBigSide = 0, xWalkSmallSide = 0, yWalkSmallSide = 0;

            if (xWalkLog[i] > 0) {
                xWalkBigSide = xWalkAdd + xWalkLog[i];
                xWalkSmallSide = xWalkAdd;
            }
            else {
                xWalkBigSide = xWalkAdd;
                xWalkSmallSide = xWalkAdd + xWalkLog[i];
            }

            if (yWalkLog[i] > 0) {
                yWalkBigSide = yWalkAdd + yWalkLog[i];
                yWalkSmallSide = yWalkAdd;
            }
            else {
                yWalkBigSide = yWalkAdd;
                yWalkSmallSide = yWalkAdd + yWalkLog[i];
            }



            for (int j = 0; j < LRFNum; j++) {//LRF
                if (xLRFLog[j] == 0 && yLRFLog[j] == 0) {
                    continue;//エラー処理の00をはじく
                }
                else {
                    /// <summary>
                    /// LRFはmm単位、ESPPのPDRはcm単位なのに注意
                    /// </summary>
                    
                    if (xWalkSmallSide < xLRFLog[j]/10 && xLRFLog[j]/10 < xWalkBigSide) {
                        if (yWalkSmallSide < yLRFLog[j]/10 && yLRFLog[j]/10 < yWalkBigSide) {//範囲内
                            //MessageBox(NULL, TEXT("範囲内にある点: %4d ",j), TEXT("ERROR"), MB_OK);
                             //AP
                            int aa = xLRFLog[j]/10 - xWalkSmallSide;
                            int ba = yLRFLog[j]/10 - yWalkSmallSide;

                            float katamukiA = abs((float)atan2(ba, aa));
                            //PB
                            int ab = xWalkBigSide - xLRFLog[j]/10;
                            int bb = yWalkBigSide - yLRFLog[j]/10;

                            float katamukiB = abs((float)atan2(bb, ab));
                            if (katamukiA > katamukiB - 0.044 && katamukiA < katamukiB + 0.044) {//0.25°=	0.004363323	rad
                                float katamuki = katamukiA - katamukiB;
                                sprintf_s(pBuff, "足跡と物体がX:%d,Y:%dで交差！", xLRFLog[j], yLRFLog[j]);
                                MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
                                wsprintf(out, TEXT("N:%d X: %d, Y : %d で交差 \r\n"), j , xLRFLog[j], yLRFLog[j]);
                                WriteFile(hFile, out, DWORD(lstrlen(out)), &dwWriteSize, NULL);
                            }
       
                        }
                    }
                }
            }
        }   
        xWalkAdd += xWalkLog[i];
        yWalkAdd += yWalkLog[i];
       
    }
    
    SelectObject(hdc, hOldPen);                      // hPen を戻す
    DeleteObject(hPen);
    CloseHandle(hFile);
    EndPaint(hwnd, &ps);
    return 0;
}

int get_deistance_me(HWND hwnd) {
	int    argc = 1;
	char* argv[] = { "-e" };//connectioninformationを力ずくで解決
	Connection_information information(argc, argv);
	Urg_driver urg;

	if (!urg.open(information.device_or_ip_name(),
		information.baudrate_or_port_number(),
		information.connection_type())) {
		cout << "Urg_driver::open(): "
			<< information.device_or_ip_name() << ": " << urg.what() << endl;
		//MessageBox(NULL, TEXT("serialポートが開きません"), TEXT("Test"), MB_OK);
		return 1;
	}

	urg.set_scanning_parameter(urg.deg2step(-135), urg.deg2step(135), 0);
	urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);


	vector<long> LRFData;
	long timeStamp = 0;
	//capturetimesは必要かも?
	if (!urg.get_distance(LRFData, &timeStamp)) {
		cout << "Urg_driver::get_distance(): " << urg.what() << endl;
		MessageBox(NULL, TEXT("計測不可"), TEXT("Test"), MB_OK);
		return 1;
	}
	print_data(hwnd, urg, LRFData, timeStamp);

#if defined(URG_MSC)
	getchar();
#endif
	return 0;
}

int Mytest(HWND hwnd) {
	HDC hdc;
	PAINTSTRUCT ps;

	hdc = BeginPaint(hwnd, &ps);
	for (int iCount = 0; iCount < 1024; iCount++)
		SetPixel(hdc, 10 + iCount, 10, 0x00);//(???,x,y,色)
	//EndPaint(hwnd, &ps);

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
   /*iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
        
    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }*/
    
    HDC hdc;
    PAINTSTRUCT ps;
    ps.fErase = 1;
    hdc = BeginPaint(hwnd, &ps);
    HPEN        hPen, hOldPen;
    hPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 255));       // 幅3ピクセルの青ペン
    hOldPen = (HPEN)SelectObject(hdc, hPen);          // hPen を選択
    
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    printf("bytes:%3d, %3s\n", iResult, recvbuf);
    //sprintf_s(pBuff,"bytes:%3d, %3s\n", iResult, recvbuf);
    //MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
    while(iResult > 0) {
        //Application.DoEvents()
        if (recvCount == 0) {
             memcpy(iRecvDataA, recvbuf, sizeof(recvbuf));
             //頭がSなら次へ、ちがければもう一回送るようにESPに要求。もういちどここから
             string strA(iRecvDataA, sizeof(iRecvDataA));
             if (strA[0] != 'S') {
                 iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
                 //MessageBox(NULL, TEXT("受信エラー"), TEXT("ERROR"), MB_OK);
             }
             else {
                iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
                printf("bytes:%3d, %3s\n", iResult, recvbuf);
                sprintf_s(pBuff, "bytes:%3d, %3s\n", iResult, recvbuf);
                //MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
                recvCount++;
             }
        }
        else if (recvCount == 1) {
            memcpy(iRecvDataB, recvbuf, sizeof(recvbuf));
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            printf("bytes:%3d, %3s\n", iResult, recvbuf);
            sprintf_s(pBuff, "bytes:%3d, %3s\n", iResult, recvbuf);
            //MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
            recvCount++;
        }
        else if (recvCount == 2) {
            memcpy(iRecvDataC, recvbuf, sizeof(recvbuf));
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            printf("bytes:%3d, %3s\n", iResult, recvbuf);
            sprintf_s(pBuff, "bytes:%3d, %3s\n", iResult, recvbuf);
            //MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
            recvCount++;
        }
        else if (recvCount == 3) {
            memcpy(iRecvDataD, recvbuf, sizeof(recvbuf));
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            printf("bytes:%3d, %3s\n", iResult, recvbuf);
            sprintf_s(pBuff, "bytes:%3d, %3s\n", iResult, recvbuf);
            //MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
            recvCount++;
        }
        else if (recvCount == 4) {
            memcpy(iRecvDataE, recvbuf, sizeof(recvbuf));
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            printf("bytes:%3d, %3s\n", iResult, recvbuf);
            sprintf_s(pBuff, "bytes:%3d, %3s\n", iResult, recvbuf);
            //MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
            recvCount++;
        }
        else {
            memcpy(iRecvDataF, recvbuf, sizeof(recvbuf));
            sprintf(iRecvData, "%s%s%s%s%s%s\0", iRecvDataA, iRecvDataB, iRecvDataC, iRecvDataD, iRecvDataE, iRecvDataF);
            printf("%12s\n", iRecvData);
            string str(iRecvData, sizeof(iRecvData));
            //MessageBox(hwnd, iRecvData, "Msg title", MB_OK | MB_ICONQUESTION);
            //頭はマイナス符号が入っている可能性がある,
            int walkX = int(str[2] - '0') * 1000 + int(str[3] - '0') * 100 + int(str[4] - '0') * 10 + int(str[5] - '0');
            if (str[1] == '-') walkX *= -1;
            int walkY = int(str[7] - '0') * 1000 + int(str[8] - '0') * 100 + int(str[9] - '0') * 10 + int(str[10] - '0');
            if (str[6] == '-') walkY *= -1;



            
            //配列に保存
            if (walkX < FRAMESIZE * 40 && walkX > FRAMESIZE * -40) {
                if (walkY < FRAMESIZE * 40 && walkY > FRAMESIZE * -40) {
                    xWalkLog[walkLogCount] = walkX;
                    yWalkLog[walkLogCount] = walkY;
                    walkLogCount++;
                    printf("walkX:%4d, walkY:%4d\nS", walkX, walkY);
                    sprintf_s(pBuff, "%d回分 walkX:  %d,walkY: %d ", walkLogCount, walkX, walkY);
                    MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                else {
                    //MessageBox(NULL, TEXT(" すうじがへん！"), TEXT("Test"), MB_OK);
                }
            }
            else {
                //MessageBox(NULL, TEXT(" すうじがへん！"), TEXT("Test"), MB_OK);
            }

            recvCount = 0;
        }
    }
    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}

int myABS(int n) {
    if (n < 0) n *= -1;
    return n;
}