#include "addtional_functions.h"

extern vector<string> process_with_chinese;//���洰������
extern uint8_t image_OnChip[_target_hight][_target_width];/*��Ƭ��ͼ��*/
extern Mat src_orig;
extern Mat src_gray;
extern Mat src_cut;
extern Mat src_blur;
extern Mat target_img;
extern Mat img_threshold;
extern Mat img_result;

void keyProcess(int key);
/*************����*******ͼ��ü�&ȥ��������*************/
uint32_t left_cut = 0, right_cut = 0, up_cut = 0, down_cut = 0;
float blur_parameter = 10;
bool savePic = false;
bool KeepSaving = false;
string outputname;
int key = 0;//��⵽�İ���
HWND get_minecraft_window();
void reminder();
void Mat2ChipImg(const Mat target_img);
void ChipImg2Mat();
string path;
string path_base="no";
Mat preprocess_image(Mat src_orig)
{
	
	cvtColor(src_orig, src_gray, CV_BGR2GRAY);//�ҶȻ�
	//(����)Mat src_cut = src_gray(Range(up_cut, src_gray.rows - 1 - down_cut), Range(left_cut, src_gray.cols - 1 - right_cut));//�ü�
	src_cut = src_gray;

	/*�����˹�˲��˴�С*///ģ��ͼ����ȡ��˳���ı߽���
	int kenel_size_x = 4 * src_cut.rows / _target_hight; if (kenel_size_x % 2 == 0) kenel_size_x += 1;
	int kenel_size_y = 4 * src_cut.cols / _target_width; if (kenel_size_y % 2 == 0) kenel_size_y += 1;
	int kenel = (kenel_size_x > kenel_size_y) ? kenel_size_x : kenel_size_y;//ȡ�ϴ�ĵ�������

	GaussianBlur(src_cut, src_blur, Size(kenel, kenel), blur_parameter);

	threshold(src_blur, img_threshold, 0, 255, CV_THRESH_OTSU);//��򷨶�ֵ��
	Size size(_target_width, _target_hight);
	resize(img_threshold, target_img, size,NULL,NULL, INTER_AREA);
	threshold(target_img, target_img, 125 , 255 , CV_THRESH_BINARY);
	target_img.copyTo(img_result);//����ͼ��



	return target_img;
	//target_img is the result
}
void showImageAndSaveThem() 
{
	if (src_blur.rows > 0 && src_blur.rows > 0) {
		imshow("ģ���Ͷ�ֵ�����ͼ��", img_threshold);//��ʾ�ɼ���ģ����ͼ��
	}
	if (img_threshold.rows > 0 && img_threshold.rows > 0) {
		imshow("ת���ֱ��ʺ��ͼ��", target_img);//��ʾ����������ͼ��
	}
	if (img_result.rows > 0 && img_result.rows > 0) {
		imshow("�������ͼ��", img_result);//��ʾ���
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
void virual_env_game()
{
	HWND hq = get_minecraft_window();
	MoveWindow(hq, 0, 0, 800, 600, TRUE);//������Ϸ����λ�úͳ���
	reminder();
	/*****************����������ʾͼ��*******************/
	namedWindow("ģ���Ͷ�ֵ�����ͼ��", WINDOW_NORMAL);
	namedWindow("ת���ֱ��ʺ��ͼ��", WINDOW_NORMAL);
	namedWindow("�������ͼ��", WINDOW_NORMAL);
	moveWindow("ģ���Ͷ�ֵ�����ͼ��", 800, 0);
	moveWindow("ת���ֱ��ʺ��ͼ��", 800, 300);
	moveWindow("�������ͼ��", 800, 600);
	setMouseCallback("�������ͼ��", on_Mouse, "�������ͼ��");
	while (true)/*********************��ʼ������**********************/
	{
		src_orig = hwnd2mat(hq);/*��Ϸ�����ȡ*/
		if (!(src_orig.rows > 0 && src_orig.rows > 0)) continue;/*ͼ���쳣����ֹ*/
		target_img = preprocess_image(src_orig);/*�ҶȻ����ü���ģ������ֵ�����ı��С*/
		/********************************************************************/
		/********************************************************************/
		Mat2ChipImg(target_img);///////////////////////////////////////////**/
		///////////////////////////////////////////////////////////////////**/
		//////////////////////ͼ���������ڴ�/////////////////////////////**/
		///////////////////////////////////////////////////////////////////**/
		///////////////////////////////////////////////////////////////////**/
		imageProcessOnChipAndOnVS(image_OnChip);///////////////////////////**/
		///////////////////////////////////////////////////////////////////**/
		///////////////////////////////////////////////////////////////////**/
		///////////////////////////////////////////////////////////////////**/
		ChipImg2Mat();/////////////////////////////////////////////////////**/
		/********************************************************************/
		/********************************************************************/
		showImageAndSaveThem();

		key = waitKey(10); //�ȴ�������ʱ��
		keyProcess(key);
	}
}


void Mat2ChipImg(const Mat target_img)
{
	if (target_img.rows == _target_hight && target_img.cols == _target_width) 
	{
		for (int i = 0; i < target_img.rows; i++) {//ת���λ�����д�����
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
	for (int i = 0; i < img_result.rows; i++) {//�������װ��Mat�У�����ʾ
		for (int j = 0; j < img_result.cols; j++) {
			img_result.at<uint8_t>(i, j) = image_OnChip[i][j];
		}
	}

}
void reminder()
{
	/*****************��ʾ��������*******************/
	cout << "������������ð��������Զ��廭��Ĳü�����\n";
	cout << "����*  ?�������ȡ  *���ڰ����̿��Ե���Ŀ��ͼ��Χ ��\n";
	change_console_color(224);
	cout << "(����)w���ü����໭���ϱ߽磨u���෴��\n";
	cout << "(����)s���ü����໭���±߽磨j���෴��\n";
	cout << "(����)a���ü����໭����߽磨h���෴��\n";
	cout << "(����)d���ü����໭���ұ߽磨k���෴��\n";
	cout << ">������ģ������ƽ���߽磨<���෴��\n";
	cout << "*������һ��ͼ��\n";
	cout << "+����ʼ��������ͼ��-����ֹ���棩\n";
	cout << "\n�������*  ?�������ȡ  *���ڰ����� ��\n";
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
		cout << "ģ����С����ǰblur_parameter=" << blur_parameter << endl;
		break;
	case '.': case'>':
		blur_parameter += 0.1;
		cout << "ģ�����ӣ���ǰblur_parameter=" << blur_parameter << endl;
		break;

	case '+':
		cout << "��ʼ��������\n";
		KeepSaving = true;
		break;
	case '-':
		cout << "ֹͣ��������\n";
		KeepSaving = false;
		break;
	case '*':
		cout << "���浥��\n";
		savePic = true;
		break;
	}
}
HWND get_minecraft_window()
{
	EnumWindows(EnumWindowsProc, 0); // ö�ٴ��ڣ�������������Բ���minecraft����

/*************************************************/
	int cnt = 1;
	for (auto i = process_with_chinese.begin(); i != process_with_chinese.end(); ++i)
	{
		cout << "���ڱ��" << cnt++ << "�ǣ�     " << *i << "\n";
	}
	change_console_color(224);
	cout << "ѡ����һ���м���(�������ִ��ڱ��)��\n";
	change_console_color(15);
	/*****************����ѡ��Ľ���*******************/
	int x = -1;
	cin >> x;
	string t;
	if (x > 0 && x <= process_with_chinese.size()) {
		t = process_with_chinese[x - 1];
	}
	if (x == -2) {//debug uses
		t = "Minecraft 1.12.2";
	}

	cout << "����ѡ�����ڣ�" << t << endl;
	cout << "�뱣�ִ��ڲ�Ҫ��С����\n�벻Ҫ��С����\n\n\n\n";

	/*****************��ȡ���ھ����hq*******************/
	char name[500];
	strcpy(name, t.c_str());
	int num = MultiByteToWideChar(0, 0, name, -1, NULL, 0);
	wchar_t *name2 = new wchar_t[num];
	MultiByteToWideChar(0, 0, name, -1, name2, num);
	outputname = t + "�������ȡ" + "ģ����";
	return FindWindow(NULL, name2);

}
