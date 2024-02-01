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
#define ESPIPADDR  "192.168.11.18"//192.168.208.233
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

#include <stdio.h>
#include <tchar.h>

#define BUTTON_ID1 0

int MyPaint(HWND hwnd);
int Mytest(HWND hwnd);
int ESPSocket(HWND hwnd);
int get_deistance_me(HWND hwnd);

char pBuff[100];
bool txtOutSituation = 0;

int xWalkLog[100];
int yWalkLog[100];
int logCount = 0;

using namespace qrk;
using namespace std;//動的配列クラスvectorの名前空間指定

namespace
{
	void print_data(HWND hwnd, const Urg_driver& urg,
		const vector<long>& data, long time_stamp) {
		HDC hdc;
		PAINTSTRUCT ps;
        
		hdc = BeginPaint(hwnd, &ps);

		HANDLE hFile;
		DWORD dwWriteSize;

		hFile = CreateFile(TEXT("てすと2.txt"), GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			MessageBox(NULL, TEXT("ファイルが開けません"), NULL, MB_OK);
		}
		SetFilePointer(hFile, 0, NULL, FILE_END);

		long min_distance = urg.min_distance();
		long max_distance = urg.max_distance();
		size_t data_n = data.size();

		//枠を書く
		/*int frameSize = 500;
		for (int iCount = 0; iCount < frameSize; iCount++) {
			SetPixel(hdc, iCount, 0, 0x00);
			SetPixel(hdc, iCount, frameSize, 0x00);
			SetPixel(hdc, 0, iCount, 0x00);
			SetPixel(hdc, frameSize, iCount, 0x00);
		}*/


		long oldl = data[0]; //比較用
		bool colorSet = true;
		bool continuedif = 0;
		int  objPos[40];//10こ*先端xy*終端xy
		int  objPosNum = 0;//配列番号
		for (size_t i = 0; i < data_n; ++i) {
			long l = data[i];

			if ((l <= min_distance) || (l >= max_distance)) {
				continue;
			}

			double radian = urg.index2rad(i);
			long x = (long)(l * sin(radian));
			long y = (long)(l * cos(radian));

			//表示用に変換
			int xPrint, yPrint = 0;
			xPrint = (-1 * x + 10000) / 40;//TODO:実はこっちもマイナスがいるのでは？？？
			yPrint = (-1 * y + 10000) / 40;//y軸方向違う

			if (txtOutSituation) {
				TCHAR out[64];
				wsprintf(out, TEXT("%4d : %7d ,%7d .length : %7ld ,differ : %7ld \r\n"), i, x, y, l, l - oldl);
				WriteFile(hFile, out, DWORD(lstrlen(out)), &dwWriteSize, NULL);
			}
			/*差があるところを見つける
			*
			* どっちが奥か見つける
			*
			* 手前の方を物体の端から
			*
			* 終端を見つける（差がないところ
			*
			* 終端まで線を引く（whileぬけたとこ-1
			*
			*/
			HPEN        hPen, hOldPen;
			hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));       // 幅3ピクセルの青ペン
			hOldPen = (HPEN)SelectObject(hdc, hPen);          // hPen を選択

			if (l - oldl > 100 || l - oldl < -100) {//さがあるところみつける
				if (oldl > l) {//どっちが物体か

				}
				else {//昔のやつが小さい,ぶったい

				}
			}


			if (colorSet == true) {
				if (l - oldl < -100) {
					colorSet = false;//ここで座標を配列に保存
					if (objPosNum < 39) {
						MoveToEx(hdc, xPrint, yPrint, NULL);
					}
				}
			}

			if (colorSet == false) {
				if (l - oldl > 230) {

					colorSet = true;//ここの座標が終端,配列番号をすすめる
					LineTo(hdc, xPrint, yPrint);
				}
				else if (i == data_n - 1) {//終端が物体中のとき線が描画されないので、falseで最後の点の時にlineto
					colorSet = true;//ここの座標が終端,配列番号をすすめる
					LineTo(hdc, xPrint, yPrint);
				}
			}

			SelectObject(hdc, hOldPen);                      // hPen を戻す
			DeleteObject(hPen);



			if (colorSet == true) {
				SetPixel(hdc, xPrint, yPrint, 0x00);
			}
			else {
				SetPixel(hdc, xPrint, yPrint, 0xFF);
			}
			oldl = l;
			//昔の座標保存しとく

		}

		//EndPaint(hwnd, &ps);
		//WriteFile(hFile, TEXT("finish. \n"), 20, &dwWriteSize, NULL);
		//CloseHandle(hFile);
		//txtOutSituation = 0;
	}
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        //InvalidateRect(hwnd, NULL, TRUE);

        MyPaint(hwnd);
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

int MyPaint(HWND hwnd) {
	if (get_deistance_me(hwnd))
		return 1;
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
		MessageBox(NULL, TEXT("serialポートが開きません"), TEXT("Test"), MB_OK);
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
		SetPixel(hdc, 10 + iCount, 10, 0xFF);//(???,x,y,色)
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
    sprintf_s(pBuff,"bytes:%3d, %3s\n", iResult, recvbuf);
    MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
    while(iResult > 0) {
        if (recvCount == 0) {
             memcpy(iRecvDataA, recvbuf, sizeof(recvbuf));
             //頭がSなら次へ、ちがければもう一回送るようにESPに要求。もういちどここから
             string strA(iRecvDataA, sizeof(iRecvDataA));
             //if (strA[0] != 'S') {
             //  MessageBox(NULL, TEXT("受信エラー"), TEXT("ERROR"), MB_OK);
             //}
             //else {
                iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
                printf("bytes:%3d, %3s\n", iResult, recvbuf);
                sprintf_s(pBuff, "bytes:%3d, %3s\n", iResult, recvbuf);
                MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
                recvCount++;
             //}
        }
        else if (recvCount == 1) {
            memcpy(iRecvDataB, recvbuf, sizeof(recvbuf));
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            printf("bytes:%3d, %3s\n", iResult, recvbuf);
            sprintf_s(pBuff, "bytes:%3d, %3s\n", iResult, recvbuf);
            MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
            recvCount++;
        }
        else if (recvCount == 2) {
            memcpy(iRecvDataC, recvbuf, sizeof(recvbuf));
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            printf("bytes:%3d, %3s\n", iResult, recvbuf);
            sprintf_s(pBuff, "bytes:%3d, %3s\n", iResult, recvbuf);
            MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
            recvCount++;
        }
        else if (recvCount == 3) {
            memcpy(iRecvDataD, recvbuf, sizeof(recvbuf));
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            printf("bytes:%3d, %3s\n", iResult, recvbuf);
            sprintf_s(pBuff, "bytes:%3d, %3s\n", iResult, recvbuf);
            MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
            recvCount++;
        }
        else if (recvCount == 4) {
            memcpy(iRecvDataE, recvbuf, sizeof(recvbuf));
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            printf("bytes:%3d, %3s\n", iResult, recvbuf);
            sprintf_s(pBuff, "bytes:%3d, %3s\n", iResult, recvbuf);
            MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
            recvCount++;
        }
        else {
            memcpy(iRecvDataF, recvbuf, sizeof(recvbuf));
            sprintf(iRecvData, "%s%s%s%s%s%s\0", iRecvDataA, iRecvDataB, iRecvDataC, iRecvDataD, iRecvDataE, iRecvDataF);
            printf("%12s\n", iRecvData);
            string str(iRecvData, sizeof(iRecvData));
            MessageBox(hwnd, iRecvData, "Msg title", MB_OK | MB_ICONQUESTION);
            //頭はマイナス符号が入っている可能性がある,
            int walkX = int(str[2] - '0') * 1000 + int(str[3] - '0') * 100 + int(str[4] - '0') * 10 + int(str[5] - '0');
            if (str[1] == '-') walkX *= -1;
            int walkY = int(str[7] - '0') * 1000 + int(str[8] - '0') * 100 + int(str[9] - '0') * 10 + int(str[10] - '0');
            if (str[6] == '-') walkY *= -1;


            printf("walkX:%4d, walkY:%4d\nS", walkX, walkY);
            sprintf_s(pBuff, "walkX:  %d,walkY: %d ", walkX, walkY);
            MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);

            //表示用に変換    
            int xPrint, yPrint = 0;
            xPrint = (walkX + 2000) / 8;//TODO:実はこっちもマイナスがいるのでは？？？
            yPrint = (-1 * walkY + 2000) / 8;//y軸方向違う 10000 40
            int frameSize = 500;
            for (int iCount = 0; iCount < frameSize; iCount++) {
                SetPixel(hdc, iCount, 0, 0x00);
                SetPixel(hdc, iCount, frameSize, 0x00);
                SetPixel(hdc, 0, iCount, 0x00);
                SetPixel(hdc, frameSize, iCount, 0x00);
            }


            //配列に保存
            if (xPrint < frameSize && xPrint > 0) {
                if (yPrint < frameSize && yPrint > 0) {
                    xWalkLog[logCount] = 250;//xPrint;
                    yWalkLog[logCount] = 250;//yPrint;
                    logCount++;
                }
                else {
                    MessageBox(NULL, TEXT(" すうじがへん！"), TEXT("Test"), MB_OK);
                }
            }
            else {
                MessageBox(NULL, TEXT(" すうじがへん！"), TEXT("Test"), MB_OK);
            }
                
            //配列に保存されてる分だけ描画
            for (int i = 0; i < logCount; i++) {
                //初回なら始点に移動
                if (i == 0)
                    MoveToEx(hdc,0, 0, NULL);//250,250
                LineTo(hdc, xWalkLog[i], yWalkLog[i]);
                MoveToEx(hdc, xWalkLog[i], yWalkLog[i], NULL);
            }
            sprintf_s(pBuff, "%d回分 ", logCount);
            MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
            recvCount = 0;
        }
    }
    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}