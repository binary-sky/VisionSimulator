#include "addtional_functions.h"

extern vector<string> process_with_chinese;//保存窗口名字
extern uint8_t image_OnChip[_target_hight][_target_width];/*单片机图像*/
extern Mat src_orig;
extern Mat src_gray;
extern Mat src_cut;
extern Mat src_blur;
extern Mat target_img;
extern Mat img_threshold;
extern Mat img_result;

void keyProcess(int key);
/*************变量*******图像裁剪&去除标题栏*************/
uint32_t left_cut = 0, right_cut = 0, up_cut = 0, down_cut = 0;
float blur_parameter = 10;
bool savePic = false;
bool KeepSaving = false;
string outputname;
int key = 0;//检测到的按键
HWND get_minecraft_window();
void reminder();
void Mat2ChipImg(const Mat target_img);
void ChipImg2Mat();
string path;
string path_base="no";
Mat preprocess_image(Mat src_orig);
void showImageAndSaveThem();
bool showing_result_already_handled = false;
void virual_env_game()
{
	HWND hq = get_minecraft_window();
	MoveWindow(hq, 0, 0, 800, 600, TRUE);//设置游戏窗口位置和长宽
	reminder();
	/*****************创建窗口显示图像*******************/
	namedWindow("模糊和二值化后的图像", WINDOW_NORMAL);
	namedWindow("转化分辨率后的图像", WINDOW_NORMAL);
	namedWindow("处理后的图像", WINDOW_NORMAL);
	moveWindow("模糊和二值化后的图像", 800, 0);
	moveWindow("转化分辨率后的图像", 800, 300);
	moveWindow("处理后的图像", 800, 600);
	setMouseCallback("处理后的图像", on_Mouse, "处理后的图像");
	while (true)/*********************开始办正事**********************/
	{
		src_orig = hwnd2mat(hq);/*游戏画面获取*/
		if (!(src_orig.rows > 0 && src_orig.rows > 0)) continue;/*图像异常，终止*/
		target_img = preprocess_image(src_orig);/*灰度化、裁剪、模糊、二值化、改变大小*/
		/********************************************************************/
		//showing_result_already_handled是个标志位
		//在imageProcessOnChipAndOnVS中调用绘制img_result后，将避免再次绘制
		/********************************************************************/
		Mat2ChipImg(target_img);///////////////////////////////////////////**/
		showing_result_already_handled = false;////////////////////////////**/
		///////////////////////////////////////////////////////////////////**/
		//////////////////////图像处理程序在此/////////////////////////////**/
		///////////////////////////////////////////////////////////////////**/
		///////////////////////////////////////////////////////////////////**/
		imageProcessOnChipAndOnVS(image_OnChip);///////////////////////////**/
		///////////////////////////////////////////////////////////////////**/
		///////////////////////////////////////////////////////////////////**/
		///////////////////////////////////////////////////////////////////**/
		if (!showing_result_already_handled) ChipImg2Mat();////////////////**/
		///////////////////////////////////////////////////////////////////**/
		/********************************************************************/
		/********************************************************************/
		showImageAndSaveThem();

		key = waitKey(10); //等待按键的时间
		keyProcess(key);
	}
}


void Mat2ChipImg(const Mat target_img)
{
	if (target_img.rows == _target_hight && target_img.cols == _target_width) 
	{
		for (int i = 0; i < target_img.rows; i++) {//转入八位数组中待处理
			for (int j = 0; j < target_img.cols; j++) {
				image_OnChip[i][j] = target_img.at<uint8_t>(i, j);
			}
		}
	}
	else
	{
		cout << "something is wrong";
	}
}

void ChipImg2Mat()
{
	for (int i = 0; i < img_result.rows; i++) {//处理完后装入Mat中，待显示
		for (int j = 0; j < img_result.cols; j++) {
			img_result.at<uint8_t>(i, j) = image_OnChip[i][j];
		}
	}

}
void reminder()
{
	/*****************提示按键操作*******************/
	cout << "程序就绪，可用按键可以自定义画面的裁剪区域\n";
	cout << "！在*  ?：画面获取  *窗口按键盘可以调整目标图像范围 ！\n";
	change_console_color(224);
	cout << "(弃用)w键裁剪更多画面上边界（u键相反）\n";
	cout << "(弃用)s键裁剪更多画面下边界（j键相反）\n";
	cout << "(弃用)a键裁剪更多画面左边界（h键相反）\n";
	cout << "(弃用)d键裁剪更多画面右边界（k键相反）\n";
	cout << ">键加重模糊处理平滑边界（<键相反）\n";
	cout << "*键保存一张图像\n";
	cout << "+键开始持续保存图像（-键终止保存）\n";
	cout << "\n！务必在*  ?：画面获取  *窗口按键盘 ！\n";
	change_console_color(15);
}
void keyProcess(int key)
{

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
		if (up_cut > 0)
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
HWND get_minecraft_window()
{
	EnumWindows(EnumWindowsProc, 0); // 枚举窗口：逐个遍历窗口以查找minecraft窗口

/*************************************************/
	int cnt = 1;
	for (auto i = process_with_chinese.begin(); i != process_with_chinese.end(); ++i)
	{
		cout << "窗口编号" << cnt++ << "是：     " << *i << "\n";
	}
	change_console_color(224);
	cout << "选择其一进行监视(输入数字窗口编号)：\n";
	change_console_color(15);
	/*****************输入选择的界面*******************/
	int x = -1;
	cin >> x;
	string t;
	if (x > 0 && x <= process_with_chinese.size()) {
		t = process_with_chinese[x - 1];
	}
	if (x == -2) {//debug uses
		t = "Minecraft 1.12.2";
	}

	cout << "你已选定窗口：" << t << endl;
	cout << "请保持窗口不要最小化！\n请不要最小化！\n\n\n\n";

	/*****************获取窗口句柄：hq*******************/
	char name[500];
	strcpy(name, t.c_str());
	int num = MultiByteToWideChar(0, 0, name, -1, NULL, 0);
	wchar_t *name2 = new wchar_t[num];
	MultiByteToWideChar(0, 0, name, -1, name2, num);
	outputname = t + "：画面获取" + "模糊后";
	return FindWindow(NULL, name2);

}

void img_result_light(int row, int col, int r = 80, int g = 125, int b = 40);
void img_result_fill(uint8_t(*image)[_target_width]);


void img_result_light(int row, int col, int r , int g , int b )
{
	if (row >= 0 && row < img_result.rows&&col >= 0 && col < img_result.cols) {
		Vec3b vv(r, g, b);
		img_result.at<Vec3b>(row, col) = vv;
		showing_result_already_handled = true;
	}

}
void img_result_fill(uint8_t(*image)[_target_width])
{
	for (int r = 0; r < _target_hight; r++) {
		for (int c = 0; c < _target_width; c++) {
			int value = image[r][c];
			img_result_light(r, c, value, value, value);
		}
	}
}


Mat preprocess_image(Mat src_orig)
{

	cvtColor(src_orig, src_gray, CV_BGR2GRAY);//灰度化
	//(弃用)Mat src_cut = src_gray(Range(up_cut, src_gray.rows - 1 - down_cut), Range(left_cut, src_gray.cols - 1 - right_cut));//裁剪
	src_cut = src_gray;

	/*计算高斯滤波核大小*///模糊图像以取得顺滑的边界线
	int kenel_size_x = 4 * src_cut.rows / _target_hight; if (kenel_size_x % 2 == 0) kenel_size_x += 1;
	int kenel_size_y = 4 * src_cut.cols / _target_width; if (kenel_size_y % 2 == 0) kenel_size_y += 1;
	int kenel = (kenel_size_x > kenel_size_y) ? kenel_size_x : kenel_size_y;//取较大的当卷积核

	GaussianBlur(src_cut, src_blur, Size(kenel, kenel), blur_parameter);

	threshold(src_blur, img_threshold, 0, 255, CV_THRESH_OTSU);//大津法二值化
	Size size(_target_width, _target_hight);
	resize(img_threshold, target_img, size, NULL, NULL, INTER_AREA);
	threshold(target_img, target_img, 125, 255, CV_THRESH_BINARY);

	cvtColor(target_img, img_result, cv::COLOR_GRAY2BGR);//复制图像


	return target_img;
	//target_img is the result
}
void showImageAndSaveThem()
{
	if (src_blur.rows > 0 && src_blur.rows > 0) {
		imshow("模糊和二值化后的图像", img_threshold);//显示采集后模糊的图像
	}
	if (img_threshold.rows > 0 && img_threshold.rows > 0) {
		imshow("转化分辨率后的图像", target_img);//显示…………的图像
	}
	if (img_result.rows > 0 && img_result.rows > 0) {
		imshow("处理后的图像", img_result);//显示结果
	}
	if (savePic || KeepSaving)
	{
		if (path_base == "no")
		{
			path_base = selectPath();
			path_base = path_base + "\\";
		}
		path = generate_path(path_base);
		imwrite(path, img_threshold);
		if (savePic)savePic = !savePic;
	}

}