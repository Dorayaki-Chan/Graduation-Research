/*
内容:
	WINAPIでLRFのデータを表示
情報:2021/09/24 N.Hirota

アドレス:C:\Users\tombow\Documents\GitHub\ARILab\urg_library\vs2017\cpp\x64\Debug\WINAPIplusURG5.exe
なんぞや:URG4のリアルタイム更新版
TODO:
	stopボタン
	データロガー

*/
#include <windows.h>

#include "Urg_driver.h"
#include <C:\Users\tombow\Documents\GitHub\ARILab\urg_library\samples\cpp\Connection_information_me.h>
#include "math_utilities.h"
#include <iostream>
#include <stdio.h>
#include <tchar.h>

#define BUTTON_ID1 0

int MyPaint(HWND hwnd);
int get_deistance_me(HWND hwnd);
int Mytest(HWND hwnd);
int Mytext();


using namespace qrk;
using namespace std;

namespace
{
	void print_data(HWND hwnd, const Urg_driver& urg,
		const vector<long>& data, long time_stamp) {
		HDC hdc;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &ps);


		long min_distance = urg.min_distance();
		long max_distance = urg.max_distance();
		size_t data_n = data.size();

		//枠を書く
		int frameSize = 500;
		for (int iCount = 0; iCount < frameSize; iCount++) {
			SetPixel(hdc, iCount, 0, 0x00);
			SetPixel(hdc, iCount, frameSize, 0x00);
			SetPixel(hdc, 0, iCount, 0x00);
			SetPixel(hdc, frameSize, iCount, 0x00);
		}


		for (size_t i = 0; i < data_n; ++i) {
			long l = data[i];
			if ((l <= min_distance) || (l >= max_distance)) {
				continue;
			}

			double radian = urg.index2rad(i);
			long x = (long)(l * sin(radian));
			long y = (long)(l * cos(radian));

			//表示用に変換
			x = (-1 * x + 10000) / 40;//TODO:実はこっちもマイナスがいるのでは？？？
			y = (-1 * y + 10000) / 40;//y軸方向違う
			int xPrint, yPrint = 0;
			xPrint = (int)x;
			yPrint = (int)y;

			SetPixel(hdc, xPrint, yPrint, 0xFF);
		}
		EndPaint(hwnd, &ps);
	}
}




LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		InvalidateRect(hwnd, NULL, TRUE);
		MyPaint(hwnd);//HWND hwnd, int argc, char* argv[] argcとargv[]てなんぞ
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case BUTTON_ID1:
			//InvalidateRect(hwnd, NULL, TRUE);
			MessageBox(hwnd, TEXT("STOP"), TEXT("BUTTON1"), MB_OK);
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

	Mytext();

	while (1) { /* メインループ */

		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {

			if (!GetMessage(&msg, NULL, 0, 0)) /* メッセージ処理 */
				return msg.wParam;

			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else {
			//MessageBox(NULL, TEXT("serialポートが開きません"), TEXT("Test"), MB_OK);
			InvalidateRect(hwnd, NULL, TRUE);
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

int Mytext() {
	int param2;
	HANDLE hFile;
	DWORD dwWriteSize;
	PSTR lpCmdLine = "test";


	hFile = CreateFile((LPCWSTR)lpCmdLine, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, TEXT("ファイルが開けません"), NULL, MB_OK);
		return 1;
	}
	SetFilePointer(hFile, 0, NULL, FILE_END);

	WriteFile(hFile, lpCmdLine, lstrlen((LPCWSTR)lpCmdLine), &dwWriteSize, NULL);
	CloseHandle(hFile);

	return 0;
}