#include "addtional_functions.h"

extern vector<string> process_with_chinese;//保存窗口

string generate_path(string path_base)
{

	static int num = 1;
	while (true) {
		std::fstream file1;
		string path = ((path_base + to_string(num)));
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

string LPCWSTR2string(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

	if (nLen <= 0) return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;

	std::string strTemp(pszDst);
	delete[] pszDst;

	return strTemp;
}

const char* OpenFile()
{
	TCHAR szBuffer[MAX_PATH] = { 0 };
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = _T("BITMAP file(*.bmp)\0*.bmp\0");	// 要选择的文件后缀   
	ofn.lpstrInitialDir = _T("");				// 默认的文件路径   
	ofn.lpstrFile = szBuffer;					// 存放文件的缓冲区   
	ofn.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
	ofn.nFilterIndex = 0;

	//标志如果是多选要加上OFN_ALLOWMULTISELECT  
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	BOOL bSel = GetOpenFileName(&ofn);

	LPWSTR lpwszStrIn = szBuffer;
	LPSTR pszOut = NULL;
	if (lpwszStrIn != NULL)
	{
		int nInputStrLen = wcslen(lpwszStrIn);

		// Double NULL Termination  
		int nOutputStrLen = WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, NULL, 0, 0, 0) + 2;
		pszOut = new char[nOutputStrLen];		//可能会造成内存泄漏

		if (pszOut)
		{
			memset(pszOut, 0x00, nOutputStrLen);
			WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
		}
	}
	return pszOut;
}


const bool SelectOpenFiles(char path[])
{
	TCHAR szBuffer[MAX_PATH] = { 0 };
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = _T("BITMAP file(*.bmp)\0*.bmp\0");	// 要选择的文件后缀   
	ofn.lpstrInitialDir = _T("");				// 默认的文件路径   
	ofn.lpstrFile = szBuffer;					// 存放文件的缓冲区   
	ofn.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
	ofn.nFilterIndex = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ALLOWMULTISELECT;	//标志如果是多选要加上OFN_ALLOWMULTISELECT  
	BOOL bSel = GetOpenFileName(&ofn);
	LPWSTR lpwszStrIn = szBuffer;

	if (lpwszStrIn != NULL)
	{
		int nInputStrLen = wcslen(lpwszStrIn);

		// Double NULL Termination  
		int nOutputStrLen = WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, NULL, 0, 0, 0) + 2;
		if (path)
		{
			memset(path, 0x00, nOutputStrLen);
			WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, path, nOutputStrLen, 0, 0);

			return true;
		}
	}
	return false;
}




string selectPath() {
	string strInitFolder;
	TCHAR szPath[MAX_PATH] = _T("");
	LPMALLOC lpMalloc = NULL;
	string strFolder;
	BROWSEINFO sInfo;
	LPITEMIDLIST lpidlBrowse = NULL;
	if (::SHGetMalloc(&lpMalloc) != NOERROR)
		return strFolder;
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));
	sInfo.pidlRoot = 0;
	sInfo.pszDisplayName = szPath;
	sInfo.lpszTitle = _T("请选择您需要的目的文件夹：");
	sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS;
	sInfo.lpfn = NULL;
	// 显示文件夹选择对话框
	lpidlBrowse = ::SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != NULL) {
		// 取得文件夹名
		if (::SHGetPathFromIDList(lpidlBrowse, szPath)) {
			strFolder = TCHAR2STRING(szPath);
			cout << "选择的文件夹为:" << endl;
			cout << strFolder << endl;
			return strFolder;
		}
	}
	if (lpidlBrowse != NULL) {
		::CoTaskMemFree(lpidlBrowse);
	}
	lpMalloc->Release();
	return strFolder;
}