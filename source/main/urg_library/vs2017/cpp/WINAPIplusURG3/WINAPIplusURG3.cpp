// windows_twn_draw.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//http://wisdom.sakura.ne.jp/system/winapi/win32/win22.html
//デスクトップアプリケーションのテンプレで成功
//mtpaint のところに描画条件を記入
//get_distance_me をすこしずつ移植中
//C:\Users\tombow\Documents\GitHub\ARILab\urg_library\vs2017\cpp\x64\Debug\WINAPIplusURG3.exe -e
/*

*/
#include <windows.h>

#include "Urg_driver.h"
#include <C:\Users\tombow\Documents\GitHub\ARILab\urg_library\samples\cpp\Connection_information_me.h>
#include "math_utilities.h"
#include <iostream>


int MyPaint(HWND hwnd);
int Mytest(HWND hwnd);
int get_deistance_me(HWND hwnd);

int Mytest(HWND hwnd) {
	HDC hdc;
	PAINTSTRUCT ps;

	hdc = BeginPaint(hwnd, &ps);
	for (int iCount = 0; iCount < 1024; iCount++)
		SetPixel(hdc, 10 + iCount, 10, 0xFF);//(???,x,y,色)
	EndPaint(hwnd, &ps);

	return 0;
}

using namespace qrk;
using namespace std;



namespace
{
	void printDataMe(HWND hwnd, const Urg_driver& urg,
		const vector<long>& data, long time_stamp)
	{
		HDC hdc;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &ps);

		//MessageBox(NULL, TEXT("わぁ"), TEXT("Test"), MB_OK);
		// Prints the X-Y coordinates for all the measurement points
		long min_distance = urg.min_distance();
		long max_distance = urg.max_distance();
		size_t data_n = data.size();

		//枠を書く
		int frameSize = 500;
		for (int iCount = 0; iCount < frameSize; iCount++) {
			SetPixel(hdc, iCount, 0, 0xFF);
			SetPixel(hdc, iCount, frameSize, 0xFF);
			SetPixel(hdc, 0, iCount, 0xFF);
			SetPixel(hdc, frameSize, iCount, 0xFF);
		}


		for (size_t i = 0; i < data_n; ++i) {
			long l = data[i];
			if ((l <= min_distance) || (l >= max_distance)) {
				continue;
			}

			double radian = urg.index2rad(i);
			long x = static_cast<long>(l * cos(radian));
			long y = static_cast<long>(l * sin(radian));

			x = (x + 10000) / 40;
			y = (-1 * y + 10000) / 40;
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
		MyPaint(hwnd);//HWND hwnd, int argc, char* argv[] argcとargv[]てなんぞ
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
	TCHAR szText[8];//intから変換する用途
	//hdc = BeginPaint(hwnd, &ps);

	get_deistance_me(hwnd);
	//wsprintf(szText, TEXT("%d"),textMe);
	//TextOut(hdc, 0, 0, szText, lstrlen(szText));

	return 0;
}

int get_deistance_me(HWND hwnd)
//Connection_informationで、argvに格納されている文字列が「-e」の場合はイーサ型センサー、「-s」の場合はシリアル型センサーと判断されます。
//したがって、argcとargvが必要です。
/*
open_urg_sensor（＆urg、argc、argv）が-1を返す場合、センサーの接続に失敗しました。
イーサネット接続の場合、最初のコマンドライン引数は「-e」、2番目の引数はIPアドレスです。
シリアル接続の場合、最初のコマンドライン引数は「-s」、2番目の引数はシリアルポート名です。
たとえば、192.168.0.10に接続されているイーサタイプセンサーからの距離を取得する場合は、次の手順に従います。

get_distance -e 192.168.0.10

動作しない場合は、センサーのモデルとソースコードを教えてください。
よろしくお願いします。
*/
{
	int    argc = 1;
	char* argv[] = { "-e" };
	Connection_information information(argc, argv);
	// Connects to the sensor
	Urg_driver urg;
	if (!urg.open(information.device_or_ip_name(),
		information.baudrate_or_port_number(),
		information.connection_type())) {
		cout << "Urg_driver::open(): "
			<< information.device_or_ip_name() << ": " << urg.what() << endl;
		MessageBox(NULL, TEXT("serialポートが開きません"), TEXT("Test"), MB_OK);
		return 1;
	}


	// Gets measurement data
#if 1
	// Case where the measurement range (start/end steps) is defined
	urg.set_scanning_parameter(urg.deg2step(-135), urg.deg2step(+135), 0);
#endif
	enum { Capture_times = 1 };
	urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);
	int* test_me = 0;
	for (int i = 0; i < Capture_times; ++i) {
		vector<long> LRFData;
		long time_stamp = 0;

		if (!urg.get_distance(LRFData, &time_stamp)) {
			MessageBox(NULL, TEXT("計測不可"), TEXT("Test"), MB_OK);
			cout << "Urg_driver::get_distance(): " << urg.what() << endl;
			return 1;
		}
		//print_data(urg, LRFData, time_stamp);
		printDataMe(hwnd, urg, LRFData, time_stamp);
	}


#if defined(URG_MSC)
	getchar();
#endif
	return 0;
}