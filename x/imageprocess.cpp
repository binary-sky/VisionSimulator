
//下面几个头文件使得该文件（imageprocess）可以跨越单片机和电脑，复制粘贴后即可运行在单片机上
//请在单片机和电脑上编写不同的"environment.h"和"extVarContainer.h"
#include <stdint.h>		
#include "environment.h"		//指示了当前的环境
#include "extVarContainer.h"	//存放了单片机上一些不能转移的外部变量，例如extern float speed;访问虚拟的车速



int imageProcessOnChipAndOnVS(uint8_t (*img)[CAMERA_COLS])
{

	for (int i = 10; i < CAMERA_ROWS - 10; i++)//测试：在图像中画一条竖线
	{
		for (int j = 10; j < CAMERA_COLS - 10; j++)
		{
			if (j == 40)
			{
				img[i][j] = 125-j;
			}
		}
	}
	return 0;
}

