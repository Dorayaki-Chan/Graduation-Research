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
#define ESPIPADDR  "10.84.233.79"//"192.168.11.18"//192.168.208.233, 
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
#include "Urg_driver.h"
#include <C:\Users\tombow\Documents\GitHub\ARILab\urg_library\samples\cpp\Connection_information_me.h>
#include "math_utilities.h"
#include <iostream>

#include <stdio.h>
#include <tchar.h>
#include <math.h>

#define BUTTON_ID1 0

int MyPaint(HWND hwnd);
int MyLRF(HWND hwnd);
int Mytest(HWND hwnd);
int ESPSocket(HWND hwnd);
int get_deistance_me(HWND hwnd);
int myABS(int n);

char pBuff[100];
bool txtOutSituation = 0;

int xWalkLog[100];
int yWalkLog[100];
int walkLogCount = 0;

int xObjectLog[100];//始終始終始終
int yObjectLog[100];
int  objPosNum = 0;//物体何個あったか

//表示用に変換
int xPrintLRF[1081], yPrintLRF[1081];
int  LRFNum = 0;//物体何個あったか

TCHAR out[64];


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        MyPaint(hwnd);
        return 0;
	case WM_COMMAND:
        switch (LOWORD(wp)) {
        case BUTTON_ID1:
            MessageBox(hwnd, TEXT("STOP"), TEXT("BUTTON1"), MB_OK);
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
            MyPaint(hwnd);

        }
    }

}

int MyPaint(HWND hwnd) {//描画は全部ここ
    //足跡と交差してるかどうか
    int xWalkAdd= STARTX, yWalkAdd = STARTY;
        //足跡の辺に対して始点A,終端B,すべてのLRFで取得した物体Pに対して傾きを調べる、AP PBの傾きが同じなら交差しているとする
        /*
            範囲を調べる、どっちが大きいか
        　　第一象限の話になるようにする
          　アークタンジェントで角度得る
           +0.25°の-0.25の誤差は許容する
        */
    int i = 0, xWalkBigSide = 0, yWalkBigSide = 0, xWalkSmallSide = 0, yWalkSmallSide = 0;
        xWalkLog[i] = 10, yWalkLog[i] = 0;
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


        int j = 0;
        xPrintLRF[j] = 5;
            if(xWalkSmallSide < xPrintLRF[j] && xPrintLRF[j] < xWalkBigSide) {//まず歩行したの範囲内かどうか
                if (yWalkSmallSide < yPrintLRF[j] && yPrintLRF[j] < yWalkBigSide) {
                    //AP
                    int aa = xPrintLRF[j] - xWalkSmallSide;
                    int ba = yPrintLRF[j] - yWalkSmallSide;
                                        
                    float katamukiA = atan2((float)ba ,(float)aa);//引数 y には指定したい座標の y 軸の値、引数 x には指定したい座標の x 軸の値を指定します。
                    //PB
                    int ab = xWalkBigSide - xPrintLRF[j];
                    int bb = yWalkBigSide - yPrintLRF[j];

                    float katamukiB = atan2((float)bb , (float)ab);
                    if (katamukiA > katamukiB - 0.0044 && katamukiA < katamukiB + 0.0044) {//0.25°=	0.004363323	rad
                        MessageBox(NULL, TEXT("足跡と物体が交差！"), TEXT("ERROR"), MB_OK);
                    }
                    else {
                        MessageBox(NULL, TEXT("kousanashi"), TEXT("ERROR"), MB_OK);
                    }
                }
                else {
                    MessageBox(NULL, TEXT("範囲外"), TEXT("ERROR"), MB_OK);
                }
            }
            else {
                MessageBox(NULL, TEXT("範囲外"), TEXT("ERROR"), MB_OK);
            }
        
        xWalkAdd += xWalkLog[i];
        yWalkAdd += yWalkLog[i];
    return 0;
}