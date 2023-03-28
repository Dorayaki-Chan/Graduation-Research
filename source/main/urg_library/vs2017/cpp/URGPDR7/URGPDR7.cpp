/*
内容:
    URGplusURG8+socket4
情報:2022/10/13 N.Hirota

アドレス:

なんぞや:
URGPDR5のPDRにはあらかじめダミーデータを入れてあるver

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
#include <C:\Users\tombow\Documents\GitHub\ARILab\urg_library\samples\cpp\Connection_information_me.h>
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
#define ESPIPADDR  "10.84.233.91"//192.168.11.18"//192.168.208.233, 
#define BUTTON_ID1 0
#define BUTTON_ID2 1
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



int MyPaint(HWND hwnd);
int MyLRF(HWND hwnd);
int Mytest(HWND hwnd);
//int ESPSocket(HWND hwnd);
int get_deistance_me(HWND hwnd);
int myABS(int n);
int myTXTout();

char pBuff[100];
bool txtOutSituation = 0;

int xWalkLog[100]={0,0,0,0,0,0,0,0,0,0,0,2 , 4, 0, 5, 3,-12,-42,-51,-1,-12,-30,-30,-18,-44,-41,-38,-25,-68,-68,-63,-60,-64,-32,-63,-62,-64,-65,-64,-32,-96,-32,-56,-17,14,21,39,24,27,27,31,22,61,68,67,68,68,34,66,63,34,64};
int yWalkLog[100]={0,0,0,0,0,0,0,0,0,0,0,68,68,34,68,68, 66, 46,  35,69,63,63,30,53,55,58,24,10,-16,-30,-33,-29,-14,-28,-29,-27,-28,-26,-14,-40,-14,-40,-67,-68,-67,-97,-65,-64,-63,-62,-27,-30,11,17,14,14,5,20,28,11,25, };
int walkLogCount = 62;// 62;

int xObjectLog[100];//始終始終始終
int yObjectLog[100];
int  objPosNum = 0;//物体何個あったか

//表示用に変換
int xLRFLog[1081] = { 1152,1176,1226,1248,1266,1276,1288,1294,1298,1303,1310,1324,1340,1344,1351,1359,1360,1368,1360,1358,1361,1360,1358,1353,1356,1356,1354,1353,1353,1351,1350,1350,1348,1345,1344,1334,1333,1330,1335,1338,1337,1335,1326,1328,1331,1328,1328,1330,1327,1324,1328,1326,1322,1321,1320,1323,1319,1321,1326,1329,1322,1321,1312,1313,1316,1318,1317,1316,1317,1315,1313,1320,1310,1310,1312,1313,1308,1300,1297,1299,1302,1305,1300,1301,1303,1304,1304,1304,1297,1299,1299,1297,1301,1304,1303,1295,1295,1297,1301,1301,1299,1294,1295,1294,1292,1291,1287,1289,1290,1289,1288,1283,1294,1291,1288,1286,1284,1285,1281,1284,1282,1287,1284,1284,1285,1287,1288,1284,1285,1289,1285,1280,1284,1289,1289,1290,1286,1287,1281,1279,1276,1277,1280,1284,1284,1280,1281,1279,1276,1280,1277,1278,1278,1276,1274,1275,1274,1274,1281,1281,1272,1276,1274,1275,1278,1283,1269,1269,1270,1276,1275,1282,1274,1275,1278,1279,1278,1275,1276,1280,1284,1285,1278,1274,1275,1281,1280,1279,1271,1273,1272,1271,1270,1268,1268,1274,1277,1271,1272,1270,1270,1276,1270,1266,1269,1271,1268,1271,1272,1268,1264,1263,1251,1263,1261,1264,1264,1263,1264,1269,1269,1273,1273,1225,1205,1207,1174,1168,1141,1140,1148,1158,1160,1163,1166,1169,1158,1157,1157,1158,1163,1165,1162,1163,1161,1158,1157,1157,1158,1160,1161,1159,1158,1156,1157,1155,1158,1155,1153,1152,1151,1153,1152,1149,1150,1150,1153,1154,1143,1137,1150,1149,1150,1152,1151,1151,1148,1146,1147,1145,1142,1143,1145,1142,1140,1141,1148,1149,1146,1145,1139,1134,1135,1140,1142,1136,1132,1135,1135,1132,1139,1141,1144,1142,1139,1138,1139,1130,1127,1128,1133,1133,1130,1128,1127,1127,1125,1125,1127,1125,1126,1122,1125,1124,1124,1126,1128,1126,1120,1116,1120,1118,1122,1125,1123,1123,1119,1121,1122,1114,1117,1118,1125,1118,1113,1123,1124,1124,1122,1121,1120,1118,1115,1115,1111,1111,1114,1111,1111,1107,1108,1110,1111,1107,1105,1109,1109,1104,1106,1104,1107,1104,1104,1102,1101,1093,1091,1099,1137,1189,1198,1193,1192,1195,1194,1193,1187,1186,1190,1189,1186,1190,1187,1187,1185,1182,1184,1181,1185,1184,1182,1183,1179,1177,1177,1173,1178,1176,1168,1172,1168,1169,1164,1163,1166,1165,1163,1165,1163,1164,1158,1154,1158,1158,1158,1158,1155,1149,1148,1145,1144,1147,1152,1145,1145,1141,1143,1141,1141,1138,1136,1134,1133,1130,1128,1126,1123,1124,1122,1126,1122,1112,1107,1092,1080,1067,1053,1041,1028,961,953,946,921,921,936,937,928,931,927,914,904,893,880,838,829,790,791,779,773,735,698,686,674,664,649,623,609,601,589,578,567,555,562,591,615,604,591,573,564,551,539,523,522,553,558,542,526,518,505,491,488,482,476,473,463,454,448,447,436,418,398,380,361,341,302,286,266,245,228,207,186,167,150,131,113,93,74,56,37,18,0,-18,-37,-56,-73,-93,-352,-410,-468,-526,-583,-640,-702,-760,-819,-878,-938,-999,-1059,-1119,-364,0,-1400,-1463,-1528,-1591,-1654,-1719,-1780,-1842,-1903,-1956,-2019,0,-1701,-1751,-1797,-1846,-1895,-1948,0,0,0,0,0,-1452,-1484,-1491,-1454,-1446,-1436,-1418,-1402,-1390,-1382,-1367,-1363,-1351,-1331,-1333,-1315,-1302,-1292,-1257,-1276,-1289,-1311,-1331,-1356,-1378,-1400,-1418,-1438,-1458,-1478,-1507,-1531,-1554,-1575,-1596,-1616,-1641,-1664,-1684,-1706,-1727,-1750,-1772,-1795,-1820,-1841,-1863,-1884,-1906,-1925,-1950,-1970,-1994,-2017,-2044,-2070,-2094,-2115,-2134,-2157,-2179,-2201,-2221,-2244,-2261,-2288,-2316,-2363,-2408,-2482,-2929,-2964,-2993,-3017,-3048,-3088,-3115,-3145,-3174,-3200,-3235,-3258,-3291,-1096,-1107,-1090,-1093,-1099,-1111,-1109,-1100,-1101,-1102,-1099,-1097,-1097,-1102,-1095,-1092,-1090,-1090,-1089,-1088,-1088,-1089,-1085,-1090,-1090,-1084,-1084,-1085,-1088,-1083,-1080,-1083,-1079,-1080,-1077,-1071,-1073,-1073,-1066,-1066,-1068,-1069,-1065,-1067,-1068,-1062,-1062,-1062,-1062,-1062,-1058,-1059,-1062,-1061,-1061,-1065,-1056,-1054,-1051,-1050,-1053,-1053,-1052,-1051,-1050,-1049,-1048,-1049,-1052,-1046,-1044,-1043,-1047,-1047,-1046,-1043,-1041,-1041,-1043,-1042,-1044,-1045,-1044,-1043,-1043,-1044,-1044,-1038,-1041,-1038,-1040,-1042,-1045,-1037,-1038,-1036,-1032,-1031,-1032,-1028,-1034,-1032,-1039,-1034,-1033,-1032,-1032,-1025,-1028,-1026,-1023,-1026,-1024,-1021,-1025,-1032,-1036,-1025,-1034,-1025,-1025,-1025,-1020,-1022,-1024,-1025,-1025,-1025,-1030,-1026,-1024,-1026,-1019,-1018,-1020,-1021,-1018,-1017,-1018,-1018,-1016,-1019,-1021,-1019,-1018,-1010,-1013,-1013,-1014,-1017,-1010,-1001,-1005,-1007,-1006,-1012,-1012,-1013,-1010,-1012,-1008,-1004,-1004,-1003,-1001,-1004,-1004,-1010,-1006,-1000,-999,-1010,-1013,-1001,-997,-1001,-995,-1000,-991,-995,-990,-988,-989,-969,-969,-975,-979,-981,-988,-986,-980,-982,-979,-977,-980,-981,-981,-983,-985,-981,-977,-978,-979,-982,-983,-982,-976,-969,-973,-977,-973,-972,-971,-977,-974,-973,-971,-971,-973,-975,-977,-974,-965,-965,-974,-975,-975,-975,-968,-970,-967,-973,-973,-974,-975,-977,-975,-976,-980,-983,-980,-978,-973,-971,-973,-974,-976,-975,-976,-970,-968,-970,-974,-975,-975,-974,-970,-971,-970,-972,-969,-968,-967,-967,-964,-968,-966,-967,-968,-968,-959,-957,-961,-964,-963,-962,-962,-962,-960,-955,-954,-954,-959,-958,-959,-957,-959,-963,-962,-949,-947,-961,-961,-958,-957,-956,-956,-957,-955,-954,-953,-952,-955,-953,-956,-959,-952,-944,-941,-940,-946,-943,-940,-945,-945,-943,-944,-945,-947,-949,-944,-942,-945,-946,-947,-945,-946,-945,-945,-942,-939,-939,-942,-940,-941,-939,-940,-937,-938,-936,-934,-932,-940,-937,-938,-936,-945,-937,-934,-936,-935,-932,-939,-945,-947,-947,-946,-945,-945,-945,-946,-945,-944,-941,-894,-878,-876,-887,-940,-940,-946,-937,-934,-948,-948,-943,-940,-944,-941,-939,-936,-943,-938,-935,-935,-936,-935,-933,-931,-930,-929,-926,-925,-930,-933,-932,-925,-926,-927,-926,-925,-923,-920,-919,-918,-921,-917,-916,-917,-919,-920,-914,-912 };
int yLRFLog[1081] = { -1152,-1166,-1205,-1216,-1222,-1221,-1223,-1217,-1210,-1204,-1201,-1202,-1207,-1199,-1195,-1192,-1182,-1178,-1161,-1150,-1142,-1131,-1119,-1105,-1098,-1088,-1077,-1066,-1057,-1046,-1036,-1026,-1016,-1004,-994,-978,-968,-957,-952,-945,-936,-926,-911,-904,-898,-887,-879,-872,-862,-852,-846,-836,-826,-817,-809,-803,-792,-786,-781,-775,-763,-755,-742,-735,-729,-723,-715,-707,-700,-691,-683,-679,-667,-660,-654,-647,-638,-627,-619,-612,-607,-602,-592,-586,-580,-573,-567,-560,-550,-544,-538,-530,-525,-520,-513,-503,-497,-491,-486,-480,-473,-464,-458,-452,-445,-438,-430,-425,-419,-412,-406,-398,-395,-388,-381,-374,-368,-362,-355,-350,-343,-338,-332,-326,-320,-315,-309,-302,-296,-291,-285,-277,-272,-268,-262,-256,-250,-244,-237,-231,-225,-219,-214,-209,-203,-197,-191,-185,-179,-174,-168,-162,-156,-151,-145,-139,-134,-128,-123,-117,-111,-106,-100,-94,-89,-84,-77,-72,-66,-61,-55,-50,-44,-38,-33,-27,-22,-16,-11,-5,0,5,11,16,22,27,33,39,44,50,55,61,66,72,77,83,89,94,100,105,111,117,122,127,133,139,144,150,156,161,166,171,175,183,188,194,200,205,211,218,223,230,235,232,234,240,239,242,242,247,254,262,267,273,280,286,288,293,299,305,311,317,322,328,332,337,342,348,354,360,366,371,376,381,387,392,398,403,408,413,418,425,430,435,441,447,454,460,461,465,476,481,488,495,500,506,511,516,522,527,532,539,546,550,556,563,572,579,584,589,593,596,603,612,620,623,627,636,642,647,658,665,674,679,684,691,698,699,704,711,722,729,734,740,746,753,758,766,774,780,788,793,803,809,817,826,835,841,844,849,859,866,876,887,893,901,906,916,925,926,937,947,961,963,968,985,995,1003,1010,1018,1026,1033,1040,1049,1055,1064,1076,1082,1092,1098,1108,1120,1131,1136,1145,1159,1169,1174,1186,1194,1209,1216,1226,1234,1244,1246,1255,1276,1331,1405,1428,1434,1446,1463,1475,1487,1493,1505,1523,1536,1546,1565,1575,1589,1602,1612,1630,1641,1661,1675,1688,1705,1715,1729,1745,1756,1780,1794,1798,1822,1834,1852,1863,1880,1903,1920,1937,1958,1975,1996,2007,2019,2047,2068,2090,2112,2127,2138,2160,2177,2198,2228,2262,2273,2297,2315,2344,2367,2393,2414,2437,2461,2487,2510,2535,2559,2584,2616,2643,2685,2709,2719,2741,2738,2741,2745,2745,2750,2751,2607,2620,2635,2603,2638,2720,2763,2775,2825,2853,2857,2869,2876,2880,2787,2801,2713,2758,2763,2789,2698,2607,2606,2607,2614,2606,2547,2540,2553,2552,2556,2557,2560,2645,2846,3026,3037,3044,3018,3045,3047,3060,3045,3122,3398,3527,3523,3519,3573,3593,3609,3712,3793,3881,3999,4063,4150,4264,4446,4537,4558,4559,4577,4594,4601,4330,4365,4359,4324,4361,4329,4264,4264,4308,4297,4326,4304,4293,4316,4307,4267,4270,4265,4240,4288,4239,4269,13478,13447,13420,13406,13357,13336,13399,13388,13400,13404,13427,13443,13463,13469,4171,0,14541,14535,14539,14533,14522,14526,14500,14481,14461,14375,14366,0,11386,11379,11349,11336,11326,11340,0,0,0,0,0,7301,7295,7172,6845,6662,6479,6267,6074,5906,5758,5587,5469,5322,5148,5064,4907,4778,4662,4458,4450,4423,4427,4423,4437,4439,4441,4430,4426,4421,4420,4440,4447,4452,4449,4447,4441,4449,4452,4445,4445,4442,4443,4443,4443,4449,4446,4443,4439,4437,4428,4431,4426,4426,4427,4435,4440,4440,4435,4425,4422,4419,4415,4406,4404,4391,4396,4402,4445,4482,4571,5340,5347,5344,5333,5334,5349,5342,5340,5336,5327,5332,5317,5319,1755,1754,1712,1699,1693,1695,1676,1647,1632,1619,1599,1581,1567,1560,1536,1517,1501,1487,1472,1457,1443,1432,1414,1408,1396,1375,1363,1352,1343,1325,1310,1302,1286,1276,1261,1243,1235,1223,1205,1194,1186,1176,1162,1155,1145,1128,1119,1109,1100,1090,1077,1068,1062,1052,1042,1037,1020,1009,997,988,982,973,964,955,945,936,927,920,914,901,892,883,879,870,862,852,843,836,830,821,815,809,801,793,785,780,772,761,756,747,742,737,731,719,713,705,696,689,683,674,671,664,662,652,645,638,632,622,618,610,602,598,591,584,580,578,574,562,561,550,545,539,531,526,522,516,511,505,502,495,488,484,475,469,464,460,453,447,443,437,431,427,423,417,411,403,399,393,389,385,377,369,365,361,356,353,348,343,338,333,327,321,316,311,306,302,297,294,288,282,277,275,271,263,257,254,248,244,238,234,228,223,219,210,206,202,199,195,192,187,181,177,172,167,164,159,155,151,147,142,137,133,128,124,120,116,111,106,102,98,93,89,84,81,76,72,67,63,59,55,51,46,42,37,34,29,25,21,16,12,8,4,0,-4,-8,-12,-17,-21,-25,-30,-34,-38,-42,-46,-51,-55,-59,-63,-68,-72,-76,-80,-85,-89,-93,-98,-102,-106,-110,-115,-119,-123,-127,-131,-135,-140,-144,-148,-153,-157,-160,-164,-169,-174,-178,-182,-187,-191,-195,-198,-202,-207,-212,-216,-221,-225,-230,-235,-239,-241,-245,-253,-257,-261,-265,-269,-274,-278,-283,-287,-291,-295,-301,-305,-310,-316,-318,-320,-324,-328,-335,-338,-342,-348,-353,-357,-362,-367,-373,-378,-381,-385,-391,-396,-402,-406,-411,-416,-420,-424,-427,-432,-439,-443,-449,-453,-458,-462,-468,-472,-476,-480,-489,-493,-499,-503,-513,-514,-518,-524,-529,-532,-542,-551,-558,-563,-568,-573,-579,-585,-591,-596,-601,-605,-581,-576,-580,-592,-634,-640,-650,-650,-654,-670,-676,-678,-683,-692,-696,-701,-705,-717,-720,-724,-730,-737,-743,-749,-754,-760,-765,-770,-776,-787,-797,-803,-804,-812,-820,-827,-833,-839,-843,-849,-856,-866,-870,-877,-886,-895,-904,-906,-912 };
int xPrintLRF[1081], yPrintLRF[1081];
int  LRFNum = 1081;//物体何個あったか

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

            xLRFLog[i] = x*-1;
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
        //MyLRF(hwnd);
        MyPaint(hwnd);
        return 0;
	case WM_COMMAND:
        switch (LOWORD(wp)) {
        case BUTTON_ID1:
            MessageBox(NULL, TEXT("LOGOUT!!"), NULL, MB_OK);
            myTXTout();
            txtOutSituation=true;//1になるはず
        case BUTTON_ID2:
            MessageBox(NULL, TEXT("RENEW!!"), NULL, MB_OK);
           // MyLRF(hwnd);
            MyPaint(hwnd);
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
    CreateWindow(
        TEXT("BUTTON"), TEXT("RENEW"),
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        550, 100, 100, 50,
        hwnd, (HMENU)BUTTON_ID2, hInstance, NULL
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
           // MyLRF(hwnd);
           // MyPaint(hwnd);
		}
	}
}

int MyLRF(HWND hwnd) {
	if (get_deistance_me(hwnd))
		return 1;
	return 0;
}

int myTXTout() {
    /*HANDLE hFile;
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
    txtOutSituation = 0;*/
    return 0;
}



int MyPaint(HWND hwnd) {//描画は全部ここ
    HDC hdc;
    PAINTSTRUCT ps;

    hdc = BeginPaint(hwnd, &ps);
    
    HANDLE hFile;
    DWORD dwWriteSize;

    hFile = CreateFile(TEXT("dodemoi.txt"), GENERIC_READ | GENERIC_WRITE, 0, NULL,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        MessageBox(NULL, TEXT("ファイルが開けません"), NULL, MB_OK);
    }
    MessageBox(NULL, TEXT("ファイル"), NULL, MB_OK);
    SetFilePointer(hFile, 0, NULL, FILE_END);
    //枠
    for (int iCount = 0; iCount < FRAMESIZE; iCount++) {
        SetPixel(hdc, iCount, 0, 0x00);
        SetPixel(hdc, iCount, FRAMESIZE, 0x00);
        SetPixel(hdc, 0, iCount, 0x00);
        SetPixel(hdc, FRAMESIZE, iCount, 0x00);
    }
    bool colorFlag = 0;
    objPosNum = 0;
    for (int i = 0; i < LRFNum; i++) {//LRF
        xPrintLRF[i] = ( xLRFLog[i] + 10000) / 40;//TODO:実はこっちもマイナスがいるのでは？？？
        yPrintLRF[i] = (-1 * yLRFLog[i] + 10000) / 40;//y軸方向違う
        if (i != 0) {
            int LRFrange = sqrt(xPrintLRF[i] ^ 2 + yPrintLRF[i] ^ 2);
            int LRFrangeN = sqrt(xPrintLRF[i - 1] ^ 2 + yPrintLRF[i - 1] ^ 2);
            if (abs(LRFrange - LRFrangeN) > 3000) {
                xObjectLog[objPosNum] = xPrintLRF[i];
                yObjectLog[objPosNum] = yPrintLRF[i];
                objPosNum++;
            }
        }

        SetPixel(hdc, xPrintLRF[i], yPrintLRF[i], 0x00);
  
        
        wsprintf(out, TEXT("LRF N: %d X: %d, Y : %d \r\n"), i, xLRFLog[i], yLRFLog[i]);
        WriteFile(hFile, out, DWORD(lstrlen(out)), &dwWriteSize, NULL);
    }

    //物体
    HPEN        hPen, hOldPen;
    hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));       // 幅3ピクセルの青ペン
    hOldPen = (HPEN)SelectObject(hdc, hPen);          // hPen を選択
  
    for (int i = 0; i < objPosNum; i++) {
        //初回なら始点に移動
        if (i == 0)
            MoveToEx(hdc, STARTX, STARTY, NULL);
        //表示用に変換    

        LineTo(hdc, xObjectLog[i], xObjectLog[i] + STARTY);

    }

    ////歩行
    //HPEN        hPen, hOldPen;
    //hPen = CreatePen(PS_SOLID, 3, RGB(0, 255, 255));       // 幅3ピクセルの青ペン
    //hOldPen = (HPEN)SelectObject(hdc, hPen);          // hPen を選択
    //
    //int xPrintAdd = 0, yPrintAdd = 0;
    //for (int i = 0; i < walkLogCount; i++) {
    //    //初回なら始点に移動
    //    if (i == 0)
    //        MoveToEx(hdc, STARTX, STARTY, NULL);
    //    //表示用に変換    
    //    int xPrint, yPrint = 0;
    //    xPrint =      xWalkLog[i] / 4;//TODO:実はこっちもマイナスがいるのでは？？？
    //    yPrint = -1 * yWalkLog[i] / 4;//y軸方向違う 10000 40
    //    xPrintAdd += xPrint;
    //    yPrintAdd += yPrint;
    //    LineTo(hdc, xPrintAdd+ STARTX, yPrintAdd + STARTY);


    //    wsprintf(out, TEXT("Walk N:%d X:%d ,Y:%d \r\n"), i, xWalkLog[i], yWalkLog[i]);
    //    WriteFile(hFile, out, DWORD(lstrlen(out)), &dwWriteSize, NULL);

    //}

    ////足跡と交差してるかどうか
    ///*int xWalkAdd = 0, yWalkAdd = 0;
    //for (int i = 0; i < walkLogCount; i++) {
    //    //足跡の辺に対して始点A,終端B,すべてのLRFで取得した物体Pに対して傾きを調べる、AP PBの傾きが同じなら交差しているとする
    //    /*
    //        範囲を調べる、どっちが大きいか
    //    　　第一象限の話になるようにする
    //      　アークタンジェントで角度得る
    //       +0.25°の-0.25の誤差は許容する
    //    */
    //    if (xWalkLog[i] == 0 && yWalkLog[i] == 0) {
    //        continue;//動いてないなら交差しない
    //    }
    //    else {
    //        int xWalkBigSide = 0, yWalkBigSide = 0, xWalkSmallSide = 0, yWalkSmallSide = 0;

    //        if (xWalkLog[i] > 0) {
    //            xWalkBigSide = xWalkAdd + xWalkLog[i];
    //            xWalkSmallSide = xWalkAdd;
    //        }
    //        else {
    //            xWalkBigSide = xWalkAdd;
    //            xWalkSmallSide = xWalkAdd + xWalkLog[i];
    //        }

    //        if (yWalkLog[i] > 0) {
    //            yWalkBigSide = yWalkAdd + yWalkLog[i];
    //            yWalkSmallSide = yWalkAdd;
    //        }
    //        else {
    //            yWalkBigSide = yWalkAdd;
    //            yWalkSmallSide = yWalkAdd + yWalkLog[i];
    //        }



    //        for (int j = 0; j < LRFNum; j++) {//LRF
    //            if (xLRFLog[j] == 0 && yLRFLog[j] == 0) {
    //                continue;//エラー処理の00をはじく
    //            }
    //            else {
    //                /// <summary>
    //                /// LRFはmm単位、ESPPのPDRはcm単位なのに注意
    //                /// </summary>
    //                
    //                if (xWalkSmallSide < xLRFLog[j]/10 && xLRFLog[j]/10 < xWalkBigSide) {
    //                    if (yWalkSmallSide < yLRFLog[j]/10 && yLRFLog[j]/10 < yWalkBigSide) {//範囲内
    //                        //MessageBox(NULL, TEXT("範囲内にある点: %4d ",j), TEXT("ERROR"), MB_OK);
    //                         //AP
    //                        int aa = xLRFLog[j]/10 - xWalkSmallSide;
    //                        int ba = yLRFLog[j]/10 - yWalkSmallSide;

    //                        float katamukiA = abs((float)atan2(ba, aa));
    //                        //PB
    //                        int ab = xWalkBigSide - xLRFLog[j]/10;
    //                        int bb = yWalkBigSide - yLRFLog[j]/10;

    //                        float katamukiB = abs((float)atan2(bb, ab));
    //                        if (katamukiA > katamukiB - 0.044 && katamukiA < katamukiB + 0.044) {//0.25°=	0.004363323	rad
    //                            float katamuki = katamukiA - katamukiB;
    //                            sprintf_s(pBuff, "足跡と物体がN:%d,X:%d,Y:%dで交差！",j, xLRFLog[j], yLRFLog[j]);
    //                            MessageBox(hwnd, pBuff, "Msg title", MB_OK | MB_ICONQUESTION);
    //                            wsprintf(out, TEXT("N:%d X: %d, Y : %d で交差 \r\n"), j , xLRFLog[j], yLRFLog[j]);
    //                            WriteFile(hFile, out, DWORD(lstrlen(out)), &dwWriteSize, NULL);
    //                        }
    //   
    //                    }
    //                }
    //            }
    //        }
    //    }   
    //    xWalkAdd += xWalkLog[i];
    //    yWalkAdd += yWalkLog[i];
    //   
    //}*/
    
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

/*int ESPSocket(HWND hwnd) {
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
    }
    
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
}*/

int myABS(int n) {
    if (n < 0) n *= -1;
    return n;
}