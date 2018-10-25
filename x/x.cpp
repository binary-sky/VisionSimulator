#include <windows.h>
#include <iostream>
#include <list>
#include <fstream>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "extVarContainer.h"

using namespace std;
using namespace cv;

#define _target_width  80 //80*60的灰度图
#define _target_hight  60 //80*60的灰度图

uint8_t image_target[_target_hight][_target_width];


Mat hwnd2mat(HWND hwnd);//实时捕获界面
BOOL behas_Chinese(std::string text);//判断窗口中是否含有中文
string TCHAR2STRING(TCHAR *STR);//数据类型转化
vector<string> process_with_chinese;//保存窗口
LPCWSTR stringToLPCWSTR(std::string orig);//数据类型转化
void scalePartAverage(const Mat &src, Mat &dst, double xRatio, double yRatio);//转化分辨率
void scaleIntervalSampling(const Mat &src, Mat &dst, double xRatio, double yRatio);//采样法转化分辨率
string generate_path();

int imageProcessOnChipAndOnVS(uint8_t(*img)[CAMERA_COLS]);

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) // 回调函数：逐个遍历窗口以查找minecraft窗口
{
	TCHAR szTitle[200];
	TCHAR szClass[200];
	GetWindowText(hwnd, szTitle, sizeof(szTitle) / sizeof(TCHAR)); // 获取窗口名称
	GetClassName(hwnd, szClass, sizeof(szClass) / sizeof(TCHAR)); // 窗口类
	string str = TCHAR2STRING(szTitle);
	auto idx = str.find("Minecraft");//满足要求
	if (idx == string::npos);
	else {
		process_with_chinese.push_back(str);
		return TRUE;
	}

	idx = str.find("minecraft");//满足要求
	if (idx == string::npos);
	else {
		process_with_chinese.push_back(str);
		return TRUE;
	}

	if (behas_Chinese(str)) //窗口名字中含有中文
	{
		process_with_chinese.push_back(str);
		return TRUE;
	}
	return TRUE;
}

int main(int argc, char **argv)
{
	/*程序作者：付清旭（哈尔滨工业大学）*/
	cout<<"程序作者：付清旭（哈尔滨工业大学）\n";
	EnumWindows(EnumWindowsProc, 0); // 枚举窗口：逐个遍历窗口以查找minecraft窗口
	
	/*************************************************/
	int cnt = 1;
	for (auto i = process_with_chinese.begin(); i != process_with_chinese.end(); ++i) 
	{
		cout << "窗口编号" << cnt++ << "是：     " << *i << "\n";
	}
	cout << "选择其一进行监视(输入数字窗口编号)：\n";

	/*****************输入选择的界面*******************/
	int x = -1;
	cin >> x;
	string t;
	if ( x>0 && x <= process_with_chinese.size() ) {
		t = process_with_chinese[x-1];
	}
	if (x == -2) {//debug uses
		t = "Minecraft 1.12.2";
	}
	cout << "你已选定窗口：" << t << endl ;
	cout << "请保持窗口不要最小化！\n请不要最小化！\n\n\n\n";
	
	/*****************获取窗口句柄：hq*******************/
	char name[500];
	strcpy(name, t.c_str());
	int num = MultiByteToWideChar(0, 0, name, -1, NULL, 0);
	wchar_t *name2 = new wchar_t[num];
	MultiByteToWideChar(0, 0, name, -1, name2, num);
	HWND hq = FindWindow(NULL, name2);


	/*****************提示按键操作*******************/
	cout << "程序就绪，可用按键可以自定义画面的裁剪区域\n";
	cout << "！在*  ?：画面获取  *窗口按键盘可以调整目标图像范围 ！\n";
	cout << "w键裁剪更多画面上边界（u键相反）\n";
	cout << "s键裁剪更多画面下边界（j键相反）\n";
	cout << "a键裁剪更多画面左边界（h键相反）\n";
	cout << "d键裁剪更多画面右边界（k键相反）\n";
	cout << ">键加重模糊处理平滑边界（<键相反）\n";
	cout << "*键保存一张图像\n";
	cout << "+键开始持续保存图像（-键终止保存）\n";
	cout << "！务必在*  ?：画面获取  *窗口按键盘 ！\n";



	/*****************创建窗口显示图像*******************/
	string outputname = t + "：画面获取"+"模糊后";
	namedWindow(outputname, WINDOW_NORMAL);
	namedWindow("转化灰度和分辨率后的图像", WINDOW_NORMAL);
	namedWindow("处理后的图像", WINDOW_NORMAL);

	
	int key = 0;//检测到的按键
	CreateDirectory(L"\\IMG", NULL);


	Mat src_orig;
	Mat src_gray;
	Mat src_cut;
	Mat src_blur;
	Mat target_img;
	Mat img_threshold;
	Mat img_result;

	/********************图像裁剪，去除标题栏*************/
	uint32_t left_cut = 0, right_cut =0,up_cut=0,down_cut=0;
	float blur_parameter = 4;
	bool savePic = false;
	bool KeepSaving = false;
	/*********************开始办正事**********************/
	while (key != 27)
	{

		src_orig = hwnd2mat(hq);
		if (!(src_orig.rows > 0 && src_orig.rows > 0))continue;//图像异常，终止

		cvtColor(src_orig, src_gray, CV_BGR2GRAY);//灰度化

		Mat src_cut = src_gray(Range(up_cut, src_gray.rows-1-down_cut), Range(left_cut, src_gray.cols-1-right_cut));//裁剪


		int kenel_size_x = blur_parameter * src_cut.rows / _target_hight; if (kenel_size_x % 2 == 0) kenel_size_x += 1;
		int kenel_size_y = blur_parameter * src_cut.cols / _target_width; if (kenel_size_y % 2 == 0) kenel_size_y += 1;
		int kenel = (kenel_size_x > kenel_size_y) ? kenel_size_x : kenel_size_y;//取较大的当卷积核
		blur(src_cut, src_blur, Size(kenel, kenel));//模糊图像以取得顺滑的边界线
		scaleIntervalSampling(src_blur, target_img,
			(double)_target_hight / (src_blur.rows),
			(double)_target_width / (src_blur.cols));//转化为80*60的灰度图
		threshold(target_img, img_threshold, 0, 255, CV_THRESH_OTSU);//大津法二值化
		img_threshold.copyTo(img_result);//复制图像

		if (src_blur.rows > 0 && src_blur.rows > 0) {
			imshow(outputname, src_blur);//显示采集后模糊的图像
		}
		if (img_threshold.rows > 0 && img_threshold.rows > 0) {
			imshow("转化灰度和分辨率后的图像", img_threshold);//显示…………的图像
		}

		for (int i = 0; i < img_threshold.rows; i++) {//转入八位数组中待处理
			for (int j = 0; j < img_threshold.cols; j++) {
				image_target[i][j] = img_threshold.at<uint8_t>(i, j);
			}
		}
		/**********************图像处理接口***************************/
		imageProcessOnChipAndOnVS(image_target);
		/**********************图像处理接口***************************/

		for (int i = 0; i < img_result.rows; i++) {//处理完后装入Mat中，待显示
			for (int j = 0; j < img_result.cols; j++) {
				img_result.at<uint8_t>(i, j)=image_target[i][j];
			}
		}
		if (img_result.rows > 0 && img_result.rows > 0) {
			imshow("处理后的图像", img_result);//显示结果
		}
		if (savePic||KeepSaving)
		{
			string path = generate_path();
			imwrite(path,img_threshold);
			if (savePic)savePic = !savePic;
		}

		key = waitKey(10); // you can change wait time
		switch (key) {
			case -1:
				break;
			case'W':case'w':
				up_cut++; 
				break;
			case'S':case's':
				down_cut++;
				break;
			case'A':case'a':
				left_cut++;
				break;
			case'D':case'd':
				right_cut++;
				break;

			case'U':case'u':
				if(up_cut>0)
					up_cut--;
				break;
			case'J':case'j':
				if (down_cut > 0)
					down_cut--;
				break;
			case'H':case'h':
				if (left_cut > 0)
					left_cut--;
				break;
			case'K':case'k':
				if (right_cut > 0)
					right_cut--;
				break;
			case ',': case'<':
				if (blur_parameter > 0.5)
					blur_parameter -= 0.1;
				cout << "模糊减小，当前blur_parameter=" << blur_parameter << endl;
				break;
			case '.': case'>':
					blur_parameter += 0.1;
				cout << "模糊增加，当前blur_parameter=" << blur_parameter << endl;
				break;

			case '+':
				cout << "开始持续保存\n";
				KeepSaving = true;
				break;
			case '-':
				cout << "停止持续保存\n";
				KeepSaving = false;
				break;
			case '*':
				cout << "保存单张\n";
				savePic = true;
				break;
		}


	}

}

string generate_path() 
{
	static int num = 1;
	while (true) {
		std::fstream file1;
		string path = ("\\IMG\\" + to_string(num));
		path = path + ".bmp";
		file1.open(path, ios::in);

		if (!file1)
		{
			file1.close();
			return path;
		}
		else 
		{
			num++;
		}
	}
	

}

Mat hwnd2mat(HWND hwnd)
{

	HDC hwindowDC, hwindowCompatibleDC;
	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	Mat src;
	BITMAPINFOHEADER  bi;
	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);
	RECT windowsize;    
	GetClientRect(hwnd, &windowsize);// get the height and width of the screen
	srcheight = windowsize.bottom;
	srcwidth = windowsize.right;
	height = windowsize.bottom / 1;  //change this to whatever size you want to resize to
	width = windowsize.right / 1;

	static bool showonce = true;
	if (showonce && height > 10 && width > 10)
	{
		cout << "窗口尺寸" << width << "*" << height;
		showonce = false;
	}
	//else 
	//{
	//	cout << "Oppps!窗口尺寸好奇怪显示不了\n";
	//	Mat x;
	//	return x;
	//}
	src.create(height, width, CV_8UC4);
	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);   
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow
	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);
	return src;
}

void scaleIntervalSampling(const Mat &src, Mat &dst, double xRatio, double yRatio)
{
	//只处理uchar型的像素
	CV_Assert(src.depth() == CV_8U);

	// 计算缩小后图像的大小
	//没有四舍五入，防止对原图像采样时越过图像边界
	int rows = static_cast<int>(src.rows * xRatio);
	int cols = static_cast<int>(src.cols * yRatio);

	dst.create(rows, cols, src.type());

	const int channesl = src.channels();

	switch (channesl)
	{
	case 1: //单通道图像
	{
		uchar *p;
		const uchar *origal;

		for (int i = 0; i < rows; i++) {
			p = dst.ptr<uchar>(i);
			//四舍五入
			//+1 和 -1 是因为Mat中的像素是从0开始计数的
			int row = static_cast<int>((i + 1) / xRatio + 0.5) - 1;
			origal = src.ptr<uchar>(row);
			for (int j = 0; j < cols; j++) {
				int col = static_cast<int>((j + 1) / yRatio + 0.5) - 1;
				p[j] = origal[col];  //取得采样像素
			}
		}
		break;
	}

	case 3://三通道图像
	{
		Vec3b *p;
		const Vec3b *origal;

		for (int i = 0; i < rows; i++) {
			p = dst.ptr<Vec3b>(i);
			int row = static_cast<int>((i + 1) / xRatio + 0.5) - 1;
			origal = src.ptr<Vec3b>(row);
			for (int j = 0; j < cols; j++) {
				int col = static_cast<int>((j + 1) / yRatio + 0.5) - 1;
				p[j] = origal[col]; //取得采样像素
			}
		}
		break;
	}
	}
}
void average(const Mat &img, Point_<int> a, Point_<int> b, Vec3b &p);
void scalePartAverage(const Mat &src, Mat &dst, double xRatio, double yRatio)
{
	int rows = static_cast<int>(src.rows * xRatio);
	int cols = static_cast<int>(src.cols * yRatio);

	dst.create(rows, cols, src.type());

	int lastRow = 0;
	int lastCol = 0;

	Vec3b *p;
	for (int i = 0; i < rows; i++) {
		p = dst.ptr<Vec3b>(i);
		int row = static_cast<int>((i + 1) / xRatio + 0.5) - 1;

		for (int j = 0; j < cols; j++) {
			int col = static_cast<int>((j + 1) / yRatio + 0.5) - 1;

			Vec3b pix;
			average(src, Point_<int>(lastRow, lastCol), Point_<int>(row, col), pix);
			p[j] = pix;

			lastCol = col + 1; //下一个子块左上角的列坐标，行坐标不变
		}
		lastCol = 0; //子块的左上角列坐标，从0开始
		lastRow = row + 1; //子块的左上角行坐标
	}
}
void average(const Mat &img, Point_<int> a, Point_<int> b, Vec3b &p)
{

	const Vec3b *pix;
	Vec3i temp;
	for (int i = a.x; i <= b.x; i++) {
		pix = img.ptr<Vec3b>(i);
		for (int j = a.y; j <= b.y; j++) {
			temp[0] += pix[j][0];
			temp[1] += pix[j][1];
			temp[2] += pix[j][2];
		}
	}

	int count = (b.x - a.x + 1) * (b.y - a.y + 1);
	p[0] = temp[0] / count;
	p[1] = temp[1] / count;
	p[2] = temp[2] / count;
}




string TCHAR2STRING(TCHAR *STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	return str;
}
BOOL behas_Chinese(std::string text)
{
	// 根据字符，来选择编码方式
	BOOL bHasChinese = FALSE;
	for (int i = 0; i < text.length(); i++)
	{
		if (text[i] < 0) // 0xBO 10110000 0xA1 10100000 第一位都是1，表示是负数
		{
			bHasChinese = TRUE;
			break;
		}
	}
	return bHasChinese;
}
//string转换车wstring
LPCWSTR stringToLPCWSTR(std::string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t *wcstring = (wchar_t *)malloc(sizeof(wchar_t)*(orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);

	return wcstring;
}