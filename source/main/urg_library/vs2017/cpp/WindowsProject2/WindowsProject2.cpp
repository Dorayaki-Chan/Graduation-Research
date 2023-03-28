#include <windows.h>
#include "..\..\..\samples\cpp\get_distance_me.cpp"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
char szClassName[] = "text_test";	//ウィンドウクラス
void MyPaint(HWND);  //WM_PAINTが呼ばれたときの描画動作 

int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow) {
	MSG msg;
	BOOL bRet;
	WNDCLASSEX wc;
	HWND hWnd;
	ATOM atom;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;	//プロシージャ名
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hCurInst;//インスタンス
	wc.hIcon = (HICON)LoadImage(NULL, MAKEINTRESOURCE(IDI_APPLICATION), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wc.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = (LPCSTR)szClassName;
	wc.hIconSm = (HICON)LoadImage(NULL, MAKEINTRESOURCE(IDI_APPLICATION), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);

	if ((atom = RegisterClassEx(&wc)) == 0)return FALSE;

	hWnd = CreateWindow(MAKEINTATOM(atom), "Window Title", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 200, 100, NULL, NULL, hCurInst, NULL);
	if (!hWnd)return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (bRet == -1) {
			break;
		}
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_PAINT:
	{
		// TODO: HDC を使用する描画コードをここに追加してください...
		MyPaint(hWnd);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return (DefWindowProc(hWnd, msg, wp, lp));
	}
	return 0;
}

void MyPaint(HWND hWnd) {
	HDC hdc;
	PAINTSTRUCT ps;
	Connection_information information(argc, argv);

	// Connects to the sensor
	Urg_driver urg;
	if (!urg.open(information.device_or_ip_name(),
		information.baudrate_or_port_number(),
		information.connection_type())) {
		cout << "Urg_driver::open(): "
			<< information.device_or_ip_name() << ": " << urg.what() << endl;
		return 1;
	}

	// Gets measurement data

	// Case where the measurement range (start/end steps) is defined
	urg.set_scanning_parameter(urg.deg2step(-135), urg.deg2step(+135), 0);

	enum { Capture_times = 1 };//取得回数
	urg.start_measurement(Urg_driver::Distance, Urg_driver::Infinity_times, 0);
	for (int i = 0; i < Capture_times; ++i) {
		vector<long> data;
		long time_stamp = 0;

		if (!urg.get_distance(data, &time_stamp)) {
			cout << "Urg_driver::get_distance(): " << urg.what() << endl;
			return 1;
		}
		float print_deg = 0;
		print_deg = (float)i * 0.25;
		printf(" %.2f ", print_deg);
		print_data(urg, data, time_stamp);
	}
	const char* szStr = "Hello, World!!";
	hdc = BeginPaint(hWnd, &ps);

	SetBkColor(hdc, RGB(10, 10, 10));  //文字の背景色を設定
	SetTextColor(hdc, RGB(200, 200, 200));  //文字の色を設定
	for (int i = 0; i < 10; i++) {//複数個表示するテスト
		TextOut(hdc, 10, 20 * i + 10,(LPCTSTR)szStr, (int)strlen(szStr));//文字を書く
	}
	EndPaint(hWnd, &ps);
}