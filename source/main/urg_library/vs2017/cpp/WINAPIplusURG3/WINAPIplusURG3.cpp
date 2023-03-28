// windows_twn_draw.cpp : ���̃t�@�C���ɂ� 'main' �֐����܂܂�Ă��܂��B�v���O�������s�̊J�n�ƏI���������ōs���܂��B
//http://wisdom.sakura.ne.jp/system/winapi/win32/win22.html
//�f�X�N�g�b�v�A�v���P�[�V�����̃e���v���Ő���
//mtpaint �̂Ƃ���ɕ`��������L��
//get_distance_me �����������ڐA��
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
		SetPixel(hdc, 10 + iCount, 10, 0xFF);//(???,x,y,�F)
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

		//MessageBox(NULL, TEXT("�킟"), TEXT("Test"), MB_OK);
		// Prints the X-Y coordinates for all the measurement points
		long min_distance = urg.min_distance();
		long max_distance = urg.max_distance();
		size_t data_n = data.size();

		//�g������
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
		MyPaint(hwnd);//HWND hwnd, int argc, char* argv[] argc��argv[]�ĂȂ�
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
	TCHAR szText[8];//int����ϊ�����p�r
	//hdc = BeginPaint(hwnd, &ps);

	get_deistance_me(hwnd);
	//wsprintf(szText, TEXT("%d"),textMe);
	//TextOut(hdc, 0, 0, szText, lstrlen(szText));

	return 0;
}

int get_deistance_me(HWND hwnd)
//Connection_information�ŁAargv�Ɋi�[����Ă��镶���񂪁u-e�v�̏ꍇ�̓C�[�T�^�Z���T�[�A�u-s�v�̏ꍇ�̓V���A���^�Z���T�[�Ɣ��f����܂��B
//���������āAargc��argv���K�v�ł��B
/*
open_urg_sensor�i��urg�Aargc�Aargv�j��-1��Ԃ��ꍇ�A�Z���T�[�̐ڑ��Ɏ��s���܂����B
�C�[�T�l�b�g�ڑ��̏ꍇ�A�ŏ��̃R�}���h���C�������́u-e�v�A2�Ԗڂ̈�����IP�A�h���X�ł��B
�V���A���ڑ��̏ꍇ�A�ŏ��̃R�}���h���C�������́u-s�v�A2�Ԗڂ̈����̓V���A���|�[�g���ł��B
���Ƃ��΁A192.168.0.10�ɐڑ�����Ă���C�[�T�^�C�v�Z���T�[����̋������擾����ꍇ�́A���̎菇�ɏ]���܂��B

get_distance -e 192.168.0.10

���삵�Ȃ��ꍇ�́A�Z���T�[�̃��f���ƃ\�[�X�R�[�h�������Ă��������B
��낵�����肢���܂��B
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
		MessageBox(NULL, TEXT("serial�|�[�g���J���܂���"), TEXT("Test"), MB_OK);
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
			MessageBox(NULL, TEXT("�v���s��"), TEXT("Test"), MB_OK);
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