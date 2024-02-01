// main.cpp
/*
https://shizenkarasuzon.hatenablog.com/?page=1584717364
*/
#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM InitApp(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);

char szClassName[] = "sample02";	//ウィンドウクラス

int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow) {
	MSG msg;
	BOOL bRet;

	if (!InitApp(hCurInst))
		return FALSE;
	if (!InitInstance(hCurInst, nCmdShow))
		return FALSE;

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

ATOM InitApp(HINSTANCE hInst) {
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;	//プロシージャ名
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;//インスタンス
	wc.hIcon = NULL; //アプリのアイコン。.icoファイルをリソースファイルに読み込みここに記入
	wc.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//Windowの背景色を指定。
	wc.lpszMenuName = NULL;	 // メニュー名。リソースファイルで設定した値を記入
	wc.lpszClassName = (LPCSTR)szClassName;
	wc.hIconSm = NULL; //アプリのアイコンの小さい版。タスクバーに表示されるもの

	return (RegisterClassEx(&wc));
}


//ウィンドウの生成
BOOL InitInstance(HINSTANCE hInst, int nCmdShow) {
	HWND hWnd;

	hWnd = CreateWindow(szClassName,
		"Sample Application 1", //Windowのタイトル
		WS_OVERLAPPEDWINDOW, //ウィンドウの種類
		CW_USEDEFAULT,	//Ｘ座標 (指定なしはCW_USEDEFAULT)
		CW_USEDEFAULT,	//Ｙ座標(指定なしはCW_USEDEFAULT)
		CW_USEDEFAULT,	//幅(指定なしはCW_USEDEFAULT)
		CW_USEDEFAULT,	//高さ(指定なしはCW_USEDEFAULT)
		NULL, //親ウィンドウのハンドル、親を作るときはNULL
		NULL, //メニューハンドル、クラスメニューを使うときはNULL
		hInst, //インスタンスハンドル
		NULL);

	if (!hWnd)
		return FALSE;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}


//ウィンドウプロシージャ
//ユーザーがWindowを操作したり、Windowが作られたりした場合、この関数が呼び出されて処理を行う。
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
	case WM_CREATE:
		//CreateWindow()でWindowを作成したときに呼び出される。ここでウィジェットを作成する。
		break;

	case WM_DESTROY:
		//ユーザーがWindow右上の×ボタンを押すとここが実行される
		PostQuitMessage(0); //終了メッセージ
		break;

	case WM_PAINT:
		//画面に図形などを描く処理を実装
		break;

	default:
		return (DefWindowProc(hWnd, msg, wp, lp));
	}
	return 0;
}