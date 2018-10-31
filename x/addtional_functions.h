#pragma once
#include <windows.h>
#include "CommDlg.h"
#include "tchar.h"
#include <shlobj.h>
#include <io.h>
#include <iostream>
#include <list>
#include <fstream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


#define CAMERA_ROWS 60
#define CAMERA_COLS 80

#define _target_width  CAMERA_COLS //80*60的灰度图
#define _target_hight  CAMERA_ROWS //80*60的灰度图

using namespace std;
using namespace cv;

Mat hwnd2mat(HWND hwnd);//实时捕获界面
BOOL behas_Chinese(std::string text);//判断窗口中是否含有中文
string TCHAR2STRING(TCHAR *STR);//					TCHAR  ->	string
LPCWSTR stringToLPCWSTR(std::string orig);//		string ->	LPCWSTR
string LPCWSTR2string(LPCWSTR pwszSrc);//				LPCWSTR ->  string

int get_string_num(string str);
void scalePartAverage(const Mat &src, Mat &dst, double xRatio, double yRatio);//转化分辨率
void scaleIntervalSampling(const Mat &src, Mat &dst, double xRatio, double yRatio);//采样法转化分辨率
string generate_path(string s);//设定合适的存储目录
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam); // 回调函数：逐个遍历窗口以查找minecraft窗口
void on_Mouse(int event, int x, int y, int flags, void *ustc);
void find_file_in_path(string directoryPath, vector<string> &bmp_filenames);//搜索图片文件
void change_console_color(int k);
string selectPath();//选择目录
/***************************************************************************/
void imageProcessOnChipAndOnVS(uint8_t(*img)[CAMERA_COLS]);//图像处理核心/***/
/***************************************************************************/


//												char	->	LPCWSTR
/*
	char ch[1024] = "SB c++";
	int num = MultiByteToWideChar(0,0,ch,-1,NULL,0);
	wchar_t *wide = new wchar_t[num];
	MultiByteToWideChar(0,0,ch,-1,wide,num);

	int num = MultiByteToWideChar(0, 0, szFind, -1, NULL, 0);
	wchar_t *szFind_lpcstr = new wchar_t[num];
	MultiByteToWideChar(0, 0, szFind, -1, szFind_lpcstr, num);

													LPCWSTR	->	char
	wchar_t widestr[1024] = L"SB java";
	int num = WideCharToMultiByte(CP_OEMCP,NULL,widestr,-1,NULL,0,NULL,FALSE);
	char *pchar = new char[num];
	WideCharToMultiByte (CP_OEMCP,NULL,widestr,-1,pchar,num,NULL,FALSE);
*/