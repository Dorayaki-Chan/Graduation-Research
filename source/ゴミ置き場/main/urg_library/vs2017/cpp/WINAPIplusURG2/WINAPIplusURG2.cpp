// windows_twn_draw.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//http://wisdom.sakura.ne.jp/system/winapi/win32/win22.html
//デスクトップアプリケーションのテンプレで成功
//mtpaint のところに描画条件を記入
//get_distance_me をすこしずつ移植中

#include <windows.h>
#include <iostream>

#include "Urg_driver.h"
#include "Connection_information.h"
#include "math_utilities.h"
EXTERN_C{
    #include <stdlib.h>
    #include "urg_sensor.h"
    #include "urg_utils.h"
}


void MyPaint(HWND hwnd);
int getDistance_me(int argc, char* argv[]);

using namespace qrk;
using namespace std;

namespace
{
    void print_data(const Urg_driver& urg,
        const vector<long>& data, long time_stamp)
    {
#if 0//1にすると角度ごとの距離を表示、0ならばx-y座標の位置を表示
        // Shows only the front step
        int front_index = urg.step2index(0);
        cout << data[front_index] << " [mm], ("
            << time_stamp << " [msec])" << endl;

#else
        // Prints the X-Y coordinates for all the measurement points
        long min_distance = urg.min_distance();
        long max_distance = urg.max_distance();
        size_t data_n = data.size();
        for (size_t i = 0; i < data_n; ++i) {
            long l = data[i];
            if ((l <= min_distance) || (l >= max_distance)) {
                continue;
            }

            double radian = urg.index2rad(i);
            long x = static_cast<long>(l * cos(radian));
            long y = static_cast<long>(l * sin(radian));
            cout << "(" << x << ", " << y << ")" << endl;
        }
        cout << endl;
#endif
    }

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		MyPaint(hwnd );//HWND hwnd, int argc, char* argv[] argcとargv[]てなんぞ
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



void MyPaint(HWND hwnd) {
	HDC hdc;
	PAINTSTRUCT ps;

	int iCount;

	hdc = BeginPaint(hwnd, &ps);
	for (iCount = 0; iCount < 1024; iCount++)
		SetPixel(hdc, 10 + iCount, 10, 0xFF);//(???,x,y,色)
	EndPaint(hwnd, &ps); 
}

int getDistance_me() {
	urg_t urg;
	int ret;
	long* length_data;
	int length_data_size;

	// \~japanese "COM1" は、センサが認識されているデバイス名にする必要がある
	// \~english "COM1" is, in this case, the device name detected for the sensor
	const char connect_device[] = "COM1";
	const long connect_baudrate = 115200;

	// センサに対して接続を行う。ここでエラー
	ret = urg_open(&urg, URG_SERIAL, connect_device, connect_baudrate);
	// \todo check error code

	return 0;
}