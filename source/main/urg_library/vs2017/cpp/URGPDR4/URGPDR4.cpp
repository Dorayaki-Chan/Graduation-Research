/*
���e:
    URGplusURG8+socket4
���:2022/10/13 N.Hirota

�A�h���X:

�Ȃ񂼂�:

LRF�̂�ƍ��̂��鉺�����A�Ƃ��ESP�ƒʐM���āA�`�悷�邱�Ƃ�ڎw��
���ӎ���:txt�J���Ƃ�ANSI�ŊJ���Ă���

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
using namespace std;//���I�z��N���Xvector�̖��O��Ԏw��

namespace
{
	void print_data(HWND hwnd, const Urg_driver& urg,
		const vector<long>& data, long time_stamp) {
		HDC hdc;
		PAINTSTRUCT ps;
        
		hdc = BeginPaint(hwnd, &ps);

		HANDLE hFile;
		DWORD dwWriteSize;

		hFile = CreateFile(TEXT("�Ă���2.txt"), GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			MessageBox(NULL, TEXT("�t�@�C�����J���܂���"), NULL, MB_OK);
		}
		SetFilePointer(hFile, 0, NULL, FILE_END);

		long min_distance = urg.min_distance();
		long max_distance = urg.max_distance();
		size_t data_n = data.size();

		//�g������
		/*int frameSize = 500;
		for (int iCount = 0; iCount < frameSize; iCount++) {
			SetPixel(hdc, iCount, 0, 0x00);
			SetPixel(hdc, iCount, frameSize, 0x00);
			SetPixel(hdc, 0, iCount, 0x00);
			SetPixel(hdc, frameSize, iCount, 0x00);
		}*/


		long oldl = data[0]; //��r�p
		bool colorSet = true;
		bool continuedif = 0;
		int  objPos[40];//10��*��[xy*�I�[xy
		int  objPosNum = 0;//�z��ԍ�
		for (size_t i = 0; i < data_n; ++i) {
			long l = data[i];

			if ((l <= min_distance) || (l >= max_distance)) {
				continue;
			}

			double radian = urg.index2rad(i);
			long x = (long)(l * sin(radian));
			long y = (long)(l * cos(radian));

			//�\���p�ɕϊ�
			int xPrint, yPrint = 0;
			xPrint = (-1 * x + 10000) / 40;//TODO:���͂��������}�C�i�X������̂ł́H�H�H
			yPrint = (-1 * y + 10000) / 40;//y�������Ⴄ

			if (txtOutSituation) {
				TCHAR out[64];
				wsprintf(out, TEXT("%4d : %7d ,%7d .length : %7ld ,differ : %7ld \r\n"), i, x, y, l, l - oldl);
				WriteFile(hFile, out, DWORD(lstrlen(out)), &dwWriteSize, NULL);
			}
			/*��������Ƃ����������
			*
			* �ǂ���������������
			*
			* ��O�̕��𕨑̂̒[����
			*
			* �I�[��������i�����Ȃ��Ƃ���
			*
			* �I�[�܂Ő��������iwhile�ʂ����Ƃ�-1
			*
			*/
			HPEN        hPen, hOldPen;
			hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));       // ��3�s�N�Z���̐y��
			hOldPen = (HPEN)SelectObject(hdc, hPen);          // hPen ��I��

			if (l - oldl > 100 || l - oldl < -100) {//��������Ƃ���݂���
				if (oldl > l) {//�ǂ��������̂�

				}
				else {//�̂̂��������,�Ԃ�����

				}
			}


			if (colorSet == true) {
				if (l - oldl < -100) {
					colorSet = false;//�����ō��W��z��ɕۑ�
					if (objPosNum < 39) {
						MoveToEx(hdc, xPrint, yPrint, NULL);
					}
				}
			}

			if (colorSet == false) {
				if (l - oldl > 230) {

					colorSet = true;//�����̍��W���I�[,�z��ԍ��������߂�
					LineTo(hdc, xPrint, yPrint);
				}
				else if (i == data_n - 1) {//�I�[�����̒��̂Ƃ������`�悳��Ȃ��̂ŁAfalse�ōŌ�̓_�̎���lineto
					colorSet = true;//�����̍��W���I�[,�z��ԍ��������߂�
					LineTo(hdc, xPrint, yPrint);
				}
			}

			SelectObject(hdc, hOldPen);                      // hPen ��߂�
			DeleteObject(hPen);



			if (colorSet == true) {
				SetPixel(hdc, xPrint, yPrint, 0x00);
			}
			else {
				SetPixel(hdc, xPrint, yPrint, 0xFF);
			}
			oldl = l;
			//�̂̍��W�ۑ����Ƃ�

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
            txtOutSituation++;//1�ɂȂ�͂�
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


	while (1) { /* ���C�����[�v */
        //PM_REMOVE�̓��b�Z�[�W������ɍ폜���邱�Ƃ�\���B��{PM_REMOVE��OK
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {

			if (!GetMessage(&msg, NULL, 0, 0)) /* ���b�Z�[�W���� */
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
	char* argv[] = { "-e" };//connectioninformation��͂����ŉ���
	Connection_information information(argc, argv);
	Urg_driver urg;

	if (!urg.open(information.device_or_ip_name(),
		information.baudrate_or_port_number(),
		information.connection_type())) {
		cout << "Urg_driver::open(): "
			<< information.device_or_ip_name() << ": " << urg.what() << endl;
		MessageBox(NULL, TEXT("serial�|�[�g���J���܂���"), TEXT("Test"), MB_OK);
		return 1;
	}

	urg.set_scanning_parameter(urg.deg2step(-135), urg.deg2step(135), 0);
	urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);


	vector<long> LRFData;
	long timeStamp = 0;
	//capturetimes�͕K�v����?
	if (!urg.get_distance(LRFData, &timeStamp)) {
		cout << "Urg_driver::get_distance(): " << urg.what() << endl;
		MessageBox(NULL, TEXT("�v���s��"), TEXT("Test"), MB_OK);
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
		SetPixel(hdc, 10 + iCount, 10, 0xFF);//(???,x,y,�F)
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

    // Initialize Winsock winsock�̏�����
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
    // Resolve the server address and port �I�̃A�h���X���|�[�g�̃G���[
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

   // �����A���ɂ���Ɠ�̃t���[�Y�������[�v�ɓ˓�����
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
    hPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 255));       // ��3�s�N�Z���̐y��
    hOldPen = (HPEN)SelectObject(hdc, hPen);          // hPen ��I��
    
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    printf("bytes:%3d, %3s\n", iResult, recvbuf);
    sprintf_s(pBuff,"bytes:%3d, %3s\n", iResult, recvbuf);
    MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
    while(iResult > 0) {
        if (recvCount == 0) {
             memcpy(iRecvDataA, recvbuf, sizeof(recvbuf));
             //����S�Ȃ玟�ցA��������΂�����񑗂�悤��ESP�ɗv���B���������ǂ�������
             string strA(iRecvDataA, sizeof(iRecvDataA));
             //if (strA[0] != 'S') {
             //  MessageBox(NULL, TEXT("��M�G���["), TEXT("ERROR"), MB_OK);
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
            //���̓}�C�i�X�����������Ă���\��������,
            int walkX = int(str[2] - '0') * 1000 + int(str[3] - '0') * 100 + int(str[4] - '0') * 10 + int(str[5] - '0');
            if (str[1] == '-') walkX *= -1;
            int walkY = int(str[7] - '0') * 1000 + int(str[8] - '0') * 100 + int(str[9] - '0') * 10 + int(str[10] - '0');
            if (str[6] == '-') walkY *= -1;


            printf("walkX:%4d, walkY:%4d\nS", walkX, walkY);
            sprintf_s(pBuff, "walkX:  %d,walkY: %d ", walkX, walkY);
            MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);

            //�\���p�ɕϊ�    
            int xPrint, yPrint = 0;
            xPrint = (walkX + 2000) / 8;//TODO:���͂��������}�C�i�X������̂ł́H�H�H
            yPrint = (-1 * walkY + 2000) / 8;//y�������Ⴄ 10000 40
            int frameSize = 500;
            for (int iCount = 0; iCount < frameSize; iCount++) {
                SetPixel(hdc, iCount, 0, 0x00);
                SetPixel(hdc, iCount, frameSize, 0x00);
                SetPixel(hdc, 0, iCount, 0x00);
                SetPixel(hdc, frameSize, iCount, 0x00);
            }


            //�z��ɕۑ�
            if (xPrint < frameSize && xPrint > 0) {
                if (yPrint < frameSize && yPrint > 0) {
                    xWalkLog[logCount] = 250;//xPrint;
                    yWalkLog[logCount] = 250;//yPrint;
                    logCount++;
                }
                else {
                    MessageBox(NULL, TEXT(" ���������ւ�I"), TEXT("Test"), MB_OK);
                }
            }
            else {
                MessageBox(NULL, TEXT(" ���������ւ�I"), TEXT("Test"), MB_OK);
            }
                
            //�z��ɕۑ�����Ă镪�����`��
            for (int i = 0; i < logCount; i++) {
                //����Ȃ�n�_�Ɉړ�
                if (i == 0)
                    MoveToEx(hdc,0, 0, NULL);//250,250
                LineTo(hdc, xWalkLog[i], yWalkLog[i]);
                MoveToEx(hdc, xWalkLog[i], yWalkLog[i], NULL);
            }
            sprintf_s(pBuff, "%d�� ", logCount);
            MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
            recvCount = 0;
        }
    }
    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}