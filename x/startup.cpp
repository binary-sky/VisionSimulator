#include "addtional_functions.h"


using namespace std;
using namespace cv;

extern void virual_env_game();
uint8_t image_OnChip[_target_hight][_target_width];
vector<string> process_with_chinese;//保存窗口
vector<string> bmp_filenames;
Mat src_orig;
Mat src_gray;
Mat src_cut;
Mat src_blur;
Mat target_img;
Mat img_threshold;
Mat img_result;
Mat image_orig;
void test_program();
void virual_env_game();
void read_picture();
void main_test();

HANDLE hConsole;
void change_console_color(int k) 
{
	SetConsoleTextAttribute(hConsole, k);

}

int main(int argc, char **argv)
{
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);//获取控制台句柄
	main_test();

	/*程序作者：付清旭（哈尔滨工业大学）*/
	cout << "Test Version 2\n";

	change_console_color(224);
	cout<<"\n程序作者：付清旭（哈尔滨工业大学）\n\n\n";
	change_console_color(31);
	cout << "选择你要使用的功能：\n0: 调试软件\n1：游戏仿真\n2：读取已有图片处理\n";
	change_console_color(15);
	int function_selection = -1;
	cin >> function_selection;

	switch (function_selection)
	{
		case 0:
			test_program();
			break;
		case 1:
			virual_env_game();
			break;
		case 2:
			read_picture();
			break;
		default:
			break;
	}
	

}

void test_program()
{



}


void read_picture()
{
	cout << "1秒后，选择图片集所处的文件夹\n";
	Sleep(300); cout << "选择图片集所处的文件夹\n";

	string imagePath = selectPath();

	cout << "稍等，正在对选定文件夹中的图片进行排序！\n";
	find_file_in_path(imagePath, bmp_filenames);


	int count = 0; 
	//int total_count = 0;

	int total_count = bmp_filenames.size();
	namedWindow("原始", WINDOW_NORMAL);
	namedWindow("处理后", WINDOW_NORMAL);

	setMouseCallback("原始", on_Mouse, "原始");
	setMouseCallback("处理后", on_Mouse, "处理后");


	bool already_warned = false;
	while (true)
	{
		string sz2 = bmp_filenames[count];

		cout << "当前文件:" << sz2 << endl;

		image_orig = imread(sz2);

		image_orig.copyTo(img_result);//复制图像
		for (int i = 0; i < image_orig.rows; i++) {//转入八位数组中待处理
			for (int j = 0; j < image_orig.cols; j++) {
				if ((i >= _target_hight || j >= _target_width) )
				{
					already_warned = true;
					if(!already_warned)
						cout << "警告！数组太大，请更改_target_hight和_target_width" << endl;
					continue;
				}
				image_OnChip[i][j] = image_orig.at<uint8_t>(i, j);
			}
		}
		//////////////////////////////////////////////////////////////////
		//////////////////////图像处理程序在此////////////////////////////
		//////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////
		imageProcessOnChipAndOnVS(image_OnChip);//////////////////////////
		//////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////
		for (int i = 0; i < img_result.rows; i++) {//处理完后装入Mat中，待显示
			for (int j = 0; j < img_result.cols; j++) {
				if ((i >= _target_hight || j >= _target_width))
				{
					already_warned = true;
					if (!already_warned)
						cout << "警告！数组太大，请更改_target_hight和_target_width" << endl;
					continue;
				}
				img_result.at<uint8_t>(i, j) = image_OnChip[i][j];
			}
		}



		imshow("原始", image_orig);
		imshow("处理后", img_result);

		cout << "处理结束,A键上一幅，D键下一幅\n";
		int key = waitKey(0);
		switch (key) 
		{
		case 'A':case 'a':
			if(count>0)
				count--;
			break;
		case 'D':case 'd':
			if(count < total_count-1)
				count++;
			break;

		}

	}


}
Mat dul;



void main_test() {
	//cout << "稍等，正在对选定文件夹中的图片进行排序！\n";
	//int count = 0;
	//find_file_in_path("D:\\IMG2", bmp_filenames);

	//namedWindow("原始", WINDOW_NORMAL);
	//namedWindow("模糊", WINDOW_NORMAL);
	//namedWindow("转化灰度和分辨率后的图像", WINDOW_NORMAL);
	//namedWindow("原始二值", WINDOW_NORMAL);
	//int total_count = bmp_filenames.size();

	//while (true)
	//{
	//	string sz2 = bmp_filenames[count];
	//	cout << "当前文件:" << sz2 << endl;
	//	src_orig = imread(sz2);

	//	if (!(src_orig.rows > 0 && src_orig.rows > 0))continue;//图像异常，终止

	//	cvtColor(src_orig, src_gray, CV_BGR2GRAY);//灰度化
	//	Mat src_cut = src_gray;//裁剪

	//	GaussianBlur(src_gray, src_blur, Size(61, 61),20,20);

	//	Mat ele = getStructuringElement(MORPH_RECT, Size(11, 11));//getStructuringElement返回值定义内核矩阵
	//	//erode(src_blur, src_blur, ele);//erode函数直接进行腐蚀操作

	//	threshold(src_blur, dul, 0, 255, CV_THRESH_OTSU);//大津法二值化



	//	//int kenel_size_x = blur_parameter * src_cut.rows / _target_hight; if (kenel_size_x % 2 == 0) kenel_size_x += 1;
	//	//int kenel_size_y = blur_parameter * src_cut.cols / _target_width; if (kenel_size_y % 2 == 0) kenel_size_y += 1;
	//	//int kenel = (kenel_size_x > kenel_size_y) ? kenel_size_x : kenel_size_y;//取较大的当卷积核
	//	//blur(src_cut, src_blur, Size(kenel, kenel));//模糊图像以取得顺滑的边界线

	//	Size size(60, 80);
	//	resize(dul, target_img, size);

	//	//scaleIntervalSampling(dul, target_img,
	//	//	(double)_target_hight / (src_blur.rows),
	//	//	(double)_target_width / (src_blur.cols));//转化为80*60的灰度图
	//	threshold(target_img, img_threshold, 0, 255, CV_THRESH_OTSU);//大津法二值化
	//	img_threshold.copyTo(img_result);//复制图像




	//	if (src_blur.rows > 0 && src_blur.rows > 0) {
	//		if (img_threshold.rows > 0 && img_threshold.rows > 0) {
	//			imshow("原始二值", dul);
	//			
	//			imshow("原始", src_gray);

	//			imshow("模糊", src_blur);//显示采集后模糊的图像

	//			imshow("转化灰度和分辨率后的图像", img_threshold);//显示…………的图像
	//		}
	//	}



	//	cout << "处理结束,A键上一幅，D键下一幅\n";
	//	int key = waitKey(0);
	//	switch (key)
	//	{
	//	case 'A':case 'a':
	//		if (count > 0)
	//			count--;
	//		break;
	//	case 'D':case 'd':
	//		if (count < total_count - 1)
	//			count++;
	//		break;

	//	}

	//}
}

void find_file_in_path(string directoryPath, vector<string> &bmp_filenames)
{
	string pathbase = directoryPath + "\\";
	directoryPath = directoryPath + "\\*";
	

	vector<string> filenames;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(stringToLPCWSTR(directoryPath), &FindFileData);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
				filenames.push_back(LPCWSTR2string(FindFileData.cFileName));
		} while (FindNextFile(hFind, &FindFileData));

		FindClose(hFind);
	}


	for (int i = 0; i < filenames.size(); i++) 
	{
		int temp_minIndex = -1;
		int temp_min = get_string_num(filenames[i]);

		for (int j = i; j < filenames.size(); j++) 
		{
			if (get_string_num(filenames[j]) < temp_min)
			{
				temp_minIndex = j;
				temp_min = get_string_num(filenames[j]);
			}
		}
		if (temp_minIndex > 0) {
			auto t = filenames[i];
			filenames[i] = filenames[temp_minIndex];
			filenames[temp_minIndex] = t;
		}


	}

	for (auto ir = filenames.begin(); ir != filenames.end(); ir++)
	{
		if ((*ir).find(".bmp") != string::npos) {
			bmp_filenames.push_back(pathbase + (*ir));
			cout << "GET " << pathbase + (*ir) << " as BMP file" << endl;
		}
		else if ((*ir).find(".jpg") != string::npos) {
			bmp_filenames.push_back(pathbase + (*ir));
			cout << "GET " << pathbase + (*ir) << " as JPG file" << endl;
		}
		else if ((*ir).find(".png") != string::npos) {
			bmp_filenames.push_back(pathbase + (*ir));
			cout << "GET " << pathbase + (*ir) << " as PNG file" << endl;
		}
	}

}

inline bool isInteger(const std::string & s)
{
	if (s.empty() || s[0]<0 || s[0] > 255|| (((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+')))) return false;

	char * p;
	strtol(s.c_str(), &p, 10);

	return (*p == 0);
}
int get_string_num(string str) {


	std::string delimiter = ".";
	if (str.find(delimiter) != -1) 
	{
		string token = str.substr(0, str.find(delimiter));
		if (isInteger(token)) {
			try {
				int temp = stoi(token);
				return temp;
			}
			catch (std::out_of_range const&) {
				return 99999;
			}
		}
		else
		{
			return 99999;
		}
	}
	else 
	{
		return 99999;
	}
}
//setMouseCallback("处理后", on_Mouse, "处理后");

//	setMouseCallback("原始", on_Mouse, "原始");
void on_Mouse(int event, int x, int y, int flags, void *ustc)//event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号
{
	Point cur_p = Point(-1, -1);//实时坐标  
	Mat img;//临时图像保存
	char tmp[200];
	Mat &img_target_local = img_result;

	/*哪个窗口*/
	const char * cchar = (char *)(ustc);
	if (strcmp(cchar, "处理后") == 0 || strcmp(cchar, "处理后的图像"))
		img_target_local = img_result;
	else if (strcmp(cchar, "原始") == 0)
		img_target_local = image_orig;
	else if (strcmp(cchar, "转化灰度和分辨率后的图像") == 0)
		img_target_local = img_threshold;
	


	if (event == CV_EVENT_MOUSEMOVE)//左键按下，读取初始坐标，并在图像上该点处划圆 
	{
		
		img_target_local.copyTo(img);//将原始图片复制到img中 
		if (x >= 0 && x < img_target_local.cols&&y>=0 && y < img_target_local.rows)
		{
			int row = y;
			int col = x;
			//获取到RGB分量的值。
			uchar B, G, R;
			
			if (img_target_local.dims == 2) 
			{
				B = img_target_local.at<uint8_t>(row, col);
				G = B;
				R = B;
			}
			else 
			{
				Vec3b pix = img_target_local.at<Vec3b>(row, col);
				B = pix[0];
				G = pix[1];
				R = pix[2];

			}
			
		
			sprintf(tmp, "POINT:(%d, %d)->RGB:(%d,%d,%d)                    ", row, col, R, G, B);
			change_console_color(160);
			cout << "\r";
			cout << tmp ;
			change_console_color(15);

		}
	

	}

}